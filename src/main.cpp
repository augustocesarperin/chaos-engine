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
#include <exception>
#include <random>
#include <string>
#include <algorithm>
#include <vector>

struct AppState {
    static constexpr int NUM_PARTICLES_INICIAL = 0;
    static constexpr float GRAVIDADE_PADRAO = 250.0f;
    static constexpr float REPULSAO_PADRAO = 5.0f;
    static constexpr float RESTITUICAO_PADRAO = 0.8f;
    static constexpr float DEFAULT_MOUSE_FORCE = 75000.0f;
    static constexpr float MIN_MOUSE_FORCE = 5000.0f;
    static constexpr float MAX_MOUSE_FORCE = 500000.0f;
    static constexpr float MOUSE_FORCE_STEP = 10000.0f;
    
    float desiredGravitationalAcceleration = GRAVIDADE_PADRAO;
    bool gravityEnabled = true;
    bool repulsionEnabled = false;
    bool collisionsEnabled = true;
    float collisionRestitution = RESTITUICAO_PADRAO;

    ParticleType currentParticleType = ParticleType::Original;
    std::string particleTypeName = "Original";
    
    bool mouseForceEnabled = false;
    bool mouseForceAttractMode = true; 
    float mouseForceStrength = DEFAULT_MOUSE_FORCE;
    sf::Vector2f mousePositionWindow;

    ParticleSystem particleSystem;
    Mousart mousart;

    sf::Font font;
    sf::Text instructions;
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;

    AppState(float width, float height) : particleSystem(width, height) {}
};

void setup(sf::RenderWindow& window, AppState& state);
void processInput(sf::RenderWindow& window, AppState& state);
void update(sf::Clock& clock, sf::RenderWindow& window, AppState& state);
void render(sf::RenderWindow& window, AppState& state);

int main()
{
    try {
        const int WIDTH = 800;
        const int HEIGHT = 600;
        
        std::string windowTitle = "Simulador de Partículas - por Augusto César Perin";
        sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), sf::String::fromUtf8(windowTitle.begin(), windowTitle.end()));
        window.setFramerateLimit(60);

        AppState state(WIDTH, HEIGHT);
        setup(window, state);
        
        sf::Clock clock;
        
        while (window.isOpen()) {
            try {
                processInput(window, state);
                update(clock, window, state);
                render(window, state);
            } catch (const std::exception& e) {
                std::cerr << "ERRO NO LOOP PRINCIPAL: " << e.what() << std::endl;
                system("pause");
                break;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "ERRO FATAL: " << e.what() << std::endl;
        system("pause");
        return -1;
    }
    return 0;
}

void setup(sf::RenderWindow& window, AppState& state) {
    sf::Image windowIcon;
    if (windowIcon.loadFromFile("assets/icon.png")) {
        window.setIcon(windowIcon.getSize().x, windowIcon.getSize().y, windowIcon.getPixelsPtr());
    } else {
        std::cerr << "[AVISO] Não foi possível carregar 'assets/icon.png'" << std::endl;
    }

    if (!state.backgroundTexture.loadFromFile("assets/background.png")) {
        std::cerr << "[AVISO] Não foi possível carregar 'assets/background.png', usando cor de fallback." << std::endl;
        state.backgroundTexture.create(1, 1);
        sf::Image fallbackImage;
        fallbackImage.create(1, 1, sf::Color(20, 20, 50));
        state.backgroundTexture.update(fallbackImage);
    }
    state.backgroundSprite.setTexture(state.backgroundTexture);
    
    sf::Vector2u windowSize = window.getSize();
    sf::Vector2u textureSize = state.backgroundTexture.getSize();
    float scaleX = static_cast<float>(windowSize.x) / textureSize.x;
    float scaleY = static_cast<float>(windowSize.y) / textureSize.y;
    state.backgroundSprite.setScale(scaleX, scaleY);

    std::vector<std::string> fontPaths = {"C:\\Windows\\Fonts\\arial.ttf", "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", "/Library/Fonts/Arial.ttf", "assets/fonts/arial.ttf", "arial.ttf"};
    bool fontLoaded = false;
    for (const auto& path : fontPaths) {
        if (state.font.loadFromFile(path)) {
            fontLoaded = true;
            break;
        }
    }
    if (!fontLoaded) {
        std::cerr << "Aviso: Nenhuma fonte encontrada. O texto não será exibido." << std::endl;
    }
    
    state.instructions.setFont(state.font);
    state.instructions.setCharacterSize(12); 
    state.instructions.setFillColor(sf::Color::White);
    state.instructions.setPosition(10.f, 10.f); 
    
    state.particleSystem.generateRandomParticles(AppState::NUM_PARTICLES_INICIAL, 1.0f, 10.0f);
    
    if (!state.mousart.initialize()) {
        throw std::runtime_error("Falha ao inicializar o Mousart. Verifique os assets do cursor.");
    }
    window.setMouseCursorVisible(false);
}

void processInput(sf::RenderWindow& window, AppState& state) {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        
        if (event.type == sf::Event::Resized) {
            sf::View view;
            view.reset(sf::FloatRect(0, 0, event.size.width, event.size.height));
            window.setView(view);
            state.particleSystem.setWindowSize(static_cast<float>(event.size.width), static_cast<float>(event.size.height));
        }
        
        if (event.type == sf::Event::MouseButtonPressed) {
            sf::Vector2f position = window.mapPixelToCoords({event.mouseButton.x, event.mouseButton.y});
            position += state.mousart.getCursorTipOffset(window);

            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_real_distribution<float> velDist(-50.0f, 50.0f);
            
            const sf::Color harmoniousPalette[] = { sf::Color(3, 169, 244), sf::Color(156, 39, 176), sf::Color(255, 87, 34), sf::Color(76, 175, 80), sf::Color(255, 193, 7) };
            std::uniform_int_distribution<int> colorIndex(0, std::size(harmoniousPalette) - 1);
            
            Particle* p = nullptr;
            if (event.mouseButton.button == sf::Mouse::Left) {
                p = state.particleSystem.addParticle(2.0f, position, {velDist(gen), velDist(gen)}, harmoniousPalette[colorIndex(gen)]);
            } else if (event.mouseButton.button == sf::Mouse::Right) {
                p = state.particleSystem.addParticle(15.0f, position, {velDist(gen), velDist(gen)}, harmoniousPalette[colorIndex(gen)]);
            }
            if (p) {
                p->setParticleType(state.currentParticleType);
            }
        }
        
        if (event.type == sf::Event::KeyPressed) {
            switch (event.key.code) {
                case sf::Keyboard::G: state.gravityEnabled = !state.gravityEnabled; break;
                case sf::Keyboard::R: state.repulsionEnabled = !state.repulsionEnabled; if(state.repulsionEnabled) state.collisionsEnabled = false; break;
                case sf::Keyboard::L: state.collisionsEnabled = !state.collisionsEnabled; if(state.collisionsEnabled) state.repulsionEnabled = false; break;
                case sf::Keyboard::C: while (state.particleSystem.getParticleCount() > 0) state.particleSystem.removeParticle(size_t(0)); break;
                case sf::Keyboard::Space: for (int i = 0; i < 20; ++i) state.particleSystem.generateRandomParticle(1.0f, 8.0f)->setParticleType(state.currentParticleType); break;
                case sf::Keyboard::M: state.mouseForceEnabled = !state.mouseForceEnabled; state.mousart.setForceMode(state.mouseForceEnabled); break;
                case sf::Keyboard::N: if (state.mouseForceEnabled) state.mouseForceAttractMode = !state.mouseForceAttractMode; break;
                case sf::Keyboard::Add: case sf::Keyboard::Equal: if (state.mouseForceEnabled) state.mouseForceStrength = std::min(AppState::MAX_MOUSE_FORCE, state.mouseForceStrength + AppState::MOUSE_FORCE_STEP); break;
                case sf::Keyboard::Subtract: case sf::Keyboard::Hyphen: if (state.mouseForceEnabled) state.mouseForceStrength = std::max(AppState::MIN_MOUSE_FORCE, state.mouseForceStrength - AppState::MOUSE_FORCE_STEP); break;
                case sf::Keyboard::K: state.mousart.cycleCursorType(); break;
                case sf::Keyboard::S: state.instructions.setFillColor(state.instructions.getFillColor().a > 0 ? sf::Color::Transparent : sf::Color::White); break;
                case sf::Keyboard::T:
                    if (state.currentParticleType == ParticleType::Original) {
                        state.currentParticleType = ParticleType::Crystal;
                        state.particleTypeName = "Crystal";
                    } else {
                        state.currentParticleType = ParticleType::Original;
                        state.particleTypeName = "Original";
                    }
                    break;
                default: break;
            }
        }
    }
}

void update(sf::Clock& clock, sf::RenderWindow& window, AppState& state) {
    float deltaTime = clock.restart().asSeconds();
    if (deltaTime > 0.1f) deltaTime = 0.1f;

    state.mousePositionWindow = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    state.mousart.update(sf::Mouse::getPosition(window), window);

    if (state.gravityEnabled) {
        state.particleSystem.applyGravityEffect(state.desiredGravitationalAcceleration);
    }
    if (state.repulsionEnabled) {
        state.particleSystem.applyInteractiveForces(AppState::REPULSAO_PADRAO);
    }
    if (state.collisionsEnabled) {
        state.particleSystem.handleCollisions(state.collisionRestitution);
    }
    if (state.mouseForceEnabled) {
        state.particleSystem.applyMouseForce(state.mousePositionWindow, state.mouseForceStrength, state.mouseForceAttractMode);
    }

    state.particleSystem.update(deltaTime);

    std::string statusText = 
        "Controles:\n"
        "G: Gravidade (" + std::string(state.gravityEnabled ? "ON" : "OFF") + ")\n"
        "R: Repulsao (" + std::string(state.repulsionEnabled ? "ON" : "OFF") + ")\n"
        "L: Colisões (" + std::string(state.collisionsEnabled ? "ON" : "OFF") + ")\n"
        "M: Forca do Mouse (" + (state.mouseForceEnabled ? "ON" : "OFF") + ") | " + (state.mouseForceAttractMode ? "Atracao" : "Repulsao") + "\n"
        "T: Tipo de Partícula (" + state.particleTypeName + ")\n"
        "C: Limpar | Espaco: Adicionar\n\n"
        "Partículas: " + std::to_string(state.particleSystem.getParticleCount()) +
        "\nFPS: " + std::to_string(static_cast<int>(1.0f / (deltaTime > 0.0001f ? deltaTime : 0.0001f)));
    state.instructions.setString(sf::String::fromUtf8(statusText.begin(), statusText.end()));
}

void render(sf::RenderWindow& window, AppState& state) {
    window.clear(sf::Color(10, 5, 15));
    window.draw(state.backgroundSprite);
    state.particleSystem.draw(window);
    window.draw(state.instructions);
    state.mousart.draw(window);
    window.display();
}