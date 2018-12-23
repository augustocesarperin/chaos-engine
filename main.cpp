/*
 * Particle Simulator
 * 
 * Developed by: Augusto César Perin
 * 
 * A 2D particle simulator with basic physics interactions
 */

#include <SFML/Graphics.hpp>
#include "ParticleSystem.h"
#include <iostream>
#include <random>
#include <string>
#include <algorithm>

int main()
{
    const int WIDTH = 800;
    const int HEIGHT = 600;
    float desiredGravitationalAcceleration = 150.0f;
    const float REPULSION = 5.0f;
    const int NUM_PARTICLES = 50;

    // Variaveis do modo com o mouse afetando as particulas
    bool mouseForceEnabled = false;
    bool mouseForceAttractMode = true; 
    float mouseForceStrength = 75000.0f;
    const float minMouseForceStrength = 5000.0f;
    const float maxMouseForceStrength = 500000.0f;
    const float mouseForceStrengthStep = 10000.0f;
    sf::Vector2f mousePositionWindow;
    
    
    std::string windowTitle = "Simulador de Partículas 2D - por Augusto César Perin";
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), sf::String::fromUtf8(windowTitle.begin(), windowTitle.end()));
    window.setFramerateLimit(60); 
    
    sf::Font font;
    if (!font.loadFromFile("C:\\Windows\\Fonts\\arial.ttf")) {
        std::cerr << "Erro ao carregar fonte!" << std::endl;
    }
    
    sf::Text instructions;
    instructions.setFont(font);
    instructions.setCharacterSize(12); 
    instructions.setFillColor(sf::Color::White);
    instructions.setPosition(10.f, 10.f);
    
    
    sf::Text signature;
    signature.setFont(font);
    signature.setCharacterSize(12); 
    signature.setFillColor(sf::Color(180, 180, 180, 200)); 
    signature.setStyle(sf::Text::Bold | sf::Text::Italic); 
    std::string devText = "Augusto César Perin";
    signature.setString(sf::String::fromUtf8(devText.begin(), devText.end()));
    
  
    sf::FloatRect signatureBounds = signature.getLocalBounds();
    signature.setPosition(WIDTH - signatureBounds.width - 15.f, HEIGHT - 25.f); 
    
    ParticleSystem particleSystem(WIDTH, HEIGHT);
    particleSystem.generateRandomParticles(NUM_PARTICLES, 1.0f, 10.0f);
    
    bool gravityEnabled = true;
    bool repulsionEnabled = false;
    
    sf::Clock clock;
    
    while (window.isOpen())
    {
        float deltaTime = clock.restart().asSeconds();
        if (deltaTime > 0.1f) deltaTime = 0.1f;

        
        mousePositionWindow = static_cast<sf::Vector2f>(sf::Mouse::getPosition(window));
        
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            
            if (event.type == sf::Event::MouseButtonPressed)
            {
                
                sf::Vector2f position = mousePositionWindow;
                
                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_real_distribution<float> velDist(-50.0f, 50.0f);
                // Sistema de cores aprimorado
                std::uniform_int_distribution<int> colorMode(0, 10); // Diferentes modos de cor
                
                sf::Vector2f velocity(velDist(gen), velDist(gen));
                sf::Color color;
                
                // Escolhe entre cores predefinidas vibrantes ou cores aleatórias
                int mode = colorMode(gen);
                if (mode <= 2) {
                    // Cores predefinidas vibrantes
                    const sf::Color vibrantColors[] = {
                        sf::Color(255, 0, 0),     // Vermelho
                        sf::Color(0, 255, 0),     // Verde
                        sf::Color(0, 0, 255),     // Azul
                        sf::Color(255, 255, 0),   // Amarelo
                        sf::Color(255, 0, 255),   // Magenta
                        sf::Color(0, 255, 255),   // Ciano
                        sf::Color(255, 128, 0),   // Laranja
                        sf::Color(128, 0, 255)    // Roxo
                    };
                    std::uniform_int_distribution<int> colorIndex(0, 7);
                    color = vibrantColors[colorIndex(gen)];
                } else if (mode <= 5) {
                    // Cores pastel (mais suaves)
                    std::uniform_int_distribution<int> pastelDist(180, 255);
                    color = sf::Color(pastelDist(gen), pastelDist(gen), pastelDist(gen));
                } else {
                    // Cores totalmente aleatórias (incluindo cores mais escuras)
                    std::uniform_int_distribution<int> fullColorDist(20, 255);
                    color = sf::Color(fullColorDist(gen), fullColorDist(gen), fullColorDist(gen));
                }
                
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

                    
                    case sf::Keyboard::M:
                        mouseForceEnabled = !mouseForceEnabled;
                        break;
                    
                    case sf::Keyboard::N:
                        if (mouseForceEnabled) { 
                            mouseForceAttractMode = !mouseForceAttractMode;
                        }
                        break;

                    case sf::Keyboard::Add: 
                    case sf::Keyboard::Equal: 
                        if (mouseForceEnabled) {
                            mouseForceStrength = std::min(maxMouseForceStrength, mouseForceStrength + mouseForceStrengthStep);
                        }
                        break;

                    case sf::Keyboard::Subtract: 
                    case sf::Keyboard::Hyphen:   
                         if (mouseForceEnabled) {
                            mouseForceStrength = std::max(minMouseForceStrength, mouseForceStrength - mouseForceStrengthStep);
                        }
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
        
       
        if (mouseForceEnabled) {
            particleSystem.applyMouseForce(mousePositionWindow, mouseForceStrength, mouseForceAttractMode);
        }
        
        
        particleSystem.update(deltaTime);
        
        // Update UI 
        std::string mouseForceStatus = mouseForceEnabled ? "ON" : "OFF";
        std::string mouseForceModeStatus = mouseForceAttractMode ? "Attract" : "Repel";

        std::string statusText = 
            "Controles:\n"
            "Clique esquerdo: Adicionar partícula\n"
            "Clique direito: Adicionar partícula grande\n"
            "G: Ativar/desativar gravidade (" + std::string(gravityEnabled ? "ON" : "OFF") + ")\n"
            "R: Repulsao entre particulas (" + std::string(repulsionEnabled ? "ON" : "OFF") + ")\n"
            "M: Forca do Mouse (" + mouseForceStatus + ")\n"
            " N: Modo Forca Mouse (" + mouseForceModeStatus + ")\n"
            " +/-: Intensidade Forca Mouse (" + std::to_string(static_cast<int>(mouseForceStrength)) + ")\n"
            "C: Limpar todas as particulas\n"
            "Espaco: Gerar particulas aleatorias\n\n"
            "Partículas: " + std::to_string(particleSystem.getParticleCount()) +
            "\nFPS: " + std::to_string(static_cast<int>(1.0f / (deltaTime > 0.0001f ? deltaTime : 0.0001f)));
        instructions.setString(sf::String::fromUtf8(statusText.begin(), statusText.end()));
        
        window.clear(sf::Color(20, 20, 30)); 
        
        particleSystem.draw(window);
        
        // Desenhar interface
        window.draw(instructions);
        
        window.display();
    }

    return 0;
}