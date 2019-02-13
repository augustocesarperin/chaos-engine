#include "ParticleSystem.h"
#include <iostream>
#include <random>
#include <cmath>

ParticleSystem::ParticleSystem(float width, float height)
    : m_particlePool(INITIAL_POOL_CAPACITY), m_width(width), m_height(height) {
}

ParticleSystem::~ParticleSystem() {
}

void ParticleSystem::setWindowSize(float width, float height) {
    m_width = width;
    m_height = height;
}

Particle* ParticleSystem::addParticle(float mass, const sf::Vector2f& position, const sf::Vector2f& velocity, const sf::Color& color) {
    Particle* particle = m_particlePool.acquireParticle(mass, position, velocity, color);
    
    if (!particle) {
        // liberar as 10% mais antigas partículas
        const auto& activeParticles = m_particlePool.getActiveParticles();
        size_t numToRelease = activeParticles.size() / 10;
        
        if (numToRelease > 0) {
            for (size_t i = 0; i < numToRelease && i < activeParticles.size(); ++i) {
                m_particlePool.releaseParticle(activeParticles[i]);
            }
            particle = m_particlePool.acquireParticle(mass, position, velocity, color);
        }
    }
    
    return particle; 
}

void ParticleSystem::removeParticle(Particle* particle) {
    if (particle) {
        m_particlePool.releaseParticle(particle);
    }
}

void ParticleSystem::removeParticle(size_t index) {
    const auto& activeParticles = m_particlePool.getActiveParticles();
    if (index < activeParticles.size()) {
        m_particlePool.releaseParticle(activeParticles[index]);
    }
}

void ParticleSystem::update(float deltaTime) {
    // Constante para resistência do ar
    const float BASE_AIR_RESISTANCE = 0.002f;
    
    // Cache do número de partículas para evitar chamadas repetidas
    const auto& activeParticles = m_particlePool.getActiveParticles();
    const size_t numParticles = activeParticles.size();
    
    for (size_t i = 0; i < numParticles; ++i) {
        Particle* particle = activeParticles[i];
        
        // Calcular resistência do ar usando a massa da partícula
        const float mass = particle->getMass();
        const float airResistance = BASE_AIR_RESISTANCE / mass;
        
        // Aplicar resistência, atualizar posição e manter dentro dos limites
        particle->applyDrag(airResistance);
        particle->update(deltaTime);
        particle->keepInBounds(m_width, m_height);
    }
}

void ParticleSystem::applyGravityEffect(float gravitationalAcceleration) {
    const float MIN_VALID_MASS = 0.0001f;
    
    // Cache do número de partículas
    const auto& activeParticles = m_particlePool.getActiveParticles();
    const size_t numParticles = activeParticles.size();
    
    for (size_t i = 0; i < numParticles; ++i) {
        Particle* particle = activeParticles[i];
        const float mass = particle->getMass();
        
        if (mass > MIN_VALID_MASS) {
            // Pre-calcular a força da gravidade baseada na massa
            const sf::Vector2f gravityForce(0.0f, mass * gravitationalAcceleration);
            particle->applyForce(gravityForce);
        }
    }
}

void ParticleSystem::draw(sf::RenderWindow& window) {
    // Obter o tamanho da janela para determinar a área visível
    const sf::Vector2u windowSize = window.getSize();
    const sf::View& view = window.getView();
    
    // Calcular os limites da área visível com uma margem para partículas grandes
    const float MARGIN = 50.0f; 
    
    const sf::FloatRect visibleArea(
        view.getCenter().x - view.getSize().x / 2.0f - MARGIN,
        view.getCenter().y - view.getSize().y / 2.0f - MARGIN,
        view.getSize().x + MARGIN * 2.0f,
        view.getSize().y + MARGIN * 2.0f
    );
    
    const auto& activeParticles = m_particlePool.getActiveParticles();
    
    int particlesDrawn = 0;
    for (const auto& particle : activeParticles) {
        if (!particle) {
            continue;
        }

        particle->renderTo(window, sf::RenderStates::Default);
        particlesDrawn++;
    }
}

