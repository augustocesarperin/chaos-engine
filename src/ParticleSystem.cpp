#include "ParticleSystem.h"
#include <random>
#include <cmath>

ParticleSystem::ParticleSystem(float width, float height)
    : m_width(width), m_height(height) {
}

void ParticleSystem::setWindowSize(float width, float height) {
    m_width = width;
    m_height = height;
}

Particle& ParticleSystem::addParticle(float mass, const sf::Vector2f& position, const sf::Vector2f& velocity, const sf::Color& color) {
    m_particles.emplace_back(mass, position, velocity, color);
    return m_particles.back();
}

void ParticleSystem::removeParticle(size_t index) {
    if (index < m_particles.size()) {
        m_particles.erase(m_particles.begin() + index);
    }
}

void ParticleSystem::update(float deltaTime) {
    for (auto& particle : m_particles) {
        // Aplicar resistência do ar baseada no tamanho e velocidade
        float airResistance = 0.002f / particle.getMass(); 
        particle.applyDrag(airResistance);
        particle.update(deltaTime);
        particle.keepInBounds(m_width, m_height);
    }
    
    for (size_t i = 0; i < m_particles.size(); ++i) {
        for (size_t j = i + 1; j < m_particles.size(); ++j) {
            if (m_particles[i].checkCollision(m_particles[j])) {
                m_particles[i].resolveCollision(m_particles[j]);
            }
        }
    }
}

void ParticleSystem::applyGravityEffect(float gravitationalAcceleration) {
    for (auto& particle : m_particles) {
        if (particle.getMass() > 0) { // Evitar divisão por zero ou problemas com massa inválida
            sf::Vector2f gravityForce(0.0f, particle.getMass() * gravitationalAcceleration);
            particle.applyForce(gravityForce);
        }
    }
}

void ParticleSystem::draw(sf::RenderWindow& window) {
    for (const auto& particle : m_particles) {
        window.draw(particle);
    }
}

void ParticleSystem::applyInteractiveForces(float strength) {
    for (size_t i = 0; i < m_particles.size(); ++i) {
        for (size_t j = i + 1; j < m_particles.size(); ++j) {
            sf::Vector2f delta = m_particles[i].getPosition() - m_particles[j].getPosition();
            float distance = std::sqrt(delta.x * delta.x + delta.y * delta.y);
            
            if (distance < 1.0f) distance = 1.0f;
            
            float forceMagnitude = strength * m_particles[i].getMass() * m_particles[j].getMass() / (distance * distance);
            // Direção da força 
            sf::Vector2f forceDirection = sf::Vector2f(delta.x / distance, delta.y / distance);
            sf::Vector2f force = forceDirection * forceMagnitude;
            // Se força > 0: repulsão, se força< 0: atração
            m_particles[i].applyForce(force);
            m_particles[j].applyForce(-force);
        }
    }
}

void ParticleSystem::applyMouseForce(const sf::Vector2f& mousePosition, float strength, bool attractMode) {
    for (auto& particle : m_particles) {
        sf::Vector2f direction = mousePosition - particle.getPosition();
        float distanceSquared = direction.x * direction.x + direction.y * direction.y;

        if (distanceSquared < 1.0f) { 
            continue; 
        }

        float distance = std::sqrt(distanceSquared);  
        sf::Vector2f normalizedDirection = direction / distance;
        float forceMagnitude = strength / distance;
        sf::Vector2f force = normalizedDirection * forceMagnitude;
       
        if (!attractMode) { 
            force = -force;
        }
        particle.applyForce(force);
    }
}

void ParticleSystem::generateRandomParticles(int count, float minMass, float maxMass) {
    for (int i = 0; i < count; ++i) {
        generateRandomParticle(minMass, maxMass);
    }
}

Particle& ParticleSystem::generateRandomParticle(float minMass, float maxMass) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> massDist(minMass, maxMass);
    std::uniform_real_distribution<float> posDist_x(0.0f, m_width);
    std::uniform_real_distribution<float> posDist_y(0.0f, m_height);
    std::uniform_real_distribution<float> velDist(-50.0f, 50.0f);
    std::uniform_int_distribution<int> colorDist(0, 255);
    
    float mass = massDist(gen);
    sf::Vector2f position(posDist_x(gen), posDist_y(gen));
    sf::Vector2f velocity(velDist(gen), velDist(gen));
    sf::Color color(colorDist(gen), colorDist(gen), colorDist(gen));
    
    return addParticle(mass, position, velocity, color);
}
