#include "ParticlePool.h"

ParticlePool::ParticlePool(size_t capacity) : m_capacity(capacity) {
    m_activeParticles.reserve(capacity);
    m_inactiveParticles.reserve(capacity);
    
    // Pré-criar todas as partículas com valores padrão
    // para evitar alocações dinâmicas durante a execução
    for (size_t i = 0; i < capacity; ++i) {
        // Criar partícula com valores temporários (serão redefinidos ao adquirir)
        m_particleStorage.emplace_back(1.0f, sf::Vector2f(0, 0), sf::Vector2f(0, 0), sf::Color::White);
        // Adicionar à lista de partículas inativas
        m_inactiveParticles.push_back(&m_particleStorage.back());
    }
}

ParticlePool::~ParticlePool() {
    // O vetor m_particleStorage gerencia automaticamente a memória das partículas
    // Não é necessário fazer delete nas partículas
}

Particle* ParticlePool::acquireParticle(float mass, const sf::Vector2f& position, 
                                       const sf::Vector2f& velocity, const sf::Color& color) {
    Particle* particle = nullptr;
    
    // Verificar se há partículas inativas disponíveis
    if (!m_inactiveParticles.empty()) {
        // Pegar a última partícula inativa
        particle = m_inactiveParticles.back();
        m_inactiveParticles.pop_back();
        

        particle->setPosition(position);
        particle->setVelocity(velocity);
        particle->setColor(color);
        particle->setMass(mass); // Garantir que a massa também seja atualizada
        
        // Adicionar à lista de partículas ativas
        m_activeParticles.push_back(particle);
        particle->setPoolIndex(m_activeParticles.size() - 1);
    } else {
        size_t currentSize = m_particleStorage.size();
        if (currentSize < m_capacity) {
            m_particleStorage.emplace_back(mass, position, velocity, color);
            particle = &m_particleStorage.back();
            m_activeParticles.push_back(particle);
            particle->setPoolIndex(m_activeParticles.size() - 1);
        } else {
            if (m_capacity < MAX_AUTO_EXPAND_CAPACITY) {
                size_t expansionSize = m_capacity / 2;
                expandCapacity(expansionSize);
                
                particle = m_inactiveParticles.back();
                m_inactiveParticles.pop_back();

                particle->setPosition(position);
                particle->setVelocity(velocity);
                particle->setColor(color);
                particle->setMass(mass);
                m_activeParticles.push_back(particle);
                particle->setPoolIndex(m_activeParticles.size() - 1);
            } else {
                if (!m_activeParticles.empty()) {
                    Particle* oldestParticle = m_activeParticles[0];
                    releaseParticle(oldestParticle);
                    particle = acquireParticle(mass, position, velocity, color);
                }
            }
        }
    }
    
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
    // Mover todas as partículas ativas para a lista de inativas
    m_inactiveParticles.insert(m_inactiveParticles.end(), 
                              m_activeParticles.begin(), 
                              m_activeParticles.end());
    m_activeParticles.clear();
}

void ParticlePool::expandCapacity(size_t additionalCapacity) {
    // Verificar se a expansão está dentro dos limites seguros
    if (m_capacity + additionalCapacity > MAX_AUTO_EXPAND_CAPACITY) {
        additionalCapacity = MAX_AUTO_EXPAND_CAPACITY - m_capacity;
        if (additionalCapacity <= 0) return; // Já estamos no máximo
    }
    
    // Calcular nova capacidade
    size_t oldCapacity = m_capacity;
    m_capacity += additionalCapacity;
    
    // Aumentar a reserva de memória para evitar realocações frequentes
    m_activeParticles.reserve(m_capacity);
    m_inactiveParticles.reserve(m_capacity);
    
    // Pré-alocar as novas partículas
    size_t currentSize = m_particleStorage.size();
    for (size_t i = 0; i < additionalCapacity; ++i) {
        m_particleStorage.emplace_back(1.0f, sf::Vector2f(0, 0), sf::Vector2f(0, 0), sf::Color::White);
        // Adicionar à lista de partículas inativas
        m_inactiveParticles.push_back(&m_particleStorage.back());
    }
}
