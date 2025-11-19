# Diagramas y Flujos

## 1. Flujo de Transacción

```
┌───────────────┐
│   Cliente     │
│ (contenedor)  │
└──────┬────────┘
       │ 1. Genera TXN (ID, timestamp, monto, descripción)
       │ 2. Calcula token dinámico
       │ 3. Serializa datos en formato "ID|TS|MONTO|DESC|TOKEN"
       ▼
┌──────────────────────────────┐
│ Socket TCP dentro de la red  │
│ bridge (puerto 8080)         │
└──────────────────────────────┘
       │ 4. Envía paquete
       ▼
┌───────────────┐
│   Servidor    │
│ (contenedor)  │
└──────┬────────┘
       │ 5. Deserializa la TXN
       │ 6. Regenera token esperado
       │ 7. Compara token recibido vs. esperado
       │ 8. Responde "TRANSACCION_VALIDA/INVALIDA"
       ▼
┌───────────────┐
│   Cliente     │
└───────────────┘ 9. Muestra resultado
```

## 2. Diagrama de Seguridad (clave dinámica)

```
             +----------------------+
             |  Clave secreta       |
             |  BANCOLOMBIA_SECRET  |
             +----------+-----------+
                        |
              +---------v---------+
              |  Datos TXN        |
              |  (ID + Timestamp) |
              +---------+---------+
                        |
                        v
            +-----------------------+
            | Hash simple (x31 + c) |
            +-----------------------+
                        |
                        v
                  Token Hexadecimal
```

- **Cliente**: genera token y lo adjunta a la transacción.
- **Servidor**: repite el cálculo con los mismos datos; si el token coincide, la transacción es auténtica.

## 3. Evidencia visual recomendada

Para la presentación/documentación se sugiere adjuntar:
- Captura de `docker-compose up` mostrando cliente y servidor.
- Capturas de Docker Desktop con los contenedores `transaction_server` y `transaction_client`.
- Captura del mensaje “TRANSACCION_VALIDA” en la terminal del cliente.