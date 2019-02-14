#pragma once
#include "Particle.h"
#include <vector>
#include <memory>
#include <deque>

class ParticlePool {
private:
    std::vector<Particle*> m_activeParticles;
    std::vector<Particle*> m_inactiveParticles;
    size_t m_capacity;
    
    std::deque<Particle> m_particleStorage;

    static constexpr size_t MAX_AUTO_EXPAND_CAPACITY = 10000;

public:
    ParticlePool(size_t initialCapacity = 1000);
    ~ParticlePool();
    
    Particle* acquireParticle(float mass, const sf::Vector2f& position, 
                             const sf::Vector2f& velocity, const sf::Color& color);
    
    void releaseParticle(Particle* particle);
    void clearAll();
    void expandCapacity(size_t additionalCapacity);
    
    size_t getActiveCount() const { return m_activeParticles.size(); }
    size_t getInactiveCount() const { return m_inactiveParticles.size(); }
    size_t getTotalCapacity() const { return m_capacity; }

    const std::vector<Particle*>& getActiveParticles() const { return m_activeParticles; }
};
