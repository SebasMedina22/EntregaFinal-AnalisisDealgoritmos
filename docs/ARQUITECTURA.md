# Arquitectura General

La solución se compone de **dos contenedores Docker** que se comunican por sockets TCP dentro de una misma red bridge creada automáticamente por `docker-compose`.

```
                Docker Bridge Network: entregafinal-analisisdealgoritmos_transaction_network
┌─────────────────────┐                                   ┌──────────────────────┐
│  Contenedor Cliente │ -- (socket TCP puerto 8080) -->   │ Contenedor Servidor  │
│  Imagen: sebas22x/  │                                   │ Imagen: sebas22x/    │
│  entregafinal-client│                                   │ entregafinal-server  │
└─────────────────────┘                                   └──────────────────────┘
```

## Componentes

1. **Cliente (`client/client.cpp`)**
   - Genera transacciones con ID único, timestamp, monto y descripción.
   - Calcula el token dinámico y lo envía al servidor.

2. **Servidor (`server/server.cpp`)**
   - Escucha conexiones en el puerto 8080.
   - Valida el token y responde si la transacción es válida.

3. **Docker Compose (`docker-compose.yml`)**
   - Define los servicios `client` y `server`.
   - Crea la red `transaction_network`.
   - Expone el puerto 8080 del servidor al host para fines de depuración.

## Flujo

1. `docker-compose pull` descarga las imágenes publicadas (`sebas22x/...`).
2. `docker-compose up` crea la red bridge y levanta ambos contenedores.
3. El cliente resuelve el hostname `server`, abre un socket TCP y envía la transacción.
4. El servidor procesa, valida el token y envía la respuesta.
5. Al terminar, `docker-compose down` detiene los contenedores y elimina la red.

## Puertos y redes

- **Puerto 8080**: expuesto por el servidor hacia el host (`8080:8080`) para facilitar pruebas.
- **Red interna**: Docker bridge que permite que `client` y `server` se comuniquen usando los hostnames declarados en Compose (no es necesario conocer IPs).

## Imágenes Docker

Cada servicio usa una imagen basada en Ubuntu 22.04:
- `sebas22x/entregafinal-server:latest`
- `sebas22x/entregafinal-client:latest`

Las imágenes incluyen el sistema operativo base, dependencias mínimas (g++), el código C++ y los ejecutables precompilados.