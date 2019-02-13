#pragma once
#include <SFML/Graphics.hpp>
#include <map>
#include <string>
#include <memory>
#include <string_view>
#include <iostream>

class TextureManager {
private:
    // Uso de string como chave e shared_ptr para gerenciamento automático de recursos
    static std::map<std::string, std::shared_ptr<sf::Texture>> m_textures;
    
    // Previne instanciação desta classe utilitária
    TextureManager() = delete;
    ~TextureManager() = delete;
    TextureManager(const TextureManager&) = delete;
    TextureManager& operator=(const TextureManager&) = delete;
    
public:
    // Retorna um shared_ptr para garantir gerência de memória segura
    static std::shared_ptr<sf::Texture> getTexture(std::string_view filename);
    
    // Pré-carrega uma textura para uso posterior (opcional)
    static bool preloadTexture(std::string_view filename);
    
    // Verifica se uma textura já está carregada
    static bool isTextureLoaded(std::string_view filename);
    
    // Libera recursos
    static void clearAll();
};
