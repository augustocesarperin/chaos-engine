#pragma once
#include <SFML/Graphics.hpp>
#include "TextureManager.h"

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

private:
    sf::Sprite m_sprite;
    sf::Texture* m_texture;  
    ParticleType m_type;
    float radius;           
    sf::Vector2f vel;      
    sf::Vector2f m_accel;  
    float mass;            
    // Configurei valores pra deixar a gravidade mais perceptivel, mas isso pode (e deve) ser alterado.
    static constexpr float DAMPING = 0.995f; 
};
