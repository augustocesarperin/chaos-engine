@echo off
echo Executando Simulador de Particulas...
ParticleSimulator.exe
if %ERRORLEVEL% neq 0 (
    echo Erro ao executar! Verifique se o arquivo foi compilado e se as DLLs do SFML estao disponiveis.
    echo Se o programa nao foi compilado, execute compile.bat primeiro.
)
pause
