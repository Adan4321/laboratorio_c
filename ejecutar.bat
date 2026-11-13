@echo off
setlocal

if not exist simulador_artemis.exe (
    echo No existe simulador_artemis.exe.
    echo Ejecute compilar.bat primero.
    endlocal
    exit /b 1
)

simulador_artemis.exe
endlocal