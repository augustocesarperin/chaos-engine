# Particle Simulator

A 2D particle simulator. Moves circles in a window with basic physics. Made to explore C++ and SFML, not for cutting-edge science.

![Particle Simulator Demo](gravacao.gif)

## Core Features

*   **Particles (`Particle`):**
    Circles with `mass`, `radius`, `position`, `velocity`, and `color`. Mass and radius affect collisions and gravity.

*   **Particle System (`ParticleSystem`):**
    Manages particles: creates, removes, applies forces (gravity), handles collisions, and updates states (`deltaTime`).

*   **Physical Interactions:**
    *   **Gravity:** Constant vertical acceleration (adjustable, toggle on/off).
    *   **Collisions:** Detection by circle overlap; basic elastic resolution with position correction.
    *   **Repulsion (Experimental):** Force between pairs, based on the inverse square of the distance (adjustable, toggle on/off).

## Interaction

*   **Mouse:**
    *   `Left Click`: Standard particle.
    *   `Right Click`: Larger/heavier particle.
*   **Keyboard:**
    *   `G`: Toggles gravity.
    *   `R`: Toggles repulsion.
    *   `C`: Clears particles.
    *   `Up/Down Arrows`: Adjusts gravity.
    *   `Left/Right Arrows`: Adjusts repulsion.
    *   `ESC`: Quits.

## Compilation

*   **Requirements:** C++ (11+), SFML (>=2.5).
*   **Build:** Tested with g++/MinGW-w64 (Windows). Use `Makefile` or `compile_run.bat`. SFML must be accessible.

## Considerations

Performance may vary with many particles/interactions - Use at your own risk.

---
Augusto César Perin
