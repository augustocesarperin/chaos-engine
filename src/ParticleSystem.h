#pragma once
#include "Particle.h"
#include "ParticlePool.h"
#include <vector>
#include <memory>

class ParticleSystem {
public:
    ParticleSystem(float width, float height);
    ~ParticleSystem();
    
    Particle* addParticle(float mass, const sf::Vector2f& position, const sf::Vector2f& velocity, const sf::Color& color);
    void removeParticle(Particle* particle);
    void removeParticle(size_t index);
    void update(float deltaTime);
    
    void applyGravityEffect(float gravitationalAcceleration);    
    void draw(sf::RenderWindow& window);
    
    void applyInteractiveForces(float strength);
    void applyMouseForce(const sf::Vector2f& mousePosition, float strength, bool attractMode);
    
    void generateRandomParticles(int count, float minMass, float maxMass);
    Particle* generateRandomParticle(float minMass, float maxMass);
    
    // Atualiza as dimensões da área de simulação quando a janela é alterada
    void setWindowSize(float width, float height);
    
    void handleCollisions(float restitution = 0.8f);
    
    size_t getParticleCount() const { return m_particlePool.getActiveCount(); }

private:
    ParticlePool m_particlePool;
    float m_width;
    float m_height;
    
    static constexpr size_t INITIAL_POOL_CAPACITY = 1000;
};
