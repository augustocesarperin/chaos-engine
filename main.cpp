#include <SFML/Graphics.hpp>
#include "ParticleSystem.h"
#include <iostream>
#include <random>
#include <string>

int main()
{
    const int WIDTH = 800;
    const int HEIGHT = 600;
    float desiredGravitationalAcceleration = 150.0f; // Aceleração gravitacional desejada (ajuste conforme necessário)
    const float REPULSION = 5.0f;
    const int NUM_PARTICLES = 50;
    
    std::string windowTitle = "Simulador de Partículas 2D";
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), sf::String::fromUtf8(windowTitle.begin(), windowTitle.end()));
    window.setFramerateLimit(60); // evita CPU 100%
    
    sf::Font font;
    if (!font.loadFromFile("C:\\Windows\\Fonts\\arial.ttf")) {
        std::cerr << "Erro ao carregar fonte!" << std::endl;
    }
    
    sf::Text instructions;
    instructions.setFont(font);
    instructions.setCharacterSize(12);
    instructions.setFillColor(sf::Color::White);
    instructions.setPosition(10.f, 10.f);
    
    ParticleSystem particleSystem(WIDTH, HEIGHT);
    particleSystem.generateRandomParticles(NUM_PARTICLES, 1.0f, 10.0f);
    
    bool gravityEnabled = true;
    bool repulsionEnabled = false;
    
    sf::Clock clock;
    
    while (window.isOpen())
    {
        float deltaTime = clock.restart().asSeconds();
        
        if (deltaTime > 0.1f) deltaTime = 0.1f;
        
        // Processa eventos
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            
            if (event.type == sf::Event::MouseButtonPressed)
            {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                sf::Vector2f position(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
                
                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_real_distribution<float> velDist(-50.0f, 50.0f);
                std::uniform_int_distribution<int> colorDist(100, 255);
                
                sf::Vector2f velocity(velDist(gen), velDist(gen));
                sf::Color color(colorDist(gen), colorDist(gen), colorDist(gen));
                
                if (event.mouseButton.button == sf::Mouse::Left) {
                    particleSystem.addParticle(2.0f, position, velocity, color);
                } else if (event.mouseButton.button == sf::Mouse::Right) {
                    particleSystem.addParticle(15.0f, position, velocity, color);
                }
            }
            
            if (event.type == sf::Event::KeyPressed)
            {
                switch (event.key.code)
                {
                    case sf::Keyboard::G:
                        gravityEnabled = !gravityEnabled;
                        break;
                    
                    case sf::Keyboard::R:
                        repulsionEnabled = !repulsionEnabled;
                        break;
                    
                    case sf::Keyboard::C:
                        
                        while (particleSystem.getParticleCount() > 0) {
                            particleSystem.removeParticle(0);
                        }
                        break;
                    
                    case sf::Keyboard::Space:
                        // Gerar mais partículas aleatórias
                        particleSystem.generateRandomParticles(20, 1.0f, 8.0f);
                        break;
                    
                    default:
                        break;
                }
            }
        }
        
        if (gravityEnabled) {
            particleSystem.applyGravityEffect(desiredGravitationalAcceleration);
        }
        
        if (repulsionEnabled) {
            particleSystem.applyInteractiveForces(REPULSION);
        }
        
        particleSystem.update(deltaTime);
        
        std::string statusText = 
            "Controles:\n"
            "Clique esquerdo: Adicionar partícula\n"
            "Clique direito: Adicionar partícula grande\n"
            "G: Ativar/desativar gravidade (" + std::string(gravityEnabled ? "ON" : "OFF") + ")\n"
            "R: Ativar/desativar repulsão (" + std::string(repulsionEnabled ? "ON" : "OFF") + ")\n"
            "C: Limpar todas as partículas\n"
            "Espaço: Gerar partículas aleatórias\n\n"
            "Partículas: " + std::to_string(particleSystem.getParticleCount()) +
            "\nFPS: " + std::to_string(1.0f / (deltaTime > 0.0001f ? deltaTime : 0.0001f));
        instructions.setString(sf::String::fromUtf8(statusText.begin(), statusText.end()));
        
        window.clear(sf::Color(20, 20, 30)); // Azul escuro
        
        particleSystem.draw(window);
        
        // Desenhar interface
        window.draw(instructions);
        
        window.display();
    }

    return 0;
}