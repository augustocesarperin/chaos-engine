#include "Particle.h"
#include <cmath>

Particle::Particle(float mass, const sf::Vector2f& position, const sf::Vector2f& velocity, const sf::Color& color)
    : vel(velocity), m_accel(0.0f, 0.0f), mass(mass) {
    
    float radius = 5.0f + mass * 0.5f;
    
    m_shape.setRadius(radius);
    m_shape.setOrigin(radius, radius);
    m_shape.setPosition(position);
    m_shape.setFillColor(color);
}

void Particle::update(float dt) {
    vel += m_accel * dt;
    vel *= DAMPING;
    
    m_shape.move(vel * dt);
    
    // Reset para a próxima frame
    m_accel = sf::Vector2f(0.0f, 0.0f);
}

void Particle::applyForce(const sf::Vector2f& f) {
    m_accel += sf::Vector2f(f.x / mass, f.y / mass);
}

void Particle::applyDrag(float dragCoefficient) {
    // Calcular a vel atual
    float speedSquared = vel.x * vel.x + vel.y * vel.y;
    
    if (speedSquared > 0.1f) { // Ignorar vel baixas
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
    
    float e = 0.8f; // elasticidade da colisão
    
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
        m_shape.setPosition(getPosition() + correction * moveRatio1);
        other.m_shape.setPosition(other.getPosition() - correction * moveRatio2);
    }
}

void Particle::keepInBounds(float width, float height) {
    sf::Vector2f position = m_shape.getPosition();
    float radius = m_shape.getRadius();
    
    if (position.x - radius < 0) {
        m_shape.setPosition(radius, position.y);
        
        // Calcular coeficiente de restituição baseado na velocidade de impacto
        float impactSpeed = std::abs(vel.x);
        float restitution = 0.9f - (impactSpeed / 300.0f); // Diminui com velocidade alta
        restitution = std::max(0.1f, std::min(0.9f, restitution)); // Limitar entre 0.1 e 0.9
        
        vel.x = -vel.x * restitution;
    }
    else if (position.x + radius > width) {
        m_shape.setPosition(width - radius, position.y);
        
        // Calcular coeficiente de restituição baseado na velocidade de impacto
        float impactSpeed = std::abs(vel.x);
        float restitution = 0.9f - (impactSpeed / 300.0f); // Diminui com velocidade alta
        restitution = std::max(0.1f, std::min(0.9f, restitution)); // Limitar entre 0.1 e 0.9
        
        vel.x = -vel.x * restitution;
    }
    
    if (position.y - radius < 0) {
        m_shape.setPosition(position.x, radius);
        
        // Calcular coeficiente de restituição baseado na velocidade de impacto
        float impactSpeed = std::abs(vel.y);
        float restitution = 0.9f - (impactSpeed / 300.0f); // Diminui com velocidade alta
        restitution = std::max(0.1f, std::min(0.9f, restitution)); // Limitar entre 0.1 e 0.9
        
        vel.y = -vel.y * restitution;
    }
    else if (position.y + radius > height) {
        m_shape.setPosition(position.x, height - radius);
        
        // Calcular coeficiente de restituição baseado na velocidade de impacto
        float impactSpeed = std::abs(vel.y);
        float restitution = 0.9f - (impactSpeed / 300.0f); // Diminui com velocidade alta
        restitution = std::max(0.1f, std::min(0.9f, restitution)); // Limitar entre 0.1 e 0.9
        
        vel.y = -vel.y * restitution;
    }
}

void Particle::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(m_shape, states);
}
