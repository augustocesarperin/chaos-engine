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
    
    // Pré-alocação de memória
    std::deque<Particle> m_particleStorage;

    // Limite de segurança 
    static constexpr size_t MAX_AUTO_EXPAND_CAPACITY = 10000;

public:
    ParticlePool(size_t initialCapacity = 1000);
    ~ParticlePool();
    
    // Desativa uma reutilização do pool (não alocar/desalocar)
    Particle* acquireParticle(float mass, const sf::Vector2f& position, 
                             const sf::Vector2f& velocity, const sf::Color& color);
    
    // Devolve uma partícula para o pool para reutilização futura
    void releaseParticle(Particle* particle);
    
    // Limpa todas as partículas e devolve para o pool
    void clearAll();
    
    void expandCapacity(size_t additionalCapacity);
    
    // Getters
    size_t getActiveCount() const { return m_activeParticles.size(); }
    size_t getInactiveCount() const { return m_inactiveParticles.size(); }
    size_t getTotalCapacity() const { return m_capacity; }
    
    // Retorna um ponteiro para um vetor de partículas ativas
    const std::vector<Particle*>& getActiveParticles() const { return m_activeParticles; }
};
