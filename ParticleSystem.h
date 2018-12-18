#pragma once
#include "Particle.h"
#include <vector>
#include <memory>

class ParticleSystem {
public:
    ParticleSystem(float width, float height);
    
    void addParticle(float mass, const sf::Vector2f& position, const sf::Vector2f& velocity, const sf::Color& color);
    void removeParticle(size_t index);
    void update(float deltaTime);
    
    void applyGravityEffect(float gravitationalAcceleration);    
    void draw(sf::RenderWindow& window);
    
    // (+ repulsão, - atração)
    void applyInteractiveForces(float strength);
    
    void generateRandomParticles(int count, float minMass, float maxMass);
    
    size_t getParticleCount() const { return m_particles.size(); }

private:
    std::vector<Particle> m_particles;
    float m_width;
    float m_height;
};
