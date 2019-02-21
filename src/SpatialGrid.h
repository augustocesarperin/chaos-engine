#pragma once
#include "Particle.h"
#include <vector>
#include <unordered_map>

class SpatialGrid {
public:
    SpatialGrid(float width, float height, float cellSize);
    
    void clear();
    
    void insert(Particle* particle);
    
    std::vector<Particle*> getNearbyParticles(Particle* particle);

private:
    long long getCellHash(int cellX, int cellY) const;

    float m_width;
    float m_height;
    float m_cellSize;
    
    std::unordered_map<long long, std::vector<Particle*>> m_grid;
};
