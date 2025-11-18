#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <ctime>
#include <sstream>
#include <iomanip>

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
Transaction createTransaction(double amount, const std::string& description) {
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

int main(int argc, char* argv[]) {
    int sock = 0;
    struct sockaddr_in serv_addr;
    const char* server_host = "server";  // Nombre del servicio en docker-compose
    const int PORT = 8080;
    
    // Crear socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Error al crear socket" << std::endl;
        return -1;
    }
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    
    // Resolver hostname a IP (en Docker, "server" se resuelve automáticamente)
    // Intentar primero con el nombre del servicio
    struct hostent* host_entry = gethostbyname(server_host);
    if (host_entry == NULL) {
        // Si falla, intentar con localhost
        host_entry = gethostbyname("localhost");
        if (host_entry == NULL) {
            std::cerr << "Error al resolver hostname" << std::endl;
            return -1;
        }
    }
    
    memcpy(&serv_addr.sin_addr, host_entry->h_addr_list[0], host_entry->h_length);
    
    // Conectar al servidor
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Error al conectar con el servidor" << std::endl;
        std::cerr << "Intentando con localhost..." << std::endl;
        
        // Intentar con localhost
        if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
            return -1;
        }
        
        if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
            std::cerr << "Error al conectar con localhost" << std::endl;
            return -1;
        }
    }
    
    std::cout << "Conectado al servidor de transacciones" << std::endl;
    
    // Crear una transacción de ejemplo
    double amount = 100000.0;
    std::string description = "Transferencia entre contenedores";
    
    if (argc >= 2) {
        amount = std::stod(argv[1]);
    }
    if (argc >= 3) {
        description = argv[2];
    }
    
    Transaction txn = createTransaction(amount, description);
    
    std::cout << "\n=== Transacción Creada ===" << std::endl;
    std::cout << "ID: " << txn.id << std::endl;
    std::cout << "Timestamp: " << txn.timestamp << std::endl;
    std::cout << "Monto: $" << txn.amount << std::endl;
    std::cout << "Descripción: " << txn.description << std::endl;
    std::cout << "Token: " << txn.token << std::endl;
    std::cout << "========================\n" << std::endl;
    
    // Serializar y enviar transacción
    std::string transactionData = serializeTransaction(txn);
    send(sock, transactionData.c_str(), transactionData.length(), 0);
    std::cout << "Transacción enviada al servidor" << std::endl;
    
    // Recibir respuesta
    char buffer[1024] = {0};
    int valread = read(sock, buffer, 1024);
    
    if (valread > 0) {
        std::string response(buffer);
        std::cout << "Respuesta del servidor: " << response << std::endl;
    }
    
    close(sock);
    return 0;
}

