#include "Mousart.h"
#include <iostream>

Mousart::Mousart() : m_isForceActive(false), m_hotspot(0, 0), m_currentType(CursorType::SKULL) {
    // Inicializa vetores com o número de tipos
    m_cursorTextures.resize(static_cast<size_t>(CursorType::COUNT));
    m_cursorScales.resize(static_cast<size_t>(CursorType::COUNT));
    m_cursorTipOffsets.resize(static_cast<size_t>(CursorType::COUNT));
    
    // Configura as escalas para cada tipo de cursor
    m_cursorScales[static_cast<size_t>(CursorType::DEFAULT)] = 0.04f;
    m_cursorScales[static_cast<size_t>(CursorType::FORCE)] = 0.04f;
    m_cursorScales[static_cast<size_t>(CursorType::SKULL)] = 0.06f; // Cursor esqueleto um pouco menor que antes
    
    // Configura os offsets da ponta para cada tipo de cursor
    m_cursorTipOffsets[static_cast<size_t>(CursorType::DEFAULT)] = sf::Vector2f(-5.0f, -5.0f);
    m_cursorTipOffsets[static_cast<size_t>(CursorType::FORCE)] = sf::Vector2f(-5.0f, -5.0f);
    m_cursorTipOffsets[static_cast<size_t>(CursorType::SKULL)] = sf::Vector2f(35.0f, 0.0f); // Ajuste muito mais significativo para a direita
}

bool Mousart::initialize() {
    if (!m_cursorTextures[static_cast<size_t>(CursorType::DEFAULT)].loadFromFile("assets/retromouse.png")) {
        std::cerr << "Erro ao carregar assets/retromouse.png" << std::endl;
        
        if (!m_cursorTextures[static_cast<size_t>(CursorType::DEFAULT)].loadFromFile("assets/retromouse_alt.png")) {
            std::cerr << "Erro ao carregar arquivo de cursor alternativo" << std::endl;
            return false;
        }
    }
    
    // Carregar cursor força
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
    
    // Obter tamanho da textura
    sf::Vector2u textureSize = m_cursorTextures[static_cast<size_t>(m_currentType)].getSize();
    
    // Aplicar escala específica para o tipo de cursor atual
    applyCurrentScale();
    
    m_cursorSprite.setColor(sf::Color(255, 255, 255, 255));
    
    // Definir hotspot para posicionar o centro do cursor
    m_hotspot = sf::Vector2f(textureSize.x * 0.5f, textureSize.y * 0.5f);
    m_cursorSprite.setOrigin(m_hotspot);
    
    // Atualiza o offset da ponta do cursor baseado no tipo atual
    updateTipOffset();
    
    return true;
}

void Mousart::update(const sf::Vector2i& mousePosition, const sf::RenderWindow& window) {
    // Obtém o tamanho atual da janela
    sf::Vector2u windowSize = window.getSize();
    
    // Cria uma cópia da posição do mouse para restringir aos limites da janela
    sf::Vector2i limitedPosition = mousePosition;
    
    // Limita a posição do cursor aos limites da janela
    // Subtrai um pequeno valor para garantir que o cursor fique totalmente visível
    limitedPosition.x = std::max(0, std::min(static_cast<int>(windowSize.x - 2), limitedPosition.x));
    limitedPosition.y = std::max(0, std::min(static_cast<int>(windowSize.y - 2), limitedPosition.y));
    
    // Atualiza a posição do sprite do cursor
    m_cursorSprite.setPosition(static_cast<float>(limitedPosition.x), static_cast<float>(limitedPosition.y));
}

void Mousart::applyCurrentScale() {
    float scale = m_cursorScales[static_cast<size_t>(m_currentType)];
    m_cursorSprite.setScale(scale, scale);
}

void Mousart::updateTipOffset() {
    // Se está no modo força, use o offset da força
    if (m_isForceActive) {
        m_tipOffset = m_cursorTipOffsets[static_cast<size_t>(CursorType::FORCE)];
    } else {
        m_tipOffset = m_cursorTipOffsets[static_cast<size_t>(m_currentType)];
    }
}

void Mousart::cycleCursorType() {
    // Permitir troca de cursor independente do modo de força
    
    // Avança para o próximo tipo (excluindo FORCE que é controlado por setForceMode)
    int nextType = static_cast<int>(m_currentType) + 1;
    if (nextType == static_cast<int>(CursorType::FORCE)) {
        nextType++;
    }
    if (nextType >= static_cast<int>(CursorType::COUNT)) {
        nextType = 0; // Voltar ao início
    }
    
    m_currentType = static_cast<CursorType>(nextType);
    
    // Aplicar a nova textura
    m_cursorSprite.setTexture(m_cursorTextures[static_cast<size_t>(m_currentType)]);
    
    // Aplicar a escala apropriada
    applyCurrentScale();
    
    // Atualizar offset da ponta
    updateTipOffset();
}

Mousart::CursorType Mousart::getCurrentType() const {
    return m_currentType;
}

void Mousart::setForceMode(bool isActive) {
    m_isForceActive = isActive;
    
    // NÃO alterar o sprite do cursor, apenas armazenar o estado de força
    // A troca de cursores deve ocorrer apenas com a tecla K (cycleCursorType)
    
    // Manter a mesma textura e escala, independentemente do modo de força
    // O cursor de força foi removido como sprite separado - usar apenas efeito visual
    
    // Atualizar o offset da ponta para o tipo de cursor atual
    updateTipOffset();
}

void Mousart::draw(sf::RenderWindow& window) {
    window.draw(m_cursorSprite);
}

sf::Vector2f Mousart::getCursorTipOffset(const sf::RenderWindow& window) const {
    if (!m_isForceActive && m_currentType == CursorType::SKULL) {
        return sf::Vector2f(-12.0f, -27.0f);
    }
    
    // Para outros cursores, usar a transformação normal
    sf::Transform spriteTransform = m_cursorSprite.getTransform();
    sf::Vector2f transformedPoint = spriteTransform.transformPoint(m_tipOffset.x, m_tipOffset.y);
    sf::Vector2f cursorPos = m_cursorSprite.getPosition();
    sf::Vector2f finalOffset = transformedPoint - cursorPos;
    
    return finalOffset;
}
