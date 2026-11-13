@echo off
setlocal

call compilar.bat
if errorlevel 1 (
    endlocal
    exit /b 1
)

simulador_artemis.exe
endlocal