#include "Particle.h"
#include <cmath>
#include <vector>
#include <string>
#include <random>
#include <map>
#include <memory>

Particle::Particle(float mass, const sf::Vector2f& position, const sf::Vector2f& velocity, const sf::Color& color)
    : vel(velocity), m_accel(0.0f, 0.0f), mass(mass), m_texture(nullptr), m_type(ParticleType::Original) {
    
    radius = 5.0f + mass * 0.5f;
    
    sf::Color enhancedColor = color;
    enhancedColor.r = std::min(255, enhancedColor.r + 40);
    enhancedColor.g = std::min(255, enhancedColor.g + 40);
    enhancedColor.b = std::min(255, enhancedColor.b + 40);
    m_sprite.setColor(enhancedColor);
    
    m_sprite.setPosition(position);
    
    // (carrega textura e configura sprite)
    setParticleType(m_type);
}

// Cache local de texturas para substituir o TextureManager
static std::map<std::string, std::shared_ptr<sf::Texture>> textureCache;

// Função auxiliar para carregar/gerenciar texturas
static sf::Texture* getTexture(const std::string& filename) {
    // Procura a textura no cache
    auto it = textureCache.find(filename);
    if (it != textureCache.end()) {
        return it->second.get();
    }
    
    // Se não existe, carrega a textura
    auto texture = std::make_shared<sf::Texture>();
    if (texture->loadFromFile("assets/" + filename)) {
        textureCache[filename] = texture;
        return texture.get();
    }
    
    return nullptr;
}

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
    
    m_texture = getTexture(textureFile);  // Usa nossa função local ao invés de TextureManager
    if (m_texture) {
        m_sprite.setTexture(*m_texture);
        
        sf::Vector2u textureSize = m_texture->getSize();
        m_sprite.setOrigin(textureSize.x / 2.0f, textureSize.y / 2.0f);
        
        float scaleFactor = 5.0f;
        
        float scale = (radius * scaleFactor) / std::max(textureSize.x, textureSize.y);
        m_sprite.setScale(scale, scale);
    }
}

void Particle::update(float dt) {

    sf::Vector2f currentPos = m_sprite.getPosition();
    
    // Integração de Verlet
    sf::Vector2f newPos = currentPos + vel * dt + 0.5f * m_accel * dt * dt;
    m_sprite.setPosition(newPos);
    
    vel = (newPos - currentPos) / dt;
    
    // amortecimento
    vel *= DAMPING;
    
    m_accel = sf::Vector2f(0.0f, 0.0f);
    
    // rotacionar de leve o sprite para efeito visual questionavel
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
    sf::Vector2f deltaPos = getPosition() - other.getPosition();
    float distance = std::sqrt(deltaPos.x * deltaPos.x + deltaPos.y * deltaPos.y);
    
    return distance < (getRadius() + other.getRadius());
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

void Particle::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    if (m_type == ParticleType::Original) {
        // Renderização original
        sf::CircleShape circle(radius);
        circle.setPosition(getPosition().x - radius, getPosition().y - radius);
        circle.setFillColor(m_sprite.getColor());
        target.draw(circle, states);
    } else if (m_texture) {
        // Com sprite
        target.draw(m_sprite, states);
    }
}
