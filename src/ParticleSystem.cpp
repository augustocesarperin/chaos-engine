#include "ParticleSystem.h"
#include <random>
#include <cmath>

ParticleSystem::ParticleSystem(float width, float height)
    : m_width(width), m_height(height) {
}

ParticleSystem::~ParticleSystem() {
    // Destrutor vazio, não há recursos externos para liberar
}

void ParticleSystem::setWindowSize(float width, float height) {
    m_width = width;
    m_height = height;
}

Particle& ParticleSystem::addParticle(float mass, const sf::Vector2f& position, const sf::Vector2f& velocity, const sf::Color& color) {
    m_particles.emplace_back(mass, position, velocity, color);
    return m_particles.back();
}

void ParticleSystem::removeParticle(size_t index) {
    if (index < m_particles.size()) {
        m_particles.erase(m_particles.begin() + index);
    }
}

void ParticleSystem::update(float deltaTime) {
    // Constante para resistência do ar
    const float BASE_AIR_RESISTANCE = 0.002f;
    
    // Cache do número de partículas para evitar chamadas repetidas a size()
    const size_t numParticles = m_particles.size();
    
    // Primeira passagem: atualizar todas as partículas individualmente
    for (size_t i = 0; i < numParticles; ++i) {
        Particle& particle = m_particles[i];
        
        // Calcular resistência do ar usando a massa da partícula
        const float mass = particle.getMass();
        const float airResistance = BASE_AIR_RESISTANCE / mass;
        
        // Aplicar resistência, atualizar posição e manter dentro dos limites
        particle.applyDrag(airResistance);
        particle.update(deltaTime);
        particle.keepInBounds(m_width, m_height);
    }
    
    // Segunda passagem: verificar colisões entre partículas
    for (size_t i = 0; i < numParticles; ++i) {
        Particle& p1 = m_particles[i];
        
        for (size_t j = i + 1; j < numParticles; ++j) {
            Particle& p2 = m_particles[j];
            
            if (p1.checkCollision(p2)) {
                p1.resolveCollision(p2);
            }
        }
    }
}

void ParticleSystem::applyGravityEffect(float gravitationalAcceleration) {
    // Constante para verificação de massa mínima
    const float MIN_VALID_MASS = 0.0001f;
    
    // Cache do número de partículas
    const size_t numParticles = m_particles.size();
    
    for (size_t i = 0; i < numParticles; ++i) {
        // Usar referências para eficiência
        Particle& particle = m_particles[i];
        const float mass = particle.getMass();
        
        // Verificar massa válida com uma constante pequena em vez de zero exato
        if (mass > MIN_VALID_MASS) {
            // Pre-calcular a força da gravidade baseada na massa
            const sf::Vector2f gravityForce(0.0f, mass * gravitationalAcceleration);
            particle.applyForce(gravityForce);
        }
    }
}

void ParticleSystem::draw(sf::RenderWindow& window) {
    // Obter o tamanho da janela para determinar a área visível
    const sf::Vector2u windowSize = window.getSize();
    const sf::View& view = window.getView();
    
    // Calcular os limites da área visível com uma margem para partículas grandes
    const float MARGIN = 50.0f;  // Margem em pixels para não cortar partículas parcialmente visíveis
    
    // Obter os limites da área visível considerando a view atual
    const sf::FloatRect visibleArea(
        view.getCenter().x - view.getSize().x / 2.0f - MARGIN,
        view.getCenter().y - view.getSize().y / 2.0f - MARGIN,
        view.getSize().x + MARGIN * 2.0f,
        view.getSize().y + MARGIN * 2.0f
    );
    
    // Renderizar apenas partículas que estão na área visível
    for (const auto& particle : m_particles) {
        const sf::Vector2f& position = particle.getPosition();
        const float radius = particle.getRadius();
        
        // Verificar se a partícula está dentro ou parcialmente dentro da área visível
        // Uma partícula está dentro se qualquer parte dela está dentro do retangulo visível
        if (position.x + radius >= visibleArea.left && 
            position.x - radius <= visibleArea.left + visibleArea.width &&
            position.y + radius >= visibleArea.top && 
            position.y - radius <= visibleArea.top + visibleArea.height) {
            
            window.draw(particle);  // Renderizar apenas partículas visíveis
        }
    }
}

