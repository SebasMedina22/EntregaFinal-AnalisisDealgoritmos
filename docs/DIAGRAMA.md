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

## 3. Evidencia visual

1. docker-compose up mostrando cliente/servidor:
   <img width="1200" height="531" alt="image" src="https://github.com/user-attachments/assets/772f6399-4e3f-4584-a4f6-6c6f24e45cd1" />

2. Contenedores en Docker Desktop:
   <img width="1619" height="377" alt="image" src="https://github.com/user-attachments/assets/09192581-378e-4aef-b6d2-e42e694a752e" />
