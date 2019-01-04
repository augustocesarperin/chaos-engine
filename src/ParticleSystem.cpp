#include "ParticleSystem.h"
#include <random>
#include <cmath>

ParticleSystem::ParticleSystem(float width, float height)
    : m_width(width), m_height(height) {
}

ParticleSystem::~ParticleSystem() {
    // Destrutor vazio, não há recursos externos para liberar
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

void ParticleSystem::handleCollisions(float restitution) {
    // Verifica todas as combinações possíveis de partículas
    for (size_t i = 0; i < m_particles.size(); ++i) {
        for (size_t j = i + 1; j < m_particles.size(); ++j) {
            Particle& p1 = m_particles[i];
            Particle& p2 = m_particles[j];
            
            // Calcula a distância entre os centros das partículas
            sf::Vector2f delta = p1.getPosition() - p2.getPosition();
            float distanceSquared = delta.x * delta.x + delta.y * delta.y;
            float radiusSum = p1.getRadius() + p2.getRadius();
            
            // Verifica se as partículas estão colidindo (distância menor que a soma dos raios)
            if (distanceSquared < radiusSum * radiusSum) {
                float distance = std::sqrt(distanceSquared);
                
                // Evita divisão por zero
                if (distance < 0.01f) {
                    distance = 0.01f;
                }
                
                // Direção normal da colisão
                sf::Vector2f normal = delta / distance;
                
                // Calcula a velocidade relativa na direção normal
                sf::Vector2f v1 = p1.getVelocity();
                sf::Vector2f v2 = p2.getVelocity();
                sf::Vector2f relativeVelocity = v1 - v2;
                float normalVelocity = relativeVelocity.x * normal.x + relativeVelocity.y * normal.y;
                
                // Se as partículas estão se afastando, não há colisão a ser resolvida
                if (normalVelocity > 0) {
                    continue;
                }
                
                // Calcula impulso com base nas massas e coeficiente de restituição
                float m1 = p1.getMass();
                float m2 = p2.getMass();
                float impulse = -(1.0f + restitution) * normalVelocity / (1.0f/m1 + 1.0f/m2);
                
                // Aplica impulso às velocidades
                sf::Vector2f impulseVector = normal * impulse;
                p1.setVelocity(v1 + impulseVector / m1);
                p2.setVelocity(v2 - impulseVector / m2);
                
                // Corrige posições para evitar sobreposição (penetration resolution)
                float penetration = radiusSum - distance;
                float pushRatio1 = m2 / (m1 + m2);
                float pushRatio2 = m1 / (m1 + m2);
                
                p1.setPosition(p1.getPosition() + normal * penetration * pushRatio1);
                p2.setPosition(p2.getPosition() - normal * penetration * pushRatio2);
            }
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
