@echo off
REM Script para compilar los programas C++ localmente en Windows

echo Compilando servidor...
g++ -o server\server.exe server\server.cpp -std=c++11
if %errorlevel% equ 0 (
    echo ✓ Servidor compilado exitosamente
) else (
    echo ✗ Error al compilar el servidor
    exit /b 1
)

echo Compilando cliente...
g++ -o client\client.exe client\client.cpp -std=c++11
if %errorlevel% equ 0 (
    echo ✓ Cliente compilado exitosamente
) else (
    echo ✗ Error al compilar el cliente
    exit /b 1
)

echo Compilacion completada

