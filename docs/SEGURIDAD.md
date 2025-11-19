# Sistema de Seguridad

El sistema implementa una **clave dinámica tipo Bancolombia** basada en tokens generados para cada transacción. El objetivo es garantizar autenticidad e integridad sin depender de frameworks externos.

## Datos que viajan en cada transacción

```
ID único        → Ej: TXN-20251119-000123
Timestamp       → Ej: 2025-11-19 15:30:05
Monto           → Ej: 50000.0
Descripción     → Ej: "Transferencia de prueba"
Token           → Calculado dinámicamente
```

La transacción serializada se envía como `ID|TIMESTAMP|MONTO|DESCRIPCION|TOKEN`.

## Algoritmo del token

1. Se concatena `transactionId + timestamp + SECRET_KEY`.
2. Se recorre la cadena con una operación hash simple:
   ```cpp
   unsigned int hash = 0;
   for (char c : data) {
       hash = hash * 31 + c;
   }
   ```
3. El resultado se convierte a hexadecimal (`std::hex`) para obtener el token.

La clave secreta compartida es `BANCOLOMBIA_SECRET_2025`. El token cambia automáticamente si cambia cualquiera de los insumos (ID, fecha, hora, descripción, etc.).

## Validación en el servidor

1. El servidor vuelve a generar el token esperado usando el mismo algoritmo y la misma clave secreta.
2. Compara el token recibido con el token calculado.
3. Si coinciden, responde `TRANSACCION_VALIDA`; de lo contrario, `TRANSACCION_INVALIDA`.

## Ejemplo

- **Transacción**:  
  `ID = TXN-20251119-000001`  
  `Timestamp = 2025-11-19 15:40:20`  
  Token generado (hex): `5e504364`

- **Servidor** vuelve a calcular y obtiene `5e504364` → transacción aceptada.
- Si un atacante altera el monto o la hora, el token se recalcula distinto y el servidor la rechaza.

## Propiedades

- **Unicidad**: cada transacción tiene ID y timestamp únicos, lo que hace que el token sea irrepetible.
- **Integridad**: cualquier cambio en los datos invalida el token.
- **Sin dependencias externas**: el hash es propio; se ejecuta igual dentro y fuera de los contenedores.
- **Pruebas documentadas**: las capturas de `docker-compose up` muestran mensajes de validación para evidenciar que el control funciona.