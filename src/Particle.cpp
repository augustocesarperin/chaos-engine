#include "Particle.h"
#include <cmath>
#include <vector>
#include <string>
#include <random>
#include <map>
#include <memory>

Particle::Particle(float mass, const sf::Vector2f& position, const sf::Vector2f& velocity, const sf::Color& color)
    : vel(velocity), m_accel(0.0f, 0.0f), mass(mass), m_texture(nullptr), m_type(ParticleType::Original),
      m_colorPulsePhase(0.0f), m_useSpeedColor(true) {
    
    radius = 5.0f + mass * 0.5f;
    
    // Melhorar a cor para torná-la mais vibrante
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
    
    m_trail.push_front(std::make_pair(position, enhancedColor));
    
    setParticleType(m_type);
}

// Usando TextureManager para gerenciar texturas centralizadamente
// O cache e carregamento agora é feito pelo TextureManager

void Particle::setParticleType(ParticleType type) {
    m_type = type;
    
    if (type == ParticleType::Original) {
        m_texture = nullptr;
        return;
    }
    
    // Selecionar textura para crystal
    std::string textureFile;
    
    static std::random_device rd;
    static std::mt19937 gen(rd());
    
    std::uniform_int_distribution<> distrib(0, 2);
    int choice = distrib(gen);
    textureFile = std::to_string(choice + 1) + ".png";
    
    m_texture = TextureManager::getTexture("assets/" + textureFile);  // Usa TextureManager centralizado
    if (m_texture) {
        m_sprite.setTexture(*m_texture);
        
        sf::Vector2u textureSize = m_texture->getSize();
        m_sprite.setOrigin(textureSize.x / 2.0f, textureSize.y / 2.0f);
        
        float scaleFactor = 5.0f;
        
        float scale = (radius * scaleFactor) / std::max(textureSize.x, textureSize.y);
        m_sprite.setScale(scale, scale);
    }
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

void Particle::update(float dt) {
    // Guardar a posição atual para o rastro
    sf::Vector2f currentPos = m_sprite.getPosition();
    
    // Integração de Verlet
    sf::Vector2f newPos = currentPos + vel * dt + 0.5f * m_accel * dt * dt;
    m_sprite.setPosition(newPos);
    
    vel = (newPos - currentPos) / dt;
    
    // Amortecimento
    vel *= DAMPING;
    
    // Resetar aceleração
    m_accel = sf::Vector2f(0.0f, 0.0f);
    
    updateTrailColor();
    
    // Adicionar nova posição ao rastro se a partícula se moveu o suficiente
    // Otimização: Comparar quadrados em vez de usar sqrt
    const float minDistanceSq = 4.0f; 
    const float dx = newPos.x - currentPos.x;
    const float dy = newPos.y - currentPos.y;
    const float distanceSq = dx*dx + dy*dy;
    
    if (distanceSq > minDistanceSq || m_trail.empty()) {
        // Pegar a cor atual para o rastro
        sf::Color trailColor = m_sprite.getColor();
        trailColor.a = 200;  // Semi-transparente
        
        m_trail.push_front(std::make_pair(newPos, trailColor));
        
        if (m_trail.size() > MAX_TRAIL_LENGTH) {
            m_trail.pop_back();
        }
    }
    
    // Atualizar cores do rastro - fading
    for (size_t i = 1; i < m_trail.size(); ++i) {
        sf::Color& color = m_trail[i].second;
        color.a = static_cast<uint8_t>(color.a * TRAIL_FADE_RATE);
    }
    
    // Fase de pulsação de cor
    m_colorPulsePhase += dt * 2.0f;
    if (m_colorPulsePhase > 2.0f * M_PI) m_colorPulsePhase -= 2.0f * M_PI;
    
    // Rotacionar o sprite para efeito visual
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

bool Particle::checkCollision(const Particle& other) const {
    // Otimização* Comparar quadrados em vez de calcular raíz
    const sf::Vector2f deltaPos = getPosition() - other.getPosition();
    const float distanceSq = deltaPos.x * deltaPos.x + deltaPos.y * deltaPos.y;
    const float sumRadius = getRadius() + other.getRadius();
    
    return distanceSq < (sumRadius * sumRadius);
}

void Particle::resolveCollision(Particle& other) {
    sf::Vector2f delta = getPosition() - other.getPosition();
    float distance = std::sqrt(delta.x * delta.x + delta.y * delta.y);
    
    if (distance == 0) return;
    
    sf::Vector2f normal = sf::Vector2f(delta.x / distance, delta.y / distance);
    sf::Vector2f rv = vel - other.vel;
    float velAlongNormal = rv.x * normal.x + rv.y * normal.y;
    
    if (velAlongNormal > 0) return;
    
    float e = 0.8f; // elasticidade

    float j = -(1 + e) * velAlongNormal;
    j /= 1/mass + 1/other.mass;
    
    sf::Vector2f impulse = sf::Vector2f(j * normal.x, j * normal.y);
    vel += sf::Vector2f(impulse.x / mass, impulse.y / mass);
    other.vel -= sf::Vector2f(impulse.x / other.mass, impulse.y / other.mass);
    
    float overlap = getRadius() + other.getRadius() - distance;
    if (overlap > 0) {
        float moveRatio1 = getRadius() / (getRadius() + other.getRadius());
        float moveRatio2 = other.getRadius() / (getRadius() + other.getRadius());
        
        sf::Vector2f correction = normal * overlap;
        m_sprite.setPosition(getPosition() + correction * moveRatio1);
        other.m_sprite.setPosition(other.getPosition() - correction * moveRatio2);
    }
}

void Particle::keepInBounds(float width, float height) {
    sf::Vector2f position = m_sprite.getPosition();
    
    if (position.x - radius < 0) {
        m_sprite.setPosition(radius, position.y);
        
        float impactSpeed = std::abs(vel.x);
        float restitution = 0.9f - (impactSpeed / 300.0f); 
        restitution = std::max(0.1f, std::min(0.9f, restitution)); 
        
        vel.x = -vel.x * restitution;
    }
    else if (position.x + radius > width) {
        m_sprite.setPosition(width - radius, position.y);
        
        float impactSpeed = std::abs(vel.x);
        float restitution = 0.9f - (impactSpeed / 300.0f);
        restitution = std::max(0.1f, std::min(0.9f, restitution)); 
        
        vel.x = -vel.x * restitution;
    }
    
    if (position.y - radius < 0) {
        m_sprite.setPosition(position.x, radius);
        
        float impactSpeed = std::abs(vel.y);
        float restitution = 0.9f - (impactSpeed / 300.0f);
        restitution = std::max(0.1f, std::min(0.9f, restitution)); 
        
        vel.y = -vel.y * restitution;
    }
    else if (position.y + radius > height) {
        m_sprite.setPosition(position.x, height - radius);
        
        float impactSpeed = std::abs(vel.y);
        float restitution = 0.9f - (impactSpeed / 300.0f);
        restitution = std::max(0.1f, std::min(0.9f, restitution)); 
        
        vel.y = -vel.y * restitution;
        
        // atrito com o solo
        float frictionCoeff = 0.05f; 
        vel.x *= (1.0f - frictionCoeff);
    }
}

void Particle::updateTrailColor() {
    if (m_useSpeedColor) {
        // Obter a velocidade e mapear para uma cor
        float speed = std::sqrt(vel.x * vel.x + vel.y * vel.y);
        
        // Extrair componentes HSV da cor base
        float h, s, v;
        RGBtoHSV(m_baseColor.r, m_baseColor.g, m_baseColor.b, h, s, v);
        
        // Mapeamento de velocidade para matiz
        // Baixa velocidade: Azul/Verde (240/120)
        // Alta velocidade: Vermelho/Laranja (0/30)
        float speedFactor = std::min(1.0f, speed / 500.0f); // Normalizar até 500
        float newHue = 240.0f - speedFactor * 240.0f; // Mapear de azul para vermelho
        
        // Pulsar saturação e brilho baseado na fase
        float pulseFactor = (std::sin(m_colorPulsePhase) + 1.0f) * 0.1f;
        s = std::min(1.0f, s + pulseFactor);
        v = std::min(1.0f, v + pulseFactor);
        
        // Converter de volta para RGB
        uint8_t r, g, b;
        HSVtoRGB(newHue, s, v, r, g, b);
        
        // Definir a nova cor
        sf::Color newColor(r, g, b, m_sprite.getColor().a);
        m_sprite.setColor(newColor);
    }
}

void Particle::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    // Otimização: Verificar se a partícula está dentro da área visível
    const sf::Vector2f particlePos = getPosition();
    const sf::View& view = target.getView();
    const sf::Vector2f viewCenter = view.getCenter();
    const sf::Vector2f viewSize = view.getSize();
    
    const float visibleMargin = 50.0f;
    
    // Cálculo de limites da área visível
    const float minX = viewCenter.x - viewSize.x/2.0f - visibleMargin - radius;
    const float maxX = viewCenter.x + viewSize.x/2.0f + visibleMargin + radius;
    const float minY = viewCenter.y - viewSize.y/2.0f - visibleMargin - radius;
    const float maxY = viewCenter.y + viewSize.y/2.0f + visibleMargin + radius;
    
    // Verificar se fora da tela
    if (particlePos.x < minX || particlePos.x > maxX || 
        particlePos.y < minY || particlePos.y > maxY) {
        return; 
    }
    
    // Desenhar o rastro primeiro
    const size_t trailSize = m_trail.size();
    for (size_t i = trailSize - 1; i > 0; --i) {
        const auto& [pos, color] = m_trail[i];
        
        // Verificar se o ponto do rastro está visível
        if (pos.x < minX || pos.x > maxX || pos.y < minY || pos.y > maxY) {
            continue; 
        }
        // cache de cálculos repetidos
        const float trailFactor = static_cast<float>(i) / trailSize;
        const float trailRadius = radius * (0.5f + 0.5f * trailFactor);
        
        // Desenhar círculos para rastros de partículas originais
        if (m_type == ParticleType::Original) {
            sf::CircleShape trailCircle(trailRadius);
            trailCircle.setPosition(pos.x - trailRadius, pos.y - trailRadius);
            trailCircle.setFillColor(color);
            target.draw(trailCircle, states);
        }
        // Para partículas com sprite, desenhar versões menores e mais transparentes do sprite
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
    }
    
    if (m_type == ParticleType::Original) {
        sf::CircleShape circle(radius);
        circle.setPosition(particlePos.x - radius, particlePos.y - radius);
        circle.setFillColor(m_sprite.getColor());
        target.draw(circle, states);
    } else if (m_texture) {
        // Com sprite
        target.draw(m_sprite, states);
    }
}
