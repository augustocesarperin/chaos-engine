#pragma once
#include <SFML/Graphics.hpp>

namespace Utility {
    inline bool isVisible(const sf::Vector2f& position, float radius, const sf::View& view) {
        sf::Vector2f viewCenter = view.getCenter();
        sf::Vector2f viewSize = view.getSize();
        
        float left = viewCenter.x - viewSize.x / 2;
        float right = viewCenter.x + viewSize.x / 2;
        float top = viewCenter.y - viewSize.y / 2;
        float bottom = viewCenter.y + viewSize.y / 2;
        
        return (position.x + radius >= left &&
                position.x - radius <= right &&
                position.y + radius >= top &&
                position.y - radius <= bottom);
    }
}
