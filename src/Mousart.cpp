#include "Mousart.h"
#include <iostream>

Mousart::Mousart() : m_isForceActive(false), m_currentType(CursorType::SKULL), m_hotspot(0, 0) {
    m_cursorTextures.resize(static_cast<size_t>(CursorType::COUNT));
    m_cursorScales.resize(static_cast<size_t>(CursorType::COUNT));
    m_cursorTipOffsets.resize(static_cast<size_t>(CursorType::COUNT));
    
    m_cursorScales[static_cast<size_t>(CursorType::DEFAULT)] = 0.04f;
    m_cursorScales[static_cast<size_t>(CursorType::FORCE)] = 0.04f;
    m_cursorScales[static_cast<size_t>(CursorType::SKULL)] = 0.06f;
    
    m_cursorTipOffsets[static_cast<size_t>(CursorType::DEFAULT)] = sf::Vector2f(-5.0f, -5.0f);
    m_cursorTipOffsets[static_cast<size_t>(CursorType::FORCE)] = sf::Vector2f(-5.0f, -5.0f);
    m_cursorTipOffsets[static_cast<size_t>(CursorType::SKULL)] = sf::Vector2f(35.0f, 0.0f);
}

bool Mousart::initialize() {
    if (!m_cursorTextures[static_cast<size_t>(CursorType::DEFAULT)].loadFromFile("assets/retromouse.png")) {
        std::cerr << "Erro ao carregar assets/retromouse.png" << std::endl;
        
        if (!m_cursorTextures[static_cast<size_t>(CursorType::DEFAULT)].loadFromFile("assets/retromouse_alt.png")) {
            std::cerr << "Erro ao carregar arquivo de cursor alternativo" << std::endl;
            return false;
        }
    }
    
    if (!m_cursorTextures[static_cast<size_t>(CursorType::FORCE)].loadFromFile("assets/retromouse_force.png")) {
        std::cerr << "Erro ao carregar assets/retromouse_force.png" << std::endl;
        m_cursorTextures[static_cast<size_t>(CursorType::FORCE)] = m_cursorTextures[static_cast<size_t>(CursorType::DEFAULT)];
    }
    
    // Carregar cursor esqueleto
    if (!m_cursorTextures[static_cast<size_t>(CursorType::SKULL)].loadFromFile("assets/mouseskull.png")) {
        std::cerr << "Erro ao carregar assets/mouseskull.png" << std::endl;
        m_cursorTextures[static_cast<size_t>(CursorType::SKULL)] = m_cursorTextures[static_cast<size_t>(CursorType::DEFAULT)];
    }
    
    // Configurar texturas
    for (auto& texture : m_cursorTextures) {
        texture.setSmooth(true);
    }
    
    m_cursorSprite.setTexture(m_cursorTextures[static_cast<size_t>(m_currentType)], true);
    
    sf::Vector2u textureSize = m_cursorTextures[static_cast<size_t>(m_currentType)].getSize();
    
    // Aplicar escala específica para o tipo de cursor atual
    applyCurrentScale();
    m_cursorSprite.setColor(sf::Color(255, 255, 255, 255));
    // Definir hotspot para posicionar o centro do cursor
    m_hotspot = sf::Vector2f(textureSize.x * 0.5f, textureSize.y * 0.5f);
    m_cursorSprite.setOrigin(m_hotspot);
    
    updateTipOffset();
    
    return true;
}

void Mousart::update(const sf::Vector2i& mousePosition, const sf::RenderWindow& window) {
    // Obtém o tamanho atual da janela
    sf::Vector2u windowSize = window.getSize();

    sf::Vector2i limitedPosition = mousePosition;
    
    // Limita a posição do cursor aos limites da janela
    limitedPosition.x = std::max(0, std::min(static_cast<int>(windowSize.x - 2), limitedPosition.x));
    limitedPosition.y = std::max(0, std::min(static_cast<int>(windowSize.y - 2), limitedPosition.y)); 
    m_cursorSprite.setPosition(static_cast<float>(limitedPosition.x), static_cast<float>(limitedPosition.y));
}

void Mousart::applyCurrentScale() {
    float scale = m_cursorScales[static_cast<size_t>(m_currentType)];
    m_cursorSprite.setScale(scale, scale);
}

void Mousart::updateTipOffset() {
    if (m_isForceActive) {
        m_tipOffset = m_cursorTipOffsets[static_cast<size_t>(CursorType::FORCE)];
    } else {
        m_tipOffset = m_cursorTipOffsets[static_cast<size_t>(m_currentType)];
    }
}

void Mousart::cycleCursorType() {
    int nextType = static_cast<int>(m_currentType) + 1;
    if (nextType == static_cast<int>(CursorType::FORCE)) {
        nextType++;
    }
    if (nextType >= static_cast<int>(CursorType::COUNT)) {
        nextType = 0; 
    }
    
    m_currentType = static_cast<CursorType>(nextType);
    m_cursorSprite.setTexture(m_cursorTextures[static_cast<size_t>(m_currentType)]);
    // Aplicar a escala apropriada
    applyCurrentScale();
    updateTipOffset();
}

Mousart::CursorType Mousart::getCurrentType() const {
    return m_currentType;
}

void Mousart::setForceMode(bool isActive) {
    m_isForceActive = isActive;
    updateTipOffset();
}

void Mousart::draw(sf::RenderWindow& window) {
    window.draw(m_cursorSprite);
}

sf::Vector2f Mousart::getCursorTipOffset() const {
    if (!m_isForceActive && m_currentType == CursorType::SKULL) {
        return sf::Vector2f(-12.0f, -27.0f);
    }
    sf::Transform spriteTransform = m_cursorSprite.getTransform();
    sf::Vector2f transformedPoint = spriteTransform.transformPoint(m_tipOffset.x, m_tipOffset.y);
    sf::Vector2f cursorPos = m_cursorSprite.getPosition();
    sf::Vector2f finalOffset = transformedPoint - cursorPos;
    
    return finalOffset;
}
