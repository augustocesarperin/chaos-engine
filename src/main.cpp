/*
 * Particle Simulator
 * 
 * Developed by: Augusto César Perin
 *  2018~2019
 */
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "ParticleSystem.h"
#include "Mousart.h"
#include <iostream>
#include <random>
#include <string>
#include <algorithm>

int main()
{
    const int WIDTH = 800;
    const int HEIGHT = 600;
    float desiredGravitationalAcceleration = 250.0f; // Valor equilibrado para simular gravidade
    const float REPULSION = 5.0f;
    const int NUM_PARTICLES = 50;

    ParticleType currentParticleType = ParticleType::Original;
    std::string particleTypeName = "Original";

 // variáveis da força que o mouse aplica às partículas
    bool mouseForceEnabled = false;
    bool mouseForceAttractMode = true; 
    float mouseForceStrength = 75000.0f;
    const float minMouseForceStrength = 5000.0f;
    const float maxMouseForceStrength = 500000.0f;
    const float mouseForceStrengthStep = 10000.0f;
    sf::Vector2f mousePositionWindow;
    
    
    std::string windowTitle = "Simulador de Partículas - por Augusto César Perin";
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), sf::String::fromUtf8(windowTitle.begin(), windowTitle.end()));

    // Carregar e definir o ícone da janela
    sf::Image windowIcon;
    if (windowIcon.loadFromFile("assets/icon.png")) {
        window.setIcon(windowIcon.getSize().x, windowIcon.getSize().y, windowIcon.getPixelsPtr());
    } else {
        std::cerr << "Erro ao carregar ícone da janela!" << std::endl;
    }

    window.setFramerateLimit(60);

    
    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("assets/background.png")) {
        std::cerr << "Erro ao carregar assets/background.png!" << std::endl;
    }
    
    sf::Sprite backgroundSprite;
    backgroundSprite.setTexture(backgroundTexture);
    
    // ajuste pra deixar a tela preenchida
    sf::Vector2u textureSize = backgroundTexture.getSize();
    float scaleX = static_cast<float>(WIDTH) / textureSize.x;
    float scaleY = static_cast<float>(HEIGHT) / textureSize.y;
    backgroundSprite.setScale(scaleX, scaleY);
    
    
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
    
    // mouse personalizado
    Mousart mousart;
    if (!mousart.initialize()) {
        std::cerr << "Erro ao inicializar Mousart! Verifique se mouse1.png e mouse2.png existem na pasta do executável." << std::endl;
        return -1;
    }
    
    window.setMouseCursorVisible(false);
    
    bool gravityEnabled = true;
    bool repulsionEnabled = false;
    
    sf::Clock clock;
    
    while (window.isOpen())
    {
        float deltaTime = clock.restart().asSeconds();
        if (deltaTime > 0.1f) deltaTime = 0.1f;

        
        mousePositionWindow = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            
            if (event.type == sf::Event::Resized)
            {
                // Criar uma nova view para o novo tamanho da janela
                sf::View view;
                view.reset(sf::FloatRect(0, 0, event.size.width, event.size.height));
                view.setViewport(sf::FloatRect(0, 0, 1, 1));
                window.setView(view);
                instructions.setPosition(10.f, 10.f);
                
                sf::Vector2u textureSize = backgroundTexture.getSize();
                float newScaleX = static_cast<float>(event.size.width) / textureSize.x;
                float newScaleY = static_cast<float>(event.size.height) / textureSize.y;
                backgroundSprite.setScale(newScaleX, newScaleY);
        
                particleSystem.setWindowSize(static_cast<float>(event.size.width), static_cast<float>(event.size.height));            
                // Força a atualização da posição do cursor pós redimensionamento
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                mousart.update(mousePos, window);
            }
            
            if (event.type == sf::Event::MouseButtonPressed)
            {
                // garante que a partícula apareça exatamente na ponta do cursor
                sf::Vector2i mouseClickPos(event.mouseButton.x, event.mouseButton.y);
                sf::Vector2f basePosition = window.mapPixelToCoords(mouseClickPos);
                
                sf::Vector2f tipOffset = mousart.getCursorTipOffset(window);
                sf::Vector2f position = basePosition + tipOffset;
                
                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_real_distribution<float> velDist(-50.0f, 50.0f);
                std::uniform_int_distribution<int> colorMode(0, 10);
                
                sf::Vector2f velocity(velDist(gen), velDist(gen));
                sf::Color color;
                
                int mode = colorMode(gen);
                if (mode <= 10) {
                    const sf::Color harmoniousPalette[] = {
                        sf::Color(3, 169, 244),
                        sf::Color(156, 39, 176),
                        sf::Color(255, 87, 34),
                        sf::Color(76, 175, 80),
                        sf::Color(255, 193, 7),
                        sf::Color(233, 30, 99),
                        sf::Color(33, 150, 243),
                        sf::Color(0, 188, 212),
                        sf::Color(139, 195, 74),
                        sf::Color(103, 58, 183),
                        sf::Color(255, 152, 0),
                        sf::Color(96, 125, 139)
                    };
                    std::uniform_int_distribution<int> colorIndex(0, 11);
                    color = harmoniousPalette[colorIndex(gen)];
                }
                
                if (event.mouseButton.button == sf::Mouse::Left) {
                    Particle& p = particleSystem.addParticle(2.0f, position, velocity, color);
                    p.setParticleType(currentParticleType);
                } else if (event.mouseButton.button == sf::Mouse::Right) {
                    Particle& p = particleSystem.addParticle(15.0f, position, velocity, color);
                    p.setParticleType(currentParticleType);
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
                        for (int i = 0; i < 20; ++i) {
                            Particle& p = particleSystem.generateRandomParticle(1.0f, 8.0f);
                            p.setParticleType(currentParticleType);
                        }
                        break;

                    
                    case sf::Keyboard::M:
                        mouseForceEnabled = !mouseForceEnabled;
                        mousart.setForceMode(mouseForceEnabled);
                        break;
                    
                    case sf::Keyboard::N:
                        if (mouseForceEnabled) { 
                            mouseForceAttractMode = !mouseForceAttractMode;
                        }
                        break;
                    
                    case sf::Keyboard::T:
                        if (currentParticleType == ParticleType::Original) {
                            currentParticleType = ParticleType::Crystal;
                            particleTypeName = "Crystal";
                        } else {
                            currentParticleType = ParticleType::Original;
                            particleTypeName = "Original";
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
                        
                    case sf::Keyboard::K:
                        // Alterna entre os tipos de cursor disponíveis
                        mousart.cycleCursorType();
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
        
        std::string mouseForceStatus = mouseForceEnabled ? "ON" : "OFF";
        std::string mouseForceModeStatus = mouseForceAttractMode ? "Attract" : "Repel";

        std::string statusText = 
            "Controles:\n"
            "Botão esquerdo: Adicionar partícula\n"
            "Botão direito: Adicionar partícula grande\n"
            "G: Ativar/desativar gravidade (" + std::string(gravityEnabled ? "ON" : "OFF") + ")\n"
            "R: Repulsao entre particulas (" + std::string(repulsionEnabled ? "ON" : "OFF") + ")\n"
            "T: Alternar tipo de partícula (" + particleTypeName + ")\n"
            "M: Forca do Mouse (" + mouseForceStatus + ")\n"
            " N: Modo Forca Mouse (" + mouseForceModeStatus + ")\n"
            " +/-: Intensidade Forca Mouse (" + std::to_string(static_cast<int>(mouseForceStrength)) + ")\n"
            "C: Limpar todas as particulas\n"
            "Espaco: Gerar particulas aleatorias\n\n"
            "Partículas: " + std::to_string(particleSystem.getParticleCount()) +
            "\nFPS: " + std::to_string(static_cast<int>(1.0f / (deltaTime > 0.0001f ? deltaTime : 0.0001f)));
        instructions.setString(sf::String::fromUtf8(statusText.begin(), statusText.end()));
        
        window.clear(sf::Color(10, 5, 15)); 
        
        window.draw(backgroundSprite);
        
        particleSystem.draw(window);
        window.draw(instructions);
        sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
        mousart.update(mousePosition, window);
        mousart.draw(window);
        
        window.display();
    }

    return 0;
}