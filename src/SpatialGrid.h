#pragma once
#include "Particle.h"
#include <vector>
#include <unordered_map>

class SpatialGrid {
public:
    SpatialGrid(float width, float height, float cellSize);
    
    void clear();
    
    // Insere uma partícula na célula apropriada da grade.
    void insert(Particle* particle);
    
    // Obtém uma lista de partículas nas células vizinhas à de uma partícula de referência.
    std::vector<Particle*> getNearbyParticles(Particle* particle);

private:
    long long getCellHash(int cellX, int cellY) const;

    float m_width;
    float m_height;
    float m_cellSize;
    
    std::unordered_map<long long, std::vector<Particle*>> m_grid;
};
