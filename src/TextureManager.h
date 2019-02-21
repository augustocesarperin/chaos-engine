#pragma once
#include <SFML/Graphics.hpp>
#include <map>
#include <string>
#include <memory>
#include <string_view>
#include <iostream>

class TextureManager {
private:
    static std::map<std::string, std::shared_ptr<sf::Texture>> m_textures;
    
    TextureManager() = delete;
    ~TextureManager() = delete;
    TextureManager(const TextureManager&) = delete;
    TextureManager& operator=(const TextureManager&) = delete;
    
public:
    static std::shared_ptr<sf::Texture> getTexture(std::string_view filename);
    
    static bool preloadTexture(std::string_view filename);
    
    static bool isTextureLoaded(std::string_view filename);
    
    static void clearAll();
};
