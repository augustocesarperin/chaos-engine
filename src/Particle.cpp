#include "Particle.h"
#include "TextureManager.h"
#include "Utility.h"
#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include <random>
#include <map>
#include <memory>

Particle::Particle(float mass, const sf::Vector2f& position, const sf::Vector2f& velocity, const sf::Color& color)
    : vel(velocity), m_accel(0.0f, 0.0f), mass(mass), m_texture(nullptr), m_type(ParticleType::Original),
      m_colorPulsePhase(0.0f), m_useSpeedColor(true) {
    
    radius = 5.0f + mass * 1.0f;
    sf::Color enhancedColor = color;
    
    float h, s, v;
    RGBtoHSV(enhancedColor.r, enhancedColor.g, enhancedColor.b, h, s, v);
    s = std::min(1.0f, s * 1.3f);  
    v = std::min(1.0f, v * 1.2f); 
    
    uint8_t r, g, b;
    HSVtoRGB(h, s, v, r, g, b);
    enhancedColor.r = r;
    enhancedColor.g = g;
    enhancedColor.b = b;
    
    m_baseColor = enhancedColor;
    m_sprite.setColor(enhancedColor);
    
    m_sprite.setPosition(position);
    
    m_trailBuffer[0] = {position, enhancedColor};
    m_trailHead = 0;
    m_trailSize = 1;
    
    setParticleType(m_type);
}

void Particle::setParticleType(ParticleType type) {
    // Preservar a cor atual antes de mudar o tipo
    sf::Color currentColor = m_sprite.getColor();
    
    m_type = type;
    
    if (type == ParticleType::Original) {
        m_texture = nullptr;
        updateTrailColor();
        return;
    }
    std::string textureFile;
    
    static std::random_device rd;
    static std::mt19937 gen(rd());
    
    std::uniform_int_distribution<> distrib(0, 2);
    int choice = distrib(gen);
    textureFile = std::to_string(choice + 1) + ".png";
    
    m_texture = TextureManager::getTexture(textureFile);
    
    if (!m_texture || m_texture->getSize().x == 0) {
        m_texture = TextureManager::getTexture("assets/" + textureFile);
    }
    
    if (m_texture && m_texture->getSize().x > 0) {
        m_sprite.setTexture(*m_texture);
        
        sf::Vector2u textureSize = m_texture->getSize();
        m_sprite.setOrigin(textureSize.x / 2.0f, textureSize.y / 2.0f);
        
        float scaleFactor = 5.0f;
        
        float scale = (radius * scaleFactor) / std::max(textureSize.x, textureSize.y);
        m_sprite.setScale(scale, scale);
    
        currentColor.a = 255;
        m_sprite.setColor(currentColor);
    } else {
        std::cerr << "Não foi possível carregar a textura: " << textureFile << std::endl;
        m_type = ParticleType::Original;
        m_texture = nullptr;
        
        sf::Color fallbackColor = m_baseColor;
        fallbackColor.r = std::max(100u, (unsigned int)fallbackColor.r);
        fallbackColor.g = std::max(100u, (unsigned int)fallbackColor.g);
        fallbackColor.b = std::max(100u, (unsigned int)fallbackColor.b);
        fallbackColor.a = 255;
        m_sprite.setColor(fallbackColor);
    }
    
    // Atualizar a cor com base na velocidade
    updateTrailColor();
}

// Função auxiliar para conversão de RGB para HSV
void Particle::RGBtoHSV(uint8_t r, uint8_t g, uint8_t b, float& h, float& s, float& v) {
    float rf = r / 255.0f;
    float gf = g / 255.0f;
    float bf = b / 255.0f;
    
    float cmax = std::max(std::max(rf, gf), bf);
    float cmin = std::min(std::min(rf, gf), bf);
    float delta = cmax - cmin;
    
    // Hue
    if (delta == 0) h = 0;
    else if (cmax == rf) h = fmod(((gf - bf) / delta), 6.0f) * 60.0f;
    else if (cmax == gf) h = ((bf - rf) / delta + 2.0f) * 60.0f;
    else h = ((rf - gf) / delta + 4.0f) * 60.0f;
    
    if (h < 0) h += 360.0f;
    s = (cmax == 0) ? 0 : delta / cmax;
    v = cmax;
}

// Função auxiliar para conversão de HSV para RGB
void Particle::HSVtoRGB(float h, float s, float v, uint8_t& r, uint8_t& g, uint8_t& b) {
    float c = v * s;
    float x = c * (1 - std::abs(fmod(h / 60.0f, 2) - 1));
    float m = v - c;
    
    float rf, gf, bf;
    
    if (h >= 0 && h < 60) { rf = c; gf = x; bf = 0; }
    else if (h >= 60 && h < 120) { rf = x; gf = c; bf = 0; }
    else if (h >= 120 && h < 180) { rf = 0; gf = c; bf = x; }
    else if (h >= 180 && h < 240) { rf = 0; gf = x; bf = c; }
    else if (h >= 240 && h < 300) { rf = x; gf = 0; bf = c; }
    else { rf = c; gf = 0; bf = x; }
    
    r = static_cast<uint8_t>((rf + m) * 255);
    g = static_cast<uint8_t>((gf + m) * 255);
    b = static_cast<uint8_t>((bf + m) * 255);
}

void Particle::updateVisuals(float dt) {
    // A posição da partícula já foi atualizada pela física em C.
    // Usamos a posição atual para o rastro.
    sf::Vector2f currentPos = m_sprite.getPosition();
    
    updateTrailColor();
    
    // Adicionar nova posição ao rastro.
    // Usamos o último ponto adicionado para checar a distância, para evitar rastros "pontilhados".
    const float minDistanceSq = 4.0f; 
    sf::Vector2f lastTrailPos = m_trailBuffer[ (m_trailHead -1 + MAX_TRAIL_LENGTH) % MAX_TRAIL_LENGTH].position;
    const float dx = currentPos.x - lastTrailPos.x;
    const float dy = currentPos.y - lastTrailPos.y;
    const float distanceSq = dx*dx + dy*dy;
    
    if (distanceSq > minDistanceSq || m_trailSize <= 1) {
        sf::Color trailColor = m_sprite.getColor();
        trailColor.a = 200;  
        
        m_trailBuffer[m_trailHead].position = currentPos;
        m_trailBuffer[m_trailHead].color = trailColor;
        
        m_trailHead = (m_trailHead + 1) % MAX_TRAIL_LENGTH;
        
        if (m_trailSize < MAX_TRAIL_LENGTH) {
            m_trailSize++;
        }
    }
    
    // Atualizar cores do rastro - fading
    for (int i = 1; i < m_trailSize; ++i) {
        int idx = (m_trailHead - i + MAX_TRAIL_LENGTH) % MAX_TRAIL_LENGTH;
        m_trailBuffer[idx].color.a = static_cast<uint8_t>(m_trailBuffer[idx].color.a * TRAIL_FADE_RATE);
    }
    
    // Fase de pulsação de cor
    m_colorPulsePhase += dt * 2.0f;
    if (m_colorPulsePhase > 2.0f * M_PI) m_colorPulsePhase -= 2.0f * M_PI;
    
    // Rotacionar o sprite p efeito visual
    static float rotationSpeed = 15.0f;
    m_sprite.rotate(rotationSpeed * dt);
}

void Particle::applyForce(const sf::Vector2f& f) {
    m_accel += sf::Vector2f(f.x / mass, f.y / mass);
}

void Particle::applyDrag(float dragCoefficient) {
    // Calcular a vel 
    float speedSquared = vel.x * vel.x + vel.y * vel.y;
    
    if (speedSquared > 0.1f) { // Ignorar vel
        float speed = std::sqrt(speedSquared);
        float dragMagnitude = dragCoefficient * speedSquared;
        sf::Vector2f dragForce = -dragMagnitude * sf::Vector2f(vel.x/speed, vel.y/speed);
        applyForce(dragForce);
    }
}

