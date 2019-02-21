#include <math.h>
#include <stdlib.h>

extern "C" {

float clamp(float value, float min, float max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

void update_particles_c(
    float* positions,
    float* previous_positions,
    float* velocities,
    float* accelerations,
    float* masses,
    float* radii,
    int particle_count,
    float dt,
    float world_width,
    float world_height,
    float restitution
) {
    const float BASE_AIR_RESISTANCE = 0.002f;

    for (int i = 0; i < particle_count; ++i) {
        float mass = masses[i];
        if (mass > 0.0001f) {
            // Aplicar resistência do ar baseada na velocidade atual
            float current_vel_x = (positions[i * 2] - previous_positions[i * 2]) / dt;
            float current_vel_y = (positions[i * 2 + 1] - previous_positions[i * 2 + 1]) / dt;
            
            float drag_x = -current_vel_x * (BASE_AIR_RESISTANCE / mass);
            float drag_y = -current_vel_y * (BASE_AIR_RESISTANCE / mass);
            
            accelerations[i * 2] += drag_x;
            accelerations[i * 2 + 1] += drag_y;
        }

        // Método de Verlet
        float new_x = 2.0f * positions[i * 2] - previous_positions[i * 2] + accelerations[i * 2] * dt * dt;
        float new_y = 2.0f * positions[i * 2 + 1] - previous_positions[i * 2 + 1] + accelerations[i * 2 + 1] * dt * dt;
        
        previous_positions[i * 2] = positions[i * 2];
        previous_positions[i * 2 + 1] = positions[i * 2 + 1];
        
        positions[i * 2] = new_x;
        positions[i * 2 + 1] = new_y;
        
        velocities[i * 2] = (positions[i * 2] - previous_positions[i * 2]) / dt;
        velocities[i * 2 + 1] = (positions[i * 2 + 1] - previous_positions[i * 2 + 1]) / dt;
        
        // Aplicar amortecimento diretamente na velocidade
        const float DAMPING = 0.998f;
        velocities[i * 2] *= DAMPING;
        velocities[i * 2 + 1] *= DAMPING;
        
        // Corrigir posição anterior baseada na nova vel
        previous_positions[i * 2] = positions[i * 2] - velocities[i * 2] * dt;
        previous_positions[i * 2 + 1] = positions[i * 2 + 1] - velocities[i * 2 + 1] * dt;
        
        // Verificar colisoes com bordas
        float radius = radii[i];
        if (positions[i * 2] < radius) {
            positions[i * 2] = radius;
            velocities[i * 2] *= -restitution;
            previous_positions[i * 2] = positions[i * 2] - velocities[i * 2] * dt;
        } else if (positions[i * 2] > world_width - radius) {
            positions[i * 2] = world_width - radius;
            velocities[i * 2] *= -restitution;
            previous_positions[i * 2] = positions[i * 2] - velocities[i * 2] * dt;
        }
        if (positions[i * 2 + 1] < radius) {
            positions[i * 2 + 1] = radius;
            velocities[i * 2 + 1] *= -restitution;
            previous_positions[i * 2 + 1] = positions[i * 2 + 1] - velocities[i * 2 + 1] * dt;
        } else if (positions[i * 2 + 1] > world_height - radius) {
            positions[i * 2 + 1] = world_height - radius;
            velocities[i * 2 + 1] *= -restitution;
            previous_positions[i * 2 + 1] = positions[i * 2 + 1] - velocities[i * 2 + 1] * dt;
        }
    }
}

} // extern "C"
