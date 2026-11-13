@echo off
setlocal

gcc -std=c11 -Wall -Wextra -Wpedantic -O2 main.c artemis.c validaciones.c -o simulador_artemis.exe -lm

if errorlevel 1 (
    echo.
    echo Error de compilacion. Verifique que gcc este instalado y agregado al PATH.
    endlocal
    exit /b 1
)

echo.
echo Compilacion exitosa: simulador_artemis.exe
endlocal