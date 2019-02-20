/*
 * Particle Simulator
 * 
 * by: Augusto César Perin
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
    static constexpr float RESTITUICAO_PADRAO = 0.7f;
    static constexpr float DEFAULT_MOUSE_FORCE = 3750.0f;
    static constexpr float MIN_MOUSE_FORCE = 50.0f;
    static constexpr float MAX_MOUSE_FORCE = 25000.0f;
    static constexpr float MOUSE_FORCE_STEP = 250.0f;
    
    float desiredGravitationalAcceleration = GRAVIDADE_PADRAO;
    bool gravityEnabled = true;
    bool repulsionEnabled = false;
    bool collisionsEnabled = true;
    float collisionRestitution = RESTITUICAO_PADRAO;
    
    ParticleType currentParticleType = ParticleType::Original;
    std::string particleTypeName = "Original";
    bool showInstructions = true;
    bool mouseCursorVisible = false;

    enum ForcePatternMode { Standard, Vortex, PulseWave, ForceLine, COUNT };
    ForcePatternMode currentForceMode = Standard;
    std::string forceModeName = "Padrão";
    
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
void updatePhysics(AppState& state, float dt);

void updateUI(sf::RenderWindow& window, AppState& state, float real_dt);
void render(sf::RenderWindow& window, AppState& state);

int main()
{
    try {
        const int WIDTH = 800;
        const int HEIGHT = 600;
    
    std::string windowTitle = "Chaos - by Augusto César Perin";
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), sf::String::fromUtf8(windowTitle.begin(), windowTitle.end()));
        window.setFramerateLimit(120);

        AppState state(WIDTH, HEIGHT);
        setup(window, state);
        
        sf::Clock clock;
        sf::Time timeSinceLastUpdate = sf::Time::Zero;
        const sf::Time TimePerFrame = sf::seconds(1.f / 60.f);
        
        while (window.isOpen()) {
            sf::Time elapsedTime = clock.restart();
            timeSinceLastUpdate += elapsedTime;
            
            processInput(window, state);
            
            while (timeSinceLastUpdate > TimePerFrame) {
                timeSinceLastUpdate -= TimePerFrame;
                updatePhysics(state, TimePerFrame.asSeconds());
            }
            
            updateUI(window, state, elapsedTime.asSeconds());
            render(window, state);
        }
    } catch (const std::exception& e) {
        std::cerr << "ERRO FATAL: " << e.what() << std::endl;
        system("pause");
        return -1;
    }
    return 0;
}

void setup(sf::RenderWindow& window, AppState& state) {
    window.setFramerateLimit(60);

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

    if (!state.font.loadFromFile("assets/fonts/PressStart2P-Regular.ttf")) {
        if (!state.font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
            std::cerr << "ERRO FATAL: Nenhuma fonte encontrada. O texto não será exibido." << std::endl;
            throw std::runtime_error("Font not found");
        }
    }
    
    state.instructions.setFont(state.font);
    state.instructions.setCharacterSize(8); 
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
        if (event.type == sf::Event::Resized) {
            sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
            window.setView(sf::View(visibleArea));

            sf::Vector2u textureSize = state.backgroundTexture.getSize();
            float scaleX = static_cast<float>(event.size.width) / textureSize.x;
            float scaleY = static_cast<float>(event.size.height) / textureSize.y;
            state.backgroundSprite.setScale(scaleX, scaleY);
            
            state.particleSystem.setWindowSize(static_cast<float>(event.size.width), static_cast<float>(event.size.height));
        }
        if (event.type == sf::Event::Closed) {
            window.close();
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
                p = state.particleSystem.addParticle(10.0f, position, {velDist(gen), velDist(gen)}, harmoniousPalette[colorIndex(gen)]);
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
                    case sf::Keyboard::Space:
                        for (int i = 0; i < 20; ++i) {
                        state.particleSystem.generateRandomParticle(2.0f, 2.0f)->setParticleType(state.currentParticleType); 
                        }
                        break;
                case sf::Keyboard::M: state.mouseForceEnabled = !state.mouseForceEnabled; state.mousart.setForceMode(state.mouseForceEnabled); break;
                case sf::Keyboard::N: if (state.mouseForceEnabled) state.mouseForceAttractMode = !state.mouseForceAttractMode; break;
                case sf::Keyboard::Add: case sf::Keyboard::Equal: if (state.mouseForceEnabled) state.mouseForceStrength = std::min(AppState::MAX_MOUSE_FORCE, state.mouseForceStrength + AppState::MOUSE_FORCE_STEP); break;
                case sf::Keyboard::Subtract: case sf::Keyboard::Hyphen: if (state.mouseForceEnabled) state.mouseForceStrength = std::max(AppState::MIN_MOUSE_FORCE, state.mouseForceStrength - AppState::MOUSE_FORCE_STEP); break;
                case sf::Keyboard::K: state.mousart.cycleCursorType(); break;
                case sf::Keyboard::S: state.instructions.setFillColor(state.instructions.getFillColor().a > 0 ? sf::Color::Transparent : sf::Color::White); break;
                case sf::Keyboard::I: state.collisionRestitution = std::min(1.0f, state.collisionRestitution + 0.05f); break;
                case sf::Keyboard::U: state.collisionRestitution = std::max(0.0f, state.collisionRestitution - 0.05f); break;
                    case sf::Keyboard::T:
                    if (state.currentParticleType == ParticleType::Original) {
                        state.currentParticleType = ParticleType::Crystal;
                        state.particleTypeName = "Crystal";
                        } else {
                        state.currentParticleType = ParticleType::Original;
                        state.particleTypeName = "Original";
                        }
                        break;
                case sf::Keyboard::F:
                    state.currentForceMode = static_cast<AppState::ForcePatternMode>((state.currentForceMode + 1) % AppState::ForcePatternMode::COUNT);
                    switch (state.currentForceMode) {
                        case AppState::Standard:   state.forceModeName = "Padrão"; break;
                        case AppState::Vortex:     state.forceModeName = "Redemoinho"; break;
                        case AppState::PulseWave:  state.forceModeName = "Onda de Pulso"; break;
                        case AppState::ForceLine:  state.forceModeName = "Linha de Força"; break;
                        default: break;
                        }
                        break;
                default: break;
            }
        }
    }
}

void updatePhysics(AppState& state, float dt) {
    ParticleSystem::PhysicsInputState inputs;
    inputs.gravityEnabled = state.gravityEnabled;
    inputs.gravitationalAcceleration = state.desiredGravitationalAcceleration;
    inputs.repulsionEnabled = state.repulsionEnabled;
    inputs.repulsionStrength = AppState::REPULSAO_PADRAO;
    inputs.collisionRestitution = state.collisionRestitution;
    inputs.mouseForceEnabled = state.mouseForceEnabled;
    inputs.mousePosition = state.mousePositionWindow;
    inputs.mouseForceStrength = state.mouseForceStrength;
    inputs.mouseForceAttractMode = state.mouseForceAttractMode;
    inputs.forceMode = state.currentForceMode;

    state.particleSystem.update(dt, inputs);
}

void updateUI(sf::RenderWindow& window, AppState& state, float real_dt) {
    state.mousePositionWindow = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    state.mousart.update(sf::Mouse::getPosition(window), window);

    float fps = (real_dt > 0.0001f) ? 1.0f / real_dt : 0.0f;

        std::string statusText = 
            "Controles:\n"
        "Botão esquerdo/direito: Adicionar partícula\n"
        "G: Gravidade (" + std::string(state.gravityEnabled ? "ON" : "OFF") + ")\n"
        "R: Repulsao (" + std::string(state.repulsionEnabled ? "ON" : "OFF") + ")\n"
        "L: Colisões (" + std::string(state.collisionsEnabled ? "ON" : "OFF") + ")\n"
        "M: Força do Mouse (" + std::string(state.mouseForceEnabled ? "ON" : "OFF") + ")\n"
        "N: Modo da Força (" + std::string(state.mouseForceAttractMode ? "Atrair" : "Repelir") + ")\n"
        "F: Padrão da Força (" + state.forceModeName + ")\n"
        "+/-: Intensidade da Força (" + std::to_string(static_cast<int>(state.mouseForceStrength)) + ")\n"
        "I/U: Restituição (" + std::to_string(state.collisionRestitution).substr(0, 4) + ")\n"
        "T: Tipo de Partícula (" + state.particleTypeName + ")\n"
        "K: Alternar Mouse\n"
        "S: Mostrar/Ocultar Controles\n"
        "C: Limpar Tudo | Espaço: Adicionar Aleatórias\n\n"
        "Partículas: " + std::to_string(state.particleSystem.getParticleCount()) +
        "\nFPS: " + std::to_string(static_cast<int>(fps));
    state.instructions.setString(sf::String::fromUtf8(statusText.begin(), statusText.end()));
}

void render(sf::RenderWindow& window, AppState& state) {
    window.clear();
    window.draw(state.backgroundSprite);

    state.particleSystem.draw(window);
    
    if (state.showInstructions) {
        window.draw(state.instructions);
    }

    state.mousart.draw(window);
    window.display();
}