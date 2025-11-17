#!/bin/bash

# Script para compilar los programas C++ localmente

echo "Compilando servidor..."
g++ -o server/server server/server.cpp -std=c++11
if [ $? -eq 0 ]; then
    echo "✓ Servidor compilado exitosamente"
else
    echo "✗ Error al compilar el servidor"
    exit 1
fi

echo "Compilando cliente..."
g++ -o client/client client/client.cpp -std=c++11
if [ $? -eq 0 ]; then
    echo "✓ Cliente compilado exitosamente"
else
    echo "✗ Error al compilar el cliente"
    exit 1
fi

echo "Compilación completada"