void ParticleSystem::applyInteractiveForces(float strength) {
    // Limitar a força máxima para evitar que as partículas saiam voando
    const float MAX_FORCE = 5000.0f;
    const float MIN_DISTANCE = 5.0f;
    const float MIN_DISTANCE_SQ = MIN_DISTANCE * MIN_DISTANCE;
    
    // evitar chamadas repetidas
    const auto& activeParticles = m_particlePool.getActiveParticles();
    const size_t numParticles = activeParticles.size();
    
    for (size_t i = 0; i < numParticles; ++i) {
        Particle* p1 = activeParticles[i];
        const float mass1 = p1->getMass();
        const sf::Vector2f& pos1 = p1->getPosition();
        
        for (size_t j = i + 1; j < numParticles; ++j) {
            Particle* p2 = activeParticles[j];
            const float mass2 = p2->getMass();
            const sf::Vector2f& pos2 = p2->getPosition();
            
            const sf::Vector2f delta = pos1 - pos2;
            
            const float distanceSquared = delta.x * delta.x + delta.y * delta.y;
            
            // Evitar distâncias muito pequenas e divisões por zero
            if (distanceSquared < 0.0001f) continue;

            const float distance = std::sqrt(distanceSquared);
            
            // Aumentar distância mínima para evitar forças extremas
            const float effectiveDistance = (distance < MIN_DISTANCE) ? MIN_DISTANCE : distance;
            const float effectiveDistanceSquared = effectiveDistance * effectiveDistance;
            
            // Calcular a força com base na distância
            // Cache para o produto das massas
            const float massProduct = mass1 * mass2;
            float forceMagnitude = strength * massProduct / effectiveDistanceSquared;
            
            forceMagnitude = std::min(forceMagnitude, MAX_FORCE);
            
            // Direção da força (normalizada)
            const sf::Vector2f forceDirection = sf::Vector2f(delta.x / distance, delta.y / distance);
            const sf::Vector2f force = forceDirection * forceMagnitude;
        
            p1->applyForce(force);
            p2->applyForce(-force);
        }
    }
}

void ParticleSystem::handleCollisions(float restitution) {
    const float MIN_DISTANCE = 0.01f;
    
    const auto& activeParticles = m_particlePool.getActiveParticles();
    const size_t numParticles = activeParticles.size();
    
    // Verifica todas as combinações possíveis de partículas
    for (size_t i = 0; i < numParticles; ++i) {
        // Usar referências para melhorar eficiência de acesso
        Particle* p1 = activeParticles[i];
        const float radius1 = p1->getRadius();
        const float mass1 = p1->getMass();
        const float inverseMass1 = (mass1 > 0.0001f) ? 1.0f / mass1 : 0.0f;
        
        for (size_t j = i + 1; j < numParticles; ++j) {
            Particle* p2 = activeParticles[j];
            const float radius2 = p2->getRadius();
            const float mass2 = p2->getMass();
            const float inverseMass2 = (mass2 > 0.0001f) ? 1.0f / mass2 : 0.0f;
            
            // Cache da soma dos raios
            const float radiusSum = radius1 + radius2;
            const float radiusSumSquared = radiusSum * radiusSum;
            
            // Calcula a distância entre os centros das partículas
            const sf::Vector2f pos1 = p1->getPosition();
            const sf::Vector2f pos2 = p2->getPosition();
            const sf::Vector2f delta = pos1 - pos2;
            
            const float distanceSquared = delta.x * delta.x + delta.y * delta.y;
            
            // Verifica se as partículas estão colidindo (distância menor que a soma dos raios)
            if (distanceSquared < radiusSumSquared) {
                // Só precisamos calcular a raíz quadrada quando há uma possível colisão
                const float distance = std::max(std::sqrt(distanceSquared), MIN_DISTANCE);
                
                // Direção normal da colisão 
                const sf::Vector2f normal = delta / distance;
                
                // Calcula a velocidade relativa na direção normal
                const sf::Vector2f v1 = p1->getVelocity();
                const sf::Vector2f v2 = p2->getVelocity();
                const sf::Vector2f relativeVelocity = v1 - v2;
                
                // Produto escalar entre velocidade relativa e normal 
                const float normalVelocity = relativeVelocity.x * normal.x + relativeVelocity.y * normal.y;
                
                // Se as partículas estão se afastando, não há colisão a ser resolvida
                if (normalVelocity > 0) {
                    continue;
                }
                
                float adjustedRestitution = restitution;
                
                // Se muitas partículas estiverem próximas (distância pequena),
                // reduz restituição 
                const float distanceRatio = distance / radiusSum;
                if (distanceRatio < 0.7f) {
                    adjustedRestitution *= distanceRatio * 0.8f;
                }
                
                const float restitutionFactor = 1.0f + adjustedRestitution;
                
                const float impulse = -restitutionFactor * normalVelocity / (inverseMass1 + inverseMass2);
                const sf::Vector2f impulseVector = normal * impulse;
                
                sf::Vector2f newV1 = v1 + impulseVector * inverseMass1;
                sf::Vector2f newV2 = v2 - impulseVector * inverseMass2;
                
                // Limitar velocidade máxima após colisão quando há muitas partículas próximas
                const float maxVelocity = 400.0f;
                
                float v1Mag = std::sqrt(newV1.x*newV1.x + newV1.y*newV1.y);
                float v2Mag = std::sqrt(newV2.x*newV2.x + newV2.y*newV2.y);
                
                if (v1Mag > maxVelocity) {
                    newV1.x = (newV1.x / v1Mag) * maxVelocity;
                    newV1.y = (newV1.y / v1Mag) * maxVelocity;
                }
                
                if (v2Mag > maxVelocity) {
                    newV2.x = (newV2.x / v2Mag) * maxVelocity;
                    newV2.y = (newV2.y / v2Mag) * maxVelocity;
                }
                
                p1->setVelocity(newV1);
                p2->setVelocity(newV2);
                
                // Corrige posições para evitar sobrepos
                const float penetration = radiusSum - distance;
                
                // Cache 
                const float totalMass = mass1 + mass2;
                const float pushRatio1 = mass2 / totalMass;
                const float pushRatio2 = mass1 / totalMass;
                
                p1->setPosition(pos1 + normal * penetration * pushRatio1);
                p2->setPosition(pos2 - normal * penetration * pushRatio2);
            }
        }
    }
}

