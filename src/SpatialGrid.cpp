#include "SpatialGrid.h"

SpatialGrid::SpatialGrid(float width, float height, float cellSize)
    : m_width(width), m_height(height), m_cellSize(cellSize) {
}

void SpatialGrid::clear() {
    m_grid.clear();
}

void SpatialGrid::insert(Particle* particle) {
    if (!particle) return;

    int cellX = static_cast<int>(particle->getPosition().x / m_cellSize);
    int cellY = static_cast<int>(particle->getPosition().y / m_cellSize);
    
    long long hash = getCellHash(cellX, cellY);
    m_grid[hash].push_back(particle);
}

std::vector<Particle*> SpatialGrid::getNearbyParticles(Particle* particle) {
    std::vector<Particle*> nearbyParticles;
    if (!particle) return nearbyParticles;

    int centralCellX = static_cast<int>(particle->getPosition().x / m_cellSize);
    int centralCellY = static_cast<int>(particle->getPosition().y / m_cellSize);
    
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            int cellX = centralCellX + x;
            int cellY = centralCellY + y;
            
            long long hash = getCellHash(cellX, cellY);
            
            if (m_grid.count(hash)) {
                nearbyParticles.insert(nearbyParticles.end(), m_grid[hash].begin(), m_grid[hash].end());
            }
        }
    }
    
    return nearbyParticles;
}

long long SpatialGrid::getCellHash(int cellX, int cellY) const {
    return static_cast<long long>(cellX) << 32 | static_cast<long long>(cellY);
}
