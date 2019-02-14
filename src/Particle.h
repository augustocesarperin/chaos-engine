#pragma once
#include <SFML/Graphics.hpp>
#include "TextureManager.h"
#include <deque>
#include <vector>
#include <cmath>
#include <memory>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

enum class ParticleType {
    Original,
    Crystal,
};

class Particle : public sf::Drawable {
public:
    Particle(float mass, const sf::Vector2f& position, const sf::Vector2f& velocity, const sf::Color& color);
    virtual ~Particle() = default;

    void updateVisuals(float dt);
    void applyForce(const sf::Vector2f& f);
    void applyDrag(float dragCoefficient);
    
    bool checkCollision(const Particle& other) const;
    
    void resolveCollision(Particle& other);
    void keepInBounds(float width, float height);
    
    sf::Vector2f getPosition() const { return m_sprite.getPosition(); }
    void setPosition(const sf::Vector2f& position) { m_sprite.setPosition(position); }
    
    sf::Vector2f getVelocity() const { return vel; }
    void setVelocity(const sf::Vector2f& velocity) { vel = velocity; }
    
    float getMass() const { return mass; }
    void setMass(float mass, bool manageRadius = true) {
        this->mass = mass;
        if (manageRadius) {
            this->radius = 5.0f + mass;
        }
    }
    float getRadius() const { return radius; }
    
    sf::Color getColor() const { return m_sprite.getColor(); }
    void setColor(const sf::Color& color) { m_sprite.setColor(color); }
    
    void setParticleType(ParticleType type);
    ParticleType getParticleType() const { return m_type; }
    
    size_t getPoolIndex() const { return poolIndex; }
    void setPoolIndex(size_t index) { poolIndex = index; }
    
    size_t getSoAIndex() const { return m_soaIndex; }
    void setSoAIndex(size_t index) { m_soaIndex = index; }
    
    void renderTo(sf::RenderTarget& target, sf::RenderStates states = sf::RenderStates::Default) const {
        draw(target, states);
    }

private:
    void updateTrail();

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    void updateTrailColor();
    
    static void RGBtoHSV(uint8_t r, uint8_t g, uint8_t b, float& h, float& s, float& v);
    static void HSVtoRGB(float h, float s, float v, uint8_t& r, uint8_t& g, uint8_t& b);

private:
    sf::Sprite m_sprite;
    std::shared_ptr<sf::Texture> m_texture;  
    ParticleType m_type;
    float radius;           
    sf::Vector2f vel;      
    sf::Vector2f m_accel;  
    float mass;
    
    static constexpr float DAMPING = 0.998f; 
    
    static constexpr int MAX_TRAIL_LENGTH = 15;
    static constexpr float TRAIL_FADE_RATE = 0.85f;
    
    struct TrailPoint {
        sf::Vector2f position;
        sf::Color color;
    };
    
    TrailPoint m_trailBuffer[MAX_TRAIL_LENGTH];
    int m_trailHead = 0;
    int m_trailSize = 0;
    
    sf::Color m_baseColor;
    float m_colorPulsePhase;
    bool m_useSpeedColor;

    size_t poolIndex;
    size_t m_soaIndex;

    std::vector<sf::Vertex> m_trailVertices;
};
