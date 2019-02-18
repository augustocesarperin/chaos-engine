#include "TextureManager.h"
#include <iostream>
#include <filesystem>

std::map<std::string, std::shared_ptr<sf::Texture>> TextureManager::m_textures;

std::shared_ptr<sf::Texture> TextureManager::getTexture(std::string_view filename) {
    const std::string filenameStr(filename);
    
    // cache pra evitar alocações desnecessárias
    auto it = m_textures.find(filenameStr);
    if (it != m_textures.end()) {
        return it->second;
    }
    
    static bool fallbackCreated = false;
    if (filenameStr == "fallback" && fallbackCreated) {
        return m_textures["fallback"];
    }
    
    auto texPtr = std::make_shared<sf::Texture>();
    bool loaded = false;
    
    if (texPtr->loadFromFile(filenameStr)) {
        loaded = true;
    } 

    else if (texPtr->loadFromFile("assets/" + filenameStr)) {
        loaded = true;
    }
    else if (texPtr->loadFromFile("sprites/" + filenameStr)) {
        loaded = true;
    }
    
    if (loaded) {
        texPtr->setSmooth(true);
        texPtr->generateMipmap(); 
        
        m_textures[filenameStr] = texPtr;
        return texPtr;
    }
    
    // textura reserva
    if (!fallbackCreated) {
        std::cerr << "[ERRO] Não foi possível carregar textura: " << filenameStr << " - usando fallback" << std::endl;
        
        auto fallbackTex = std::make_shared<sf::Texture>();
        fallbackTex->create(32, 32);
        
        sf::Image img;
        img.create(32, 32, sf::Color::Magenta);
        fallbackTex->update(img);
        
        m_textures["fallback"] = fallbackTex;
        fallbackCreated = true;
    } else {
        std::cerr << "[ERRO] Não foi possível carregar textura: " << filenameStr << " - usando fallback" << std::endl;
    }
    
    return m_textures["fallback"];
}

bool TextureManager::preloadTexture(std::string_view filename) {
    const std::string filenameStr(filename);
    if (isTextureLoaded(filename)) {
        return true;
    }
    
    sf::Texture tex;
    bool loaded = false;
    
    if (tex.loadFromFile(filenameStr)) {
        loaded = true;
    } 
    else if (tex.loadFromFile("assets/" + filenameStr)) {
        loaded = true;
    }
    else if (tex.loadFromFile("sprites/" + filenameStr)) {
        loaded = true;
    }
    
    if (loaded) {
        tex.setSmooth(true);
        tex.generateMipmap();
        
        auto texPtr = std::make_shared<sf::Texture>(std::move(tex));
        
        m_textures[filenameStr] = texPtr;
        return true;
    }
    
    return false;
}

bool TextureManager::isTextureLoaded(std::string_view filename) {
    const std::string filenameStr(filename);
    return m_textures.find(filenameStr) != m_textures.end();
}

void TextureManager::clearAll() {
    m_textures.clear();
}
