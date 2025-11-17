# Sistema de Transacciones con Clave Dinámica

## Descripción

Sistema de transacciones seguro que implementa un mecanismo de clave dinámica mediante tokens, similar al sistema utilizado por Bancolombia. El sistema permite la comunicación segura entre contenedores Docker mediante sockets.

## Arquitectura

El sistema consta de dos contenedores Docker que se comunican mediante sockets TCP:

- **Contenedor Cliente**: Genera transacciones con tokens dinámicos y las envía al servidor
- **Contenedor Servidor**: Recibe, valida y procesa las transacciones

Ver documentación detallada en:
- [Arquitectura del Sistema](docs/ARQUITECTURA.md)
- [Sistema de Seguridad](docs/SEGURIDAD.md)
- [Diagramas](docs/DIAGRAMA.md)

## Requisitos

- Docker
- Docker Compose
- Compilador C++ (g++)

## Instalación y Despliegue

### Prerrequisitos

1. Instalar Docker: https://docs.docker.com/get-docker/
2. Instalar Docker Compose: https://docs.docker.com/compose/install/
3. (Opcional) Si usas otro usuario de DockerHub distinto a `sebas22x`, exporta la variable `DOCKERHUB_USER` antes de correr Docker Compose:
   ```bash
   # Linux/Mac
   export DOCKERHUB_USER=miusuario

   # Windows (PowerShell)
   $env:DOCKERHUB_USER="miusuario"
   ```

### Pasos de Despliegue

1. Clonar el repositorio:
```bash
git clone <url-del-repositorio>
cd <nombre-del-repositorio>
```

2. Compilar el código C++:
```bash
# Compilar el servidor
g++ -o server server.cpp -std=c++11

# Compilar el cliente
g++ -o client client.cpp -std=c++11
```

3. Construir las imágenes Docker (opcional si vas a usar las publicadas en DockerHub):
```bash
docker-compose build
```

4. Descargar y ejecutar las imágenes publicadas (por defecto se descargan desde `sebas22x`):
```bash
docker-compose pull
docker-compose up
```

## Uso

### Ejecutar con Docker Compose

1. **Descargar y ejecutar las imágenes publicadas**:
```bash
docker-compose pull
docker-compose up
```

2. **(Opcional) Reconstruir imágenes locales y ejecutar**:
```bash
docker-compose up --build
```

3. **Ejecutar solo el servidor** (en una terminal):
```bash
docker-compose up server
```

4. **Ejecutar el cliente manualmente** (en otra terminal):
```bash
docker-compose run --rm client ./client 50000 "Mi transferencia"
```

### Ejecutar localmente (sin Docker)

1. **Compilar**:
```bash
# En Windows
build.bat

# En Linux/Mac
chmod +x build.sh
./build.sh
```

2. **Ejecutar servidor**:
```bash
# Windows
server\server.exe

# Linux/Mac
./server/server
```

3. **Ejecutar cliente** (en otra terminal):
```bash
# Windows
client\client.exe 50000 "Transferencia de prueba"

# Linux/Mac
./client/client 50000 "Transferencia de prueba"
```

### Parámetros del Cliente

El cliente acepta parámetros opcionales:
```bash
./client [monto] [descripción]
```

- **monto**: Monto de la transacción (default: 100000.0)
- **descripción**: Descripción de la transacción (default: "Transferencia entre contenedores")

## Estructura del Proyecto

```
.
├── server/          # Código del servidor
├── client/          # Código del cliente
├── docker/          # Archivos Docker
├── docs/            # Documentación
└── README.md        # Este archivo
```

## Sistema de Seguridad

El sistema implementa un mecanismo de **clave dinámica mediante tokens**, similar al sistema de Bancolombia:

- Cada transacción tiene un **token único** generado usando: `Hash(ID + Timestamp + SecretKey)`
- El servidor valida el token regenerándolo con los mismos parámetros
- Las transacciones incluyen: ID único, timestamp, monto, descripción y token

Ver documentación completa en [docs/SEGURIDAD.md](docs/SEGURIDAD.md)

## Imágenes en DockerHub

Las imágenes públicas oficiales para este proyecto están en la cuenta `sebas22x`:

- Servidor: `sebas22x/entregafinal-server:latest`
- Cliente: `sebas22x/entregafinal-client:latest`

Para usar las imágenes sin compilar:
```bash
docker pull sebas22x/entregafinal-server:latest
docker pull sebas22x/entregafinal-client:latest
```

Si necesitas subir tus propias imágenes con otro usuario, define la variable `DOCKERHUB_USER` antes de ejecutar `docker-compose`. El archivo `docker-compose.yml` usa el valor de esa variable y, si no está presente, recurre automáticamente a `sebas22x`.

## Autores

[Agregar nombres de los integrantes]

## Licencia

[Especificar licencia si aplica]

