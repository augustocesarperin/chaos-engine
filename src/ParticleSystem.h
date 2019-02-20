#pragma once
#include "Particle.h"
#include "ParticlePool.h"
#include "SpatialGrid.h"
#include <vector>
#include <memory>
#include <SFML/Graphics.hpp>
#include <map>

class ParticleSystem {
public:
    struct PhysicsInputState {
        bool gravityEnabled;
        float gravitationalAcceleration;
        bool repulsionEnabled;
        float repulsionStrength;
        bool collisionsEnabled;
        float collisionRestitution;
        bool mouseForceEnabled;
        sf::Vector2f mousePosition;
        float mouseForceStrength;
        bool mouseForceAttractMode;
        int forceMode;
    };

    ParticleSystem(float width, float height);
    ~ParticleSystem();
    
    Particle* addParticle(float mass, const sf::Vector2f& position, const sf::Vector2f& velocity, const sf::Color& color);
    void removeParticle(Particle* particle);
    void removeParticle(size_t index);
    void update(float deltaTime, const PhysicsInputState& inputs);
    
    void draw(sf::RenderWindow& window);
    
    void generateRandomParticles(int count, float minMass = 1.0f, float maxMass = 5.0f);
    Particle* generateRandomParticle(float minMass, float maxMass);
    
    void setWindowSize(float width, float height);
    void handleCollisions(float restitution, float dt);
    
    size_t getParticleCount() const { return m_particlePool.getActiveCount(); }

private:
    void applyInteractiveForces(float repulsionStrength);
    void applyGravityEffect(float gravitationalAcceleration);
    void applyMouseForce(const sf::Vector2f& mousePosition, float strength, bool attractMode, int forceMode);
    void updateHeadVertices();

    void syncToSoA();
    void syncFromSoA(float dt);

    void updateTrailVertices();

    ParticlePool m_particlePool;
    std::unique_ptr<SpatialGrid> m_grid;
    float m_width;
    float m_height;
    
    static constexpr size_t INITIAL_POOL_CAPACITY = 1000;
    static constexpr float GRID_CELL_SIZE = 60.0f;
    static constexpr float MOUSE_FORCE_STEP = 10000.0f;

    sf::VertexArray m_trailVertices;
    sf::VertexArray m_untexturedHeadVertices;
    std::map<std::shared_ptr<sf::Texture>, sf::VertexArray> m_texturedHeadBatches;

    std::vector<float> m_soa_positions;
    std::vector<float> m_soa_velocities;
    std::vector<float> m_soa_accelerations;
    std::vector<float> m_soa_masses;
    std::vector<float> m_soa_radii;
    std::vector<float> m_soa_previous_positions;
};

#ifdef __cplusplus
extern "C" {
#endif

void update_particles_c(
    float* positions,
    float* previous_positions,
    float* velocities,
    float* accelerations,
    float* masses,
    float* radii,
    int particle_count,
    float dt,
    float world_width,
    float world_height,
    float restitution
);

#ifdef __cplusplus
}
#endif
