#include "ParticleSystem.h"
#include <iostream>
#include <random>
#include <cmath>

ParticleSystem::ParticleSystem(float width, float height)
    : m_particlePool(INITIAL_POOL_CAPACITY), m_width(width), m_height(height) {
    m_grid = std::make_unique<SpatialGrid>(width, height, GRID_CELL_SIZE);
    m_trailVertices.setPrimitiveType(sf::TriangleStrip);
    m_untexturedHeadVertices.setPrimitiveType(sf::Triangles);
}

ParticleSystem::~ParticleSystem() {
}

void ParticleSystem::setWindowSize(float width, float height) {
    m_width = width;
    m_height = height;
    m_grid = std::make_unique<SpatialGrid>(width, height, GRID_CELL_SIZE);
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

void ParticleSystem::update(float deltaTime, const PhysicsInputState& inputs) {
    syncToSoA();

    if (inputs.gravityEnabled) {
        applyGravityEffect(inputs.gravitationalAcceleration);
    }
    if (inputs.repulsionEnabled) {
        applyInteractiveForces(inputs.repulsionStrength);
    }
    if (inputs.mouseForceEnabled) {
        applyMouseForce(inputs.mousePosition, inputs.mouseForceStrength, inputs.mouseForceAttractMode, inputs.forceMode);
    }

    if (m_soa_previous_positions.size() != m_soa_positions.size()) {
        m_soa_previous_positions.resize(m_soa_positions.size());
        for (size_t i = 0; i < m_particlePool.getActiveCount(); ++i) {
            m_soa_previous_positions[i * 2]     = m_soa_positions[i * 2] - m_soa_velocities[i * 2] * deltaTime;
            m_soa_previous_positions[i * 2 + 1] = m_soa_positions[i * 2 + 1] - m_soa_velocities[i * 2 + 1] * deltaTime;
        }
    }

    // Chamar a função C otimizada
    update_particles_c(
        m_soa_positions.data(),
        m_soa_previous_positions.data(),
        m_soa_velocities.data(),
        m_soa_accelerations.data(),
        m_soa_masses.data(),
        m_soa_radii.data(),
        m_particlePool.getActiveCount(),
        deltaTime,
        m_width,
        m_height,
        inputs.collisionRestitution
    );

    syncFromSoA(deltaTime);

    if (inputs.collisionsEnabled) {
        handleCollisions(inputs.collisionRestitution, deltaTime);
    }
    
    updateTrailVertices();
    updateHeadVertices();
}

void ParticleSystem::syncToSoA() {
    const auto& activeParticles = m_particlePool.getActiveParticles();
    const size_t numParticles = activeParticles.size();
    
    m_soa_positions.resize(numParticles * 2);
    m_soa_velocities.resize(numParticles * 2);
    m_soa_accelerations.resize(numParticles * 2);
    m_soa_masses.resize(numParticles);
    m_soa_radii.resize(numParticles);
    
    for (size_t i = 0; i < numParticles; ++i) {
        Particle* p = activeParticles[i];
        p->setSoAIndex(i); 

        const sf::Vector2f pos = p->getPosition();
        const sf::Vector2f vel = p->getVelocity();        
        m_soa_positions[i * 2]     = pos.x;
        m_soa_positions[i * 2 + 1] = pos.y;
        
        m_soa_velocities[i * 2]     = vel.x;
        m_soa_velocities[i * 2 + 1] = vel.y;

        m_soa_masses[i] = p->getMass();
        m_soa_radii[i] = p->getRadius();
    }

    std::fill(m_soa_accelerations.begin(), m_soa_accelerations.end(), 0.0f);
}

void ParticleSystem::syncFromSoA(float dt) {
    const auto& activeParticles = m_particlePool.getActiveParticles();
    const size_t numParticles = activeParticles.size();


    for (size_t i = 0; i < numParticles; ++i) {
        Particle* p = activeParticles[i];
        p->setPosition({m_soa_positions[i * 2], m_soa_positions[i * 2 + 1]});
        p->setVelocity({m_soa_velocities[i * 2], m_soa_velocities[i * 2 + 1]});
        p->updateVisuals(dt);
    }
}

void ParticleSystem::applyGravityEffect(float gravitationalAcceleration) {
    const float MIN_VALID_MASS = 0.0001f;
    const size_t numParticles = m_particlePool.getActiveCount();
    
    for (size_t i = 0; i < numParticles; ++i) {
        const float mass = m_soa_masses[i];
        if (mass > MIN_VALID_MASS) {
            m_soa_accelerations[i * 2 + 1] += gravitationalAcceleration;
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
    
    window.draw(m_trailVertices, sf::BlendAdd);

    window.draw(m_untexturedHeadVertices);
    for (const auto& pair : m_texturedHeadBatches) {
        sf::RenderStates states(pair.first.get());
        window.draw(pair.second, states);
    }
}

void ParticleSystem::applyInteractiveForces(float strength) {
    m_grid->clear();
    const auto& activeParticles = m_particlePool.getActiveParticles();
    for (Particle* p : activeParticles) {
        m_grid->insert(p);
    }

    const float MAX_FORCE = 5000.0f;
    const float MIN_DISTANCE = 5.0f;
    
    for (Particle* p1 : activeParticles) {
        std::vector<Particle*> nearby = m_grid->getNearbyParticles(p1);
        
        for (Particle* p2 : nearby) {
            if (p1->getSoAIndex() >= p2->getSoAIndex()) continue;

            const float mass1 = p1->getMass();
            const sf::Vector2f p1_pos = p1->getPosition();
            
            const float mass2 = p2->getMass();
            const sf::Vector2f p2_pos = p2->getPosition();
            
            const float dx = p1_pos.x - p2_pos.x;
            const float dy = p1_pos.y - p2_pos.y;
            
            const float distSq = dx * dx + dy * dy;

            if (distSq > 0.0001f) {
                const float dist = sqrt(distSq);
                const float effectiveDist = (dist < MIN_DISTANCE) ? MIN_DISTANCE : dist;
            const float massProduct = mass1 * mass2;
            
                float forceMagnitude = strength * massProduct / (effectiveDist * effectiveDist);
            forceMagnitude = std::min(forceMagnitude, MAX_FORCE);
            
                const float fx = (dx / dist) * forceMagnitude;
                const float fy = (dy / dist) * forceMagnitude;

                size_t index1 = p1->getSoAIndex();
                size_t index2 = p2->getSoAIndex();

                if (mass1 > 0.0001f) {
                    m_soa_accelerations[index1 * 2]     += fx;
                    m_soa_accelerations[index1 * 2 + 1] += fy;
                }
                if (mass2 > 0.0001f) {
                    m_soa_accelerations[index2 * 2]     -= fx;
                    m_soa_accelerations[index2 * 2 + 1] -= fy;
                }
            }
        }
    }
}

void ParticleSystem::handleCollisions(float restitution, float deltaTime) {
    m_grid->clear();
    const auto& activeParticles = m_particlePool.getActiveParticles();
    for (Particle* p : activeParticles) {
        m_grid->insert(p);
    }
    
    for (Particle* p1 : activeParticles) {
        std::vector<Particle*> nearby = m_grid->getNearbyParticles(p1);
        
        for (Particle* p2 : nearby) {
            if (p1->getSoAIndex() >= p2->getSoAIndex()) continue;

            const float r1 = p1->getRadius();
            const float m1 = p1->getMass();
            const float invM1 = (m1 > 0.0001f) ? 1.0f / m1 : 0.0f;
            
            const float r2 = p2->getRadius();
            const float m2 = p2->getMass();
            const float invM2 = (m2 > 0.0001f) ? 1.0f / m2 : 0.0f;
            
            const float radiusSum = r1 + r2;
            const sf::Vector2f deltaPos = p1->getPosition() - p2->getPosition();
            const float distSq = deltaPos.x * deltaPos.x + deltaPos.y * deltaPos.y;

            if (distSq < radiusSum * radiusSum) {
                const float distance = std::sqrt(distSq);
                const sf::Vector2f normal = (distance > 0.0001f) ? deltaPos / distance : sf::Vector2f(1, 0);

                const sf::Vector2f relativeVelocity = p1->getVelocity() - p2->getVelocity();
                const float velAlongNormal = relativeVelocity.x * normal.x + relativeVelocity.y * normal.y;

                if (velAlongNormal > 0) continue;

                const float e = restitution;
                float j = -(1.0f + e) * velAlongNormal;
                j /= (invM1 + invM2);
                const sf::Vector2f impulse = j * normal;
                p1->setVelocity(p1->getVelocity() + impulse * invM1);
                p2->setVelocity(p2->getVelocity() - impulse * invM2);

                const sf::Vector2f tangent = {-normal.y, normal.x};
                const float friction = 0.9f; 
                const float vt = relativeVelocity.x * tangent.x + relativeVelocity.y * tangent.y;
                sf::Vector2f tangent_impulse = tangent * (vt * friction);
                tangent_impulse /= (invM1 + invM2);
                p1->setVelocity(p1->getVelocity() - tangent_impulse * invM1);
                p2->setVelocity(p2->getVelocity() + tangent_impulse * invM2);

                const float percent = 0.5f; 
                const float slop = 0.01f; 
                const float penetration = std::max(radiusSum - distance - slop, 0.0f);
                const sf::Vector2f correction = normal * (penetration / (invM1 + invM2)) * percent;
                
                p1->setPosition(p1->getPosition() + correction * invM1);
                p2->setPosition(p2->getPosition() - correction * invM2);

                size_t index1 = p1->getSoAIndex();
                m_soa_previous_positions[index1 * 2] = p1->getPosition().x - p1->getVelocity().x * deltaTime;
                m_soa_previous_positions[index1 * 2 + 1] = p1->getPosition().y - p1->getVelocity().y * deltaTime;
                
                size_t index2 = p2->getSoAIndex();
                m_soa_previous_positions[index2 * 2] = p2->getPosition().x - p2->getVelocity().x * deltaTime;
                m_soa_previous_positions[index2 * 2 + 1] = p2->getPosition().y - p2->getVelocity().y * deltaTime;
            }
        }
    }
}

void ParticleSystem::applyMouseForce(const sf::Vector2f& mousePosition, float strength, bool attractMode, int forceMode) {
    const float influenceRadius = 800.0f; 
    const float minMass = 1.0f; 

    static float pulseTime = 0.0f;
    pulseTime += 0.05f; 

    for (size_t i = 0; i < m_particlePool.getActiveCount(); ++i) {
        float px = m_soa_positions[i * 2];
        float py = m_soa_positions[i * 2 + 1];
        float mass = m_soa_masses[i];
        
        sf::Vector2f direction = mousePosition - sf::Vector2f(px, py);
        float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);

        if (distance < influenceRadius && distance > 0.01f) {
            
            float normalizedDistance = std::min(1.0f, distance / influenceRadius);
            float falloff = std::pow(1.0f - normalizedDistance, 2.0f);
            
            float forceMagnitude = strength * falloff / std::max(mass, minMass);

            if (!attractMode) {
                forceMagnitude *= -1;
            }

            sf::Vector2f force = {0.0f, 0.0f};

            switch (forceMode) {
                case 0: { // Padrão
                    force = (direction / distance) * forceMagnitude;
                    break;
                }
                case 1: { // Redemoinho
                    const float orbitRadius = 60.0f; // Raio da órbita estável

                    sf::Vector2f radialForce;
                    sf::Vector2f tangentForce;

                    if (distance > orbitRadius) {
                        radialForce = (direction / distance) * forceMagnitude * 2.0f;
                        sf::Vector2f tangent = {-direction.y, direction.x};
                        tangentForce = (tangent / distance) * forceMagnitude * 0.5f;
                    } else {
                        float pushStrength = forceMagnitude * (1.0f - distance / orbitRadius);
                        radialForce = -(direction / distance) * pushStrength * 0.5f;

                        // O giro é máximo para manter a órbita rápida e apertada.
                        sf::Vector2f tangent = {-direction.y, direction.x};
                        tangentForce = (tangent / distance) * forceMagnitude * 2.0f;
                    }

                    force = radialForce + tangentForce;
                    break;
                }
                case 2: { // Onda de Pulso
                    float pulse = sin(pulseTime - distance * 0.05f);
                    force = (direction / distance) * forceMagnitude * pulse;
                    break;
                }
                case 3: { // Linha de Força
                    float dx = mousePosition.x - px;
                    float lineFalloff = std::max(0.0f, 1.0f - std::abs(dx) / influenceRadius);
                    float lineForce = strength * lineFalloff / std::max(mass, minMass);
                    if (dx < 0) lineForce *= -1;
                    if (!attractMode) lineForce *= -1;
                    force = {lineForce, 0.0f};
                    break;
                }
            }
            m_soa_accelerations[i * 2]     += force.x;
            m_soa_accelerations[i * 2 + 1] += force.y;
        }
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

void ParticleSystem::updateTrailVertices() {
    m_trailVertices.clear();
    const auto& activeParticles = m_particlePool.getActiveParticles();

    for (const auto* particle : activeParticles) {
        auto trail = particle->getTrailData();
        if (trail.size < 2) {
            continue;
        }

        for (int i = 0; i < trail.size; ++i) {
            int current_idx = (trail.head - trail.size + i + Particle::MAX_TRAIL_LENGTH) % Particle::MAX_TRAIL_LENGTH;
            
            sf::Vector2f p = trail.buffer[current_idx].position;
            sf::Vector2f direction;

            if (i < trail.size - 1) {
                int next_idx = (trail.head - trail.size + i + 1 + Particle::MAX_TRAIL_LENGTH) % Particle::MAX_TRAIL_LENGTH;
                direction = trail.buffer[next_idx].position - p;
            } else {
                int prev_idx = (trail.head - trail.size + i - 1 + Particle::MAX_TRAIL_LENGTH) % Particle::MAX_TRAIL_LENGTH;
                direction = p - trail.buffer[prev_idx].position;
            }

            float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
            if (length < 0.1f) length = 0.1f;

            sf::Vector2f unitPerpendicular(-direction.y / length, direction.x / length);

            float thickness_ratio = static_cast<float>(i) / (trail.size - 1);
            float ease_out_ratio = thickness_ratio * (2.0f - thickness_ratio);
            float thickness = particle->getRadius() * 0.7f * ease_out_ratio;
            
            sf::Vector2f offset = unitPerpendicular * thickness;
            
            sf::Color color = trail.buffer[current_idx].color;

            m_trailVertices.append(sf::Vertex(p - offset, color));
            m_trailVertices.append(sf::Vertex(p + offset, color));
        }

        if (m_trailVertices.getVertexCount() > 0) {
            m_trailVertices.append(m_trailVertices[m_trailVertices.getVertexCount() - 1]);
        }
    }
}

void ParticleSystem::updateHeadVertices() {
    m_untexturedHeadVertices.clear();
    for (auto& pair : m_texturedHeadBatches) {
        pair.second.clear();
    }

    const auto& activeParticles = m_particlePool.getActiveParticles();
    for (const auto* p : activeParticles) {
        sf::Vector2f pos = p->getPosition();
        float radius = p->getRadius();
        sf::Color color = p->getColor();

        sf::Vector2f topLeft(pos.x - radius, pos.y - radius);
        sf::Vector2f topRight(pos.x + radius, pos.y - radius);
        sf::Vector2f bottomRight(pos.x + radius, pos.y + radius);
        sf::Vector2f bottomLeft(pos.x - radius, pos.y + radius);

        auto texture = p->getTexture();
        if (texture) {
            if (m_texturedHeadBatches.find(texture) == m_texturedHeadBatches.end()) {
                m_texturedHeadBatches[texture].setPrimitiveType(sf::Quads);
            }
            sf::VertexArray& batch = m_texturedHeadBatches[texture];
            sf::Vector2u texSize = texture->getSize();
            batch.append(sf::Vertex(topLeft, color, {0.f, 0.f}));
            batch.append(sf::Vertex(topRight, color, {static_cast<float>(texSize.x), 0.f}));
            batch.append(sf::Vertex(bottomRight, color, {static_cast<float>(texSize.x), static_cast<float>(texSize.y)}));
            batch.append(sf::Vertex(bottomLeft, color, {0.f, static_cast<float>(texSize.y)}));
        } else {
            const int pointCount = 12;
            const float angleIncrement = (2.0f * 3.14159265f) / pointCount;

            for (int i = 0; i < pointCount; ++i) {
                float angle1 = i * angleIncrement;
                float angle2 = (i + 1) * angleIncrement;

                sf::Vector2f p1(pos.x + radius * std::cos(angle1), pos.y + radius * std::sin(angle1));
                sf::Vector2f p2(pos.x + radius * std::cos(angle2), pos.y + radius * std::sin(angle2));

                m_untexturedHeadVertices.append(sf::Vertex(pos, color));
                m_untexturedHeadVertices.append(sf::Vertex(p1, color));
                m_untexturedHeadVertices.append(sf::Vertex(p2, color));
            }
        }
    }
}
