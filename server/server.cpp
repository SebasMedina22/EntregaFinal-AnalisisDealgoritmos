#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <random>
#include <map>

// Estructura para representar una transacción
struct Transaction {
    std::string id;
    std::string timestamp;
    double amount;
    std::string description;
    std::string token;
};

// Generador de tokens dinámicos (clave dinámica)
class TokenGenerator {
private:
    std::string secretKey;
    
public:
    TokenGenerator() {
        secretKey = "BANCOLOMBIA_SECRET_2025";
    }
    
    // Genera un token basado en timestamp y secret key
    std::string generateToken(const std::string& transactionId, const std::string& timestamp) {
        // Algoritmo simple de hash para generar token
        // En producción, usar algoritmos más seguros como HMAC-SHA256
        std::string data = transactionId + timestamp + secretKey;
        unsigned int hash = 0;
        
        for (char c : data) {
            hash = hash * 31 + c;
        }
        
        // Convertir a string hexadecimal
        std::stringstream ss;
        ss << std::hex << hash;
        return ss.str();
    }
    
    // Valida un token
    bool validateToken(const std::string& token, const std::string& transactionId, const std::string& timestamp) {
        std::string expectedToken = generateToken(transactionId, timestamp);
        return token == expectedToken;
    }
};

// Generador de IDs únicos para transacciones
class TransactionIDGenerator {
private:
    int counter;
    
public:
    TransactionIDGenerator() : counter(0) {}
    
    std::string generateID() {
        std::time_t now = std::time(0);
        std::stringstream ss;
        ss << "TXN-" << std::put_time(std::localtime(&now), "%Y%m%d") 
           << "-" << std::setfill('0') << std::setw(6) << ++counter;
        return ss.str();
    }
};

// Obtiene el timestamp actual en formato string
std::string getCurrentTimestamp() {
    std::time_t now = std::time(0);
    std::tm* timeinfo = std::localtime(&now);
    char buffer[80];
    std::strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", timeinfo);
    return std::string(buffer);
}

// Procesa una transacción y genera el token
Transaction processTransaction(double amount, const std::string& description) {
    TransactionIDGenerator idGen;
    TokenGenerator tokenGen;
    
    Transaction txn;
    txn.id = idGen.generateID();
    txn.timestamp = getCurrentTimestamp();
    txn.amount = amount;
    txn.description = description;
    txn.token = tokenGen.generateToken(txn.id, txn.timestamp);
    
    return txn;
}

// Serializa una transacción a string
std::string serializeTransaction(const Transaction& txn) {
    std::stringstream ss;
    ss << txn.id << "|" << txn.timestamp << "|" 
       << txn.amount << "|" << txn.description << "|" << txn.token;
    return ss.str();
}

// Deserializa una transacción desde string
Transaction deserializeTransaction(const std::string& data) {
    Transaction txn;
    std::stringstream ss(data);
    std::string item;
    
    std::getline(ss, item, '|');
    txn.id = item;
    
    std::getline(ss, item, '|');
    txn.timestamp = item;
    
    std::getline(ss, item, '|');
    txn.amount = std::stod(item);
    
    std::getline(ss, item, '|');
    txn.description = item;
    
    std::getline(ss, item, '|');
    txn.token = item;
    
    return txn;
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    const int PORT = 8080;
    
    // Crear socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        std::cerr << "Error al crear socket" << std::endl;
        return -1;
    }
    
    // Configurar opciones del socket
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        std::cerr << "Error al configurar socket" << std::endl;
        return -1;
    }
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    
    // Bind
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        std::cerr << "Error en bind" << std::endl;
        return -1;
    }
    
    // Listen
    if (listen(server_fd, 3) < 0) {
        std::cerr << "Error en listen" << std::endl;
        return -1;
    }
    
    std::cout << "Servidor de transacciones iniciado en puerto " << PORT << std::endl;
    std::cout << "Esperando conexiones..." << std::endl;
    
    TokenGenerator tokenGen;
    
    while (true) {
        // Aceptar conexión
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            std::cerr << "Error al aceptar conexión" << std::endl;
            continue;
        }
        
        std::cout << "Cliente conectado desde " << inet_ntoa(address.sin_addr) << std::endl;
        
        char buffer[1024] = {0};
        int valread = read(new_socket, buffer, 1024);
        
        if (valread > 0) {
            std::string receivedData(buffer);
            std::cout << "Datos recibidos: " << receivedData << std::endl;
            
            // Deserializar transacción
            Transaction receivedTxn = deserializeTransaction(receivedData);
            
            // Validar token
            bool isValid = tokenGen.validateToken(receivedTxn.token, receivedTxn.id, receivedTxn.timestamp);
            
            std::string response;
            if (isValid) {
                response = "TRANSACCION_VALIDA|Token verificado correctamente|ID: " + receivedTxn.id;
                std::cout << "Transacción válida: " << receivedTxn.id << std::endl;
            } else {
                response = "TRANSACCION_INVALIDA|Token no válido|ID: " + receivedTxn.id;
                std::cerr << "Transacción inválida: " << receivedTxn.id << std::endl;
            }
            
            send(new_socket, response.c_str(), response.length(), 0);
        }
        
        close(new_socket);
    }
    
    close(server_fd);
    return 0; 
}