#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class Mousart {
public:
    enum class CursorType {
        DEFAULT,
        FORCE,
        SKULL,
        COUNT 
    };

private:
    std::vector<sf::Texture> m_cursorTextures;
    sf::Sprite m_cursorSprite;
    bool m_isForceActive;
    CursorType m_currentType;
    
    std::vector<float> m_cursorScales;
    
    std::vector<sf::Vector2f> m_cursorTipOffsets;
    
    sf::Vector2f m_hotspot;
    
    sf::Vector2f m_tipOffset;
    void applyCurrentScale();
    
    void updateTipOffset(); 

public:
    Mousart();
    ~Mousart() = default;
    
    bool initialize();
    void update(const sf::Vector2i& mousePosition, const sf::RenderWindow& window);
    void setForceMode(bool isActive);
    void draw(sf::RenderWindow& window);
    void cycleCursorType();
    CursorType getCurrentType() const;
    
    // Considera resolução da janela para garantir consistencia
    sf::Vector2f getCursorTipOffset() const;
};
