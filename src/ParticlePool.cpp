#include "ParticlePool.h"

ParticlePool::ParticlePool(size_t capacity) : m_capacity(capacity) {
    m_activeParticles.reserve(capacity);
    m_inactiveParticles.reserve(capacity);
    
    for (size_t i = 0; i < capacity; ++i) {
        m_particleStorage.emplace_back();
        m_inactiveParticles.push_back(&m_particleStorage.back());
    }
}

ParticlePool::~ParticlePool() {
}

Particle* ParticlePool::acquireParticle(float mass, const sf::Vector2f& position, 
                                       const sf::Vector2f& velocity, const sf::Color& color) {
    if (m_inactiveParticles.empty()) {
        if (m_capacity < MAX_AUTO_EXPAND_CAPACITY) {
            size_t expansionSize = m_capacity > 0 ? (m_capacity / 2) : 16;
            expandCapacity(expansionSize);
        } else {
            // Se não puder expandir, recicla a partícula mais antiga
            if (!m_activeParticles.empty()) {
                Particle* oldestParticle = m_activeParticles.front();
                releaseParticle(oldestParticle);
            }
        }
    }
    
    if (m_inactiveParticles.empty()) {
        return nullptr; 
    }

    Particle* particle = m_inactiveParticles.back();
    m_inactiveParticles.pop_back();
    
    particle->initialize(mass, position, velocity, color);
    
    m_activeParticles.push_back(particle);
    particle->setPoolIndex(m_activeParticles.size() - 1);
    
    return particle;
}

void ParticlePool::releaseParticle(Particle* particle) {
    if (!particle || m_activeParticles.empty()) {
        return;
    }

    size_t indexToRemove = particle->getPoolIndex();

    if (indexToRemove >= m_activeParticles.size() || m_activeParticles[indexToRemove] != particle) {
        return;
    }

    Particle* lastParticle = m_activeParticles.back();
    m_activeParticles[indexToRemove] = lastParticle;

    lastParticle->setPoolIndex(indexToRemove);

    m_activeParticles.pop_back();

    m_inactiveParticles.push_back(particle);
}

void ParticlePool::clearAll() {
    m_inactiveParticles.insert(m_inactiveParticles.end(), 
                              m_activeParticles.begin(), 
                              m_activeParticles.end());
    m_activeParticles.clear();
}

void ParticlePool::expandCapacity(size_t additionalCapacity) {
    if (m_capacity + additionalCapacity > MAX_AUTO_EXPAND_CAPACITY) {
        additionalCapacity = MAX_AUTO_EXPAND_CAPACITY - m_capacity;
        if (additionalCapacity <= 0) return; 
    }
    
    m_capacity += additionalCapacity;
    m_activeParticles.reserve(m_capacity);
    m_inactiveParticles.reserve(m_capacity);
    
    for (size_t i = 0; i < additionalCapacity; ++i) {
        m_particleStorage.emplace_back();
        m_inactiveParticles.push_back(&m_particleStorage.back());
    }
}
