@echo off
echo Compilando Simulador de Particulas...
g++ main.cpp ParticleSystem.cpp Particle.cpp -o ParticleSimulator -lsfml-graphics -lsfml-window -lsfml-system
if %ERRORLEVEL% == 0 (
    echo Compilacao concluida com sucesso!
    echo Execute o programa com: ParticleSimulator.exe
) else (
    echo Erro na compilacao! Verifique se o SFML esta instalado corretamente.
)
pause
