@echo off
echo Compilando Simulador de Particulas...
g++ src/main.cpp src/ParticleSystem.cpp src/Particle.cpp src/Mousart.cpp src/TextureManager.cpp src/ParticlePool.cpp src/physics_c.cpp src/SpatialGrid.cpp -o Chaos -lsfml-graphics -lsfml-window -lsfml-system
if %ERRORLEVEL% == 0 (
    echo Compilacao concluida com sucesso!
    echo Execute o programa com: Chaos.exe
) else (
    echo Erro na compilacao! Verifique se o SFML esta instalado corretamente.
)
pause