void Particle::updateTrailColor() {
    if (m_useSpeedColor) {
        float speed = sqrtf(vel.x * vel.x + vel.y * vel.y);
        
        // Extrair componentes HSV da cor base
        float h, s, v;
        RGBtoHSV(m_baseColor.r, m_baseColor.g, m_baseColor.b, h, s, v);
        
        if (std::isnan(h) || std::isinf(h)) h = 0.0f;
        s = std::max(0.5f, std::min(1.0f, s)); // Forçar saturação mínima de 50%
        v = std::max(0.5f, std::min(1.0f, v)); // Forçar brilho mínimo de 50%
        
        float speedFactor = std::min(1.0f, speed / 500.0f); 
        float newHue = 240.0f - speedFactor * 240.0f; 
        
        float pulseFactor = (std::sin(m_colorPulsePhase) + 1.0f) * 0.1f;
        s = std::min(1.0f, s + pulseFactor);
        v = std::min(1.0f, v + pulseFactor);
        
        uint8_t r, g, b;
        HSVtoRGB(newHue, s, v, r, g, b);
        
        sf::Color newColor(r, g, b, 255);
        m_sprite.setColor(newColor);
    } else {
        sf::Color color = m_baseColor;
        color.a = 255;
        m_sprite.setColor(color);
    }
}

void Particle::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    const sf::Vector2f particlePos = m_sprite.getPosition();
    
    float radius = this->radius; 
    if (m_texture) {
        radius = m_sprite.getScale().x * m_texture->getSize().x * 0.5f;
    }

    try {
        if (!Utility::isVisible(particlePos, radius, target.getView())) {
            return;
        }
        
        for (int i = m_trailSize - 1; i > 0; --i) {
            try {
                int idx = (m_trailHead - i + MAX_TRAIL_LENGTH) % MAX_TRAIL_LENGTH;
                
                if (idx < 0 || idx >= MAX_TRAIL_LENGTH) {
                    continue;
                }
                
                const sf::Vector2f& pos = m_trailBuffer[idx].position;
                const sf::Color& color = m_trailBuffer[idx].color;
                
                if (!Utility::isVisible(pos, radius * 0.5f, target.getView())) {
                    continue;
                }
                const float trailFactor = static_cast<float>(i) / m_trailSize;
                const float trailRadius = radius * (0.5f + 0.5f * trailFactor);
                
                // Desenhar círculos para rastros de partículas originais
                if (m_type == ParticleType::Original) {
                    sf::CircleShape trailCircle(trailRadius);
                    trailCircle.setPosition(pos.x - trailRadius, pos.y - trailRadius);
                    trailCircle.setFillColor(color);
                    target.draw(trailCircle, states);
                }
                else if (m_texture) {
                    sf::Sprite trailSprite(*m_texture);
                    
                    const sf::Vector2u& textureSize = m_texture->getSize();
                    const float halfTexX = textureSize.x / 2.0f;
                    const float halfTexY = textureSize.y / 2.0f;
                    trailSprite.setOrigin(halfTexX, halfTexY);
                    
                    const float maxTextureDim = static_cast<float>(std::max(textureSize.x, textureSize.y));
                    const float scale = trailRadius * 10.0f / maxTextureDim;
                    
                    trailSprite.setScale(scale, scale);
                    trailSprite.setPosition(pos);
                    trailSprite.setColor(color);
                    
                    target.draw(trailSprite, states);
                }
            } catch (...) {
                // Silenciar erro durante trail
            }
        }
        
        if (m_type == ParticleType::Original) {
            sf::CircleShape circle(radius);
            circle.setPosition(particlePos.x - radius, particlePos.y - radius);
            circle.setFillColor(m_sprite.getColor());
            target.draw(circle, states);
        } else if (m_texture) {
            target.draw(m_sprite, states);
        } else {
            sf::CircleShape circle(radius);
            circle.setPosition(particlePos.x - radius, particlePos.y - radius);
            circle.setFillColor(m_sprite.getColor());
            target.draw(circle, states);
        }
    } catch(...) {
    }
}
