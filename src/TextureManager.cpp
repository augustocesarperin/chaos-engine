#include "TextureManager.h"
#include <iostream>

std::map<std::string, sf::Texture> TextureManager::m_textures;

sf::Texture* TextureManager::getTexture(const std::string& filename) {
    auto it = m_textures.find(filename);
    if (it != m_textures.end()) {
        return &it->second;
    }
    
    sf::Texture tex;
    if (tex.loadFromFile(filename)) {
        std::cout << "Textura carregada com sucesso: " << filename << std::endl;
        
        // garantir transparencia em pngs
        tex.setSmooth(true);       
        tex.generateMipmap();       
        
        // Importante: n modificar o pixel format da imagem
        sf::Image img = tex.copyToImage();
        std::cout << "  - Dimensoes: " << img.getSize().x << "x" << img.getSize().y << std::endl;
        std::cout << "  - Tem transparencia: SIM" << std::endl;
        
        // Criar nova textura a partir da imagem 
        sf::Texture newTex;
        newTex.loadFromImage(img);
        newTex.setSmooth(true);
        
        m_textures[filename] = newTex;
        return &m_textures[filename];
    }
    
    if (tex.loadFromFile("sprites/" + filename)) {
        std::cout << "Textura carregada do diretório sprites/: " << filename << std::endl;
        tex.setSmooth(true);
        m_textures[filename] = tex;
        return &m_textures[filename];
    }
    
    std::cerr << "ERRO AO CARREGAR TEXTURA: " << filename << std::endl;
    
    // Textura reserva
    tex.create(32, 32);
    sf::Image img;
    img.create(32, 32, sf::Color::Magenta);
    tex.update(img);
    m_textures["fallback"] = tex;
    
    std::cout << "Usando textura de fallback para: " << filename << std::endl;
    return &m_textures["fallback"];
}

void TextureManager::clearAll() {
    m_textures.clear();
}
