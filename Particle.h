#pragma once
#include <SFML/Graphics.hpp>

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
    
    
    sf::Vector2f getPosition() const { return m_shape.getPosition(); }
    void setPosition(const sf::Vector2f& position) { m_shape.setPosition(position); }
    
    sf::Vector2f getVelocity() const { return vel; }
    void setVelocity(const sf::Vector2f& velocity) { vel = velocity; }
    
    float getMass() const { return mass; }
    float getRadius() const { return m_shape.getRadius(); }
    
    sf::Color getColor() const { return m_shape.getFillColor(); }
    void setColor(const sf::Color& color) { m_shape.setFillColor(color); }

private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    sf::CircleShape m_shape;
    sf::Vector2f vel;              
    sf::Vector2f m_accel;         
    float mass;                    
    
    // Valor 0.98 funciona bem, testei entre 0.95-0.99
    static constexpr float DAMPING = 0.98f; 
};
