#include "TextureManager.h"
#include <iostream>
#include <filesystem>

std::map<std::string, std::shared_ptr<sf::Texture>> TextureManager::m_textures;

std::shared_ptr<sf::Texture> TextureManager::getTexture(std::string_view filename) {
    // Converter para std::string para compatibilidade com std::map
    const std::string filenameStr(filename);
    
    // Verificar cache primeiro - evita alocações desnecessárias
    auto it = m_textures.find(filenameStr);
    if (it != m_textures.end()) {
        return it->second;
    }
    
    // Também verificar se já temos a fallback para evitar recriar
    static bool fallbackCreated = false;
    if (filenameStr == "fallback" && fallbackCreated) {
        return m_textures["fallback"];
    }
    
    // Criar um novo shared_ptr para a textura
    auto texPtr = std::make_shared<sf::Texture>();
    bool loaded = false;
    
    // Primeiro tentar diretamente do caminho fornecido
    if (texPtr->loadFromFile(filenameStr)) {
        loaded = true;
        // Textura carregada com sucesso do caminho direto
    } 
    // Se falhar, tentar no diretório assets/
    else if (texPtr->loadFromFile("assets/" + filenameStr)) {
        loaded = true;
        // Textura carregada com sucesso do diretório assets/
    }
    // Se ainda falhar, tentar no diretório sprites/
    else if (texPtr->loadFromFile("sprites/" + filenameStr)) {
        loaded = true;
        // Textura carregada com sucesso do diretório sprites/
    }
    
    // Se a textura foi carregada com sucesso
    if (loaded) {
        texPtr->setSmooth(true);
        texPtr->generateMipmap(); // Gera mipmaps apenas uma vez
        
        // Armazenar o shared_ptr no mapa e retornar
        m_textures[filenameStr] = texPtr;
        return texPtr;
    }
    
    // Criar textura de fallback (somente uma vez)
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
    
    // Usando textura de fallback
    return m_textures["fallback"];
}

bool TextureManager::preloadTexture(std::string_view filename) {
    // Verificar se já está carregada
    const std::string filenameStr(filename);
    if (isTextureLoaded(filename)) {
        return true;
    }
    
    // Tentar carregar a textura
    sf::Texture tex;
    bool loaded = false;
    
    // Primeiro tentar diretamente do caminho fornecido
    if (tex.loadFromFile(filenameStr)) {
        loaded = true;
    } 
    // Se falhar, tentar no diretório assets/
    else if (tex.loadFromFile("assets/" + filenameStr)) {
        loaded = true;
    }
    // Se ainda falhar, tentar no diretório sprites/
    else if (tex.loadFromFile("sprites/" + filenameStr)) {
        loaded = true;
    }
    
    // Se a textura foi carregada com sucesso
    if (loaded) {
        tex.setSmooth(true);
        tex.generateMipmap();
        
        // Criar um shared_ptr a partir da textura carregada
        auto texPtr = std::make_shared<sf::Texture>(std::move(tex));
        
        // Agora inserimos o shared_ptr no mapa
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