void ParticleSystem::applyInteractiveForces(float strength) {
    // Limitar a força máxima para evitar que as partículas saiam voando
    const float MAX_FORCE = 5000.0f;
    const float MIN_DISTANCE = 5.0f;
    const float MIN_DISTANCE_SQ = MIN_DISTANCE * MIN_DISTANCE;
    
    // Cache do número de partículas para evitar chamadas repetidas a size()
    const size_t numParticles = m_particles.size();
    
    for (size_t i = 0; i < numParticles; ++i) {
        // Armazena referências para as partículas para melhorar a eficiência
        Particle& p1 = m_particles[i];
        const float mass1 = p1.getMass();
        const sf::Vector2f& pos1 = p1.getPosition();
        
        for (size_t j = i + 1; j < numParticles; ++j) {
            // Armazena referências para a segunda partícula
            Particle& p2 = m_particles[j];
            const float mass2 = p2.getMass();
            const sf::Vector2f& pos2 = p2.getPosition();
            
            // Vetor entre as partículas
            const sf::Vector2f delta = pos1 - pos2;
            
            // Otimização: Usar quadrados para evitar raíz quadrada
            const float distanceSquared = delta.x * delta.x + delta.y * delta.y;
            
            // Evitar distâncias muito pequenas e divisões por zero
            if (distanceSquared < 0.0001f) continue;
            
            // Calcular raíz quadrada apenas uma vez
            const float distance = std::sqrt(distanceSquared);
            
            // Aumentar distância mínima para evitar forças extremas
            const float effectiveDistance = (distance < MIN_DISTANCE) ? MIN_DISTANCE : distance;
            const float effectiveDistanceSquared = effectiveDistance * effectiveDistance;
            
            // Calcular a força com base na distância
            // Cache para o produto das massas
            const float massProduct = mass1 * mass2;
            float forceMagnitude = strength * massProduct / effectiveDistanceSquared;
            
            // Limitar a força para evitar comportamento explosivo
            forceMagnitude = std::min(forceMagnitude, MAX_FORCE);
            
            // Direção da força (normalizada)
            const sf::Vector2f forceDirection = sf::Vector2f(delta.x / distance, delta.y / distance);
            const sf::Vector2f force = forceDirection * forceMagnitude;
            
            // Aplicar a força - Se força > 0: repulsão, se força< 0: atração
            p1.applyForce(force);
            p2.applyForce(-force);
        }
    }
}

void ParticleSystem::handleCollisions(float restitution) {
    // Constantes para evitar divisão por zero
    const float MIN_DISTANCE = 0.01f;
    
    // Cache do número de partículas para evitar chamadas repetidas a size()
    const size_t numParticles = m_particles.size();
    
    // Verifica todas as combinações possíveis de partículas
    for (size_t i = 0; i < numParticles; ++i) {
        // Usar referências para melhorar eficiência de acesso
        Particle& p1 = m_particles[i];
        const float radius1 = p1.getRadius();
        const float mass1 = p1.getMass();
        const float inverseMass1 = (mass1 > 0.0001f) ? 1.0f / mass1 : 0.0f;
        
        for (size_t j = i + 1; j < numParticles; ++j) {
            // Usar referências para melhorar eficiência de acesso
            Particle& p2 = m_particles[j];
            const float radius2 = p2.getRadius();
            const float mass2 = p2.getMass();
            const float inverseMass2 = (mass2 > 0.0001f) ? 1.0f / mass2 : 0.0f;
            
            // Cache da soma dos raios
            const float radiusSum = radius1 + radius2;
            const float radiusSumSquared = radiusSum * radiusSum;
            
            // Calcula a distância entre os centros das partículas
            const sf::Vector2f pos1 = p1.getPosition();
            const sf::Vector2f pos2 = p2.getPosition();
            const sf::Vector2f delta = pos1 - pos2;
            
            // Otimização: Usar quadrados para evitar raíz quadrada até ser necessário
            const float distanceSquared = delta.x * delta.x + delta.y * delta.y;
            
            // Verifica se as partículas estão colidindo (distância menor que a soma dos raios)
            if (distanceSquared < radiusSumSquared) {
                // Só precisamos calcular a raíz quadrada quando há uma possível colisão
                const float distance = std::max(std::sqrt(distanceSquared), MIN_DISTANCE);
                
                // Direção normal da colisão (já normalizada)
                const sf::Vector2f normal = delta / distance;
                
                // Calcula a velocidade relativa na direção normal
                const sf::Vector2f v1 = p1.getVelocity();
                const sf::Vector2f v2 = p2.getVelocity();
                const sf::Vector2f relativeVelocity = v1 - v2;
                
                // Produto escalar entre velocidade relativa e normal 
                const float normalVelocity = relativeVelocity.x * normal.x + relativeVelocity.y * normal.y;
                
                // Se as partículas estão se afastando, não há colisão a ser resolvida
                if (normalVelocity > 0) {
                    continue;
                }
                
                // Cache do fator de restituição aumentado
                const float restitutionFactor = 1.0f + restitution;
                
                // Calcula impulso com base nas massas e coeficiente de restituição
                // Usar massas inversas pré-calculadas para evitar divisões repetidas
                const float impulse = -restitutionFactor * normalVelocity / (inverseMass1 + inverseMass2);
                const sf::Vector2f impulseVector = normal * impulse;
                
                // Aplica impulso às velocidades
                p1.setVelocity(v1 + impulseVector * inverseMass1);
                p2.setVelocity(v2 - impulseVector * inverseMass2);
                
                // Corrige posições para evitar sobreposição (penetration resolution)
                const float penetration = radiusSum - distance;
                
                // Cache da soma das massas e proporções de deslocamento
                const float totalMass = mass1 + mass2;
                const float pushRatio1 = mass2 / totalMass;
                const float pushRatio2 = mass1 / totalMass;
                
                // Aplicar correção de posição
                p1.setPosition(pos1 + normal * penetration * pushRatio1);
                p2.setPosition(pos2 - normal * penetration * pushRatio2);
            }
        }
    }
}

void ParticleSystem::applyMouseForce(const sf::Vector2f& mousePosition, float strength, bool attractMode) {
    // Constantes para evitar divisão por zero ou forças muito grandes
    const float MIN_DISTANCE_SQ = 1.0f;
    
    // Cache do número de partículas
    const size_t numParticles = m_particles.size();
    
    // Pre-calcular o multiplicador de direção com base no modo de atração/repulsão
    const float directionMultiplier = attractMode ? 1.0f : -1.0f;
    
    for (size_t i = 0; i < numParticles; ++i) {
        // Usar referências para melhorar eficiência de acesso
        Particle& particle = m_particles[i];
        const sf::Vector2f& particlePosition = particle.getPosition();
        
        // Calcular vetor de direção da força
        const sf::Vector2f direction = mousePosition - particlePosition;
        const float distanceSquared = direction.x * direction.x + direction.y * direction.y;

        // Evitar distâncias muito pequenas e divisão por zero
        if (distanceSquared < MIN_DISTANCE_SQ) { 
            continue; 
        }

        // Calcular força apenas quando realmente necessário
        const float distance = std::sqrt(distanceSquared);  
        const float forceMagnitude = strength / distance;
        
        // Aplicar força na direção apropriada (atração ou repulsão)
        const sf::Vector2f normalizedDirection = direction / distance;
        const sf::Vector2f force = normalizedDirection * forceMagnitude * directionMultiplier;
        
        particle.applyForce(force);
    }
}

void ParticleSystem::generateRandomParticles(int count, float minMass, float maxMass) {
    for (int i = 0; i < count; ++i) {
        generateRandomParticle(minMass, maxMass);
    }
}

Particle& ParticleSystem::generateRandomParticle(float minMass, float maxMass) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> massDist(minMass, maxMass);
    std::uniform_real_distribution<float> posDist_x(0.0f, m_width);
    std::uniform_real_distribution<float> posDist_y(0.0f, m_height);
    std::uniform_real_distribution<float> velDist(-50.0f, 50.0f);
    std::uniform_int_distribution<int> colorDist(0, 255);
    
    float mass = massDist(gen);
    sf::Vector2f position(posDist_x(gen), posDist_y(gen));
    sf::Vector2f velocity(velDist(gen), velDist(gen));
    sf::Color color(colorDist(gen), colorDist(gen), colorDist(gen));
    
    return addParticle(mass, position, velocity, color);
}