void ParticleSystem::applyMouseForce(const sf::Vector2f& mousePosition, float strength, bool attractMode) {
    const float MIN_DISTANCE_SQ = 1.0f;
    
    const auto& activeParticles = m_particlePool.getActiveParticles();
    const size_t numParticles = activeParticles.size();
    
    const float directionMultiplier = attractMode ? 1.0f : -1.0f;
    
    for (size_t i = 0; i < numParticles; ++i) {
        Particle* particle = activeParticles[i];
        const sf::Vector2f& particlePosition = particle->getPosition();
        
        const sf::Vector2f direction = mousePosition - particlePosition;
        const float distanceSquared = direction.x * direction.x + direction.y * direction.y;

        // Evitar distâncias muito pequenas e div por 0
        if (distanceSquared < MIN_DISTANCE_SQ) { 
            continue; 
        }

        // Calcular força apenas quando realmente necessário
        const float distance = std::sqrt(distanceSquared);  
        const float forceMagnitude = strength / distance;
        
        const sf::Vector2f normalizedDirection = direction / distance;
        const sf::Vector2f force = normalizedDirection * forceMagnitude * directionMultiplier;
        
        particle->applyForce(force);
    }
}

void ParticleSystem::generateRandomParticles(int count, float minMass, float maxMass) {
    for (int i = 0; i < count; ++i) {
        generateRandomParticle(minMass, maxMass);
    }
}

Particle* ParticleSystem::generateRandomParticle(float minMass, float maxMass) {
    std::random_device rd;
    std::mt19937 gen(rd());
    float mass = std::uniform_real_distribution<float>(minMass, maxMass)(gen);
    
    const float r = std::uniform_real_distribution<float>(0.0f, 255.0f)(gen);
    const float g = std::uniform_real_distribution<float>(0.0f, 255.0f)(gen);
    const float b = std::uniform_real_distribution<float>(0.0f, 255.0f)(gen);
    
    sf::Color color(static_cast<sf::Uint8>(r), static_cast<sf::Uint8>(g), static_cast<sf::Uint8>(b));
    
    // Gerar posição aleatória dentro da área de simulação
    float x = std::uniform_real_distribution<float>(0.0f, m_width)(gen);
    float y = std::uniform_real_distribution<float>(0.0f, m_height)(gen);
    
    // Gerar velocidade inicial aleatória
    float vx = std::uniform_real_distribution<float>(-50.0f, 50.0f)(gen);
    float vy = std::uniform_real_distribution<float>(-50.0f, 50.0f)(gen);
    
    return addParticle(mass, sf::Vector2f(x, y), sf::Vector2f(vx, vy), color);
}
