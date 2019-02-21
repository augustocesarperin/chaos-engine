@echo off
echo - Iniciando processo de compilacao com CMake -

REM
if not exist "build" (
    echo Criando diretorio de build...
    mkdir build
)

REM 
cd build

REM 
echo Configurando com CMake...
cmake .. -G "MinGW Makefiles"

REM 
if %ERRORLEVEL% neq 0 (
    echo.
    echo ERRO: Falha ao configurar com CMake.
    echo Verifique se o CMake e o compilador MinGW estao no PATH do sistema.
    cd ..
    pause
    exit /b
)

REM 
echo Compilando o projeto com mingw32-make...
mingw32-make

REM 
if %ERRORLEVEL% equ 0 (
    echo.
    echo --- Compilacao concluida com sucesso! ---
    echo O executavel 'Chaos.exe' esta na pasta 'build'.
) else (
    echo.
    echo ERRO: Falha na compilacao. Verifique a saida do compilador acima.
)

REM 
cd ..

echo.
pause
