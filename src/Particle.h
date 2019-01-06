#pragma once
#include <SFML/Graphics.hpp>
#include "TextureManager.h"
#include <deque>
#include <vector>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

enum class ParticleType {
    Original,  // Partículas originais
    Crystal,   // Cristais neon
};

class Particle : public sf::Drawable {
public:
    Particle(float mass, const sf::Vector2f& position, const sf::Vector2f& velocity, const sf::Color& color);
    virtual ~Particle() = default;

    void update(float dt);
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
    float getRadius() const { return radius; }
    
    sf::Color getColor() const { return m_sprite.getColor(); }
    void setColor(const sf::Color& color) { m_sprite.setColor(color); }
    
    void setParticleType(ParticleType type);
    ParticleType getParticleType() const { return m_type; }

private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    void updateTrailColor();
    
    static void RGBtoHSV(uint8_t r, uint8_t g, uint8_t b, float& h, float& s, float& v);
    static void HSVtoRGB(float h, float s, float v, uint8_t& r, uint8_t& g, uint8_t& b);

private:
    sf::Sprite m_sprite;
    sf::Texture* m_texture;  
    ParticleType m_type;
    float radius;           
    sf::Vector2f vel;      
    sf::Vector2f m_accel;  
    float mass;
    
    static constexpr float DAMPING = 0.995f; 
    
    // Rastros
    std::deque<std::pair<sf::Vector2f, sf::Color>> m_trail;
    static constexpr int MAX_TRAIL_LENGTH = 15;
    static constexpr float TRAIL_FADE_RATE = 0.85f;
    
    // Variação de cores
    sf::Color m_baseColor;
    float m_colorPulsePhase;
    bool m_useSpeedColor;
};
