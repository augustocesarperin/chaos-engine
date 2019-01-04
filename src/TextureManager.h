#pragma once
#include <SFML/Graphics.hpp>
#include <map>
#include <string>

class TextureManager {
private:
    static std::map<std::string, sf::Texture> m_textures;
    
public:
    static sf::Texture* getTexture(const std::string& filename);
    static void clearAll();
};
