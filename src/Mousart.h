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
    
    // Escalas específicas por tipo de cursor
    std::vector<float> m_cursorScales;
    
    // Offsets específicos por tipo de cursor para a ponta
    std::vector<sf::Vector2f> m_cursorTipOffsets;
    
    // hotspot é a posição exata dentro da textura onde o clique acontece
    sf::Vector2f m_hotspot;
    
    sf::Vector2f m_tipOffset;
    // Aplica a escala apropriada para o cursor atual
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
    
    // Considera a resolução atual da janela para garantir consistencia
    sf::Vector2f getCursorTipOffset(const sf::RenderWindow& window) const;
};
