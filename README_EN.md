# Particle Simulator

A particle simulator in a window with some crumbs of physics implemented. Made to explore C++ and SFML, not for cutting-edge science.

![Particle Simulator Demo](gifrec.gif)

## Features

*   **Particles (`Particle`):**
    Circles with `mass`, `radius`, `position`, `velocity`, and `color`. Mass and radius affect collisions and gravity.

*   **Particle System (`ParticleSystem`):**
    Manages particles: creates, removes, applies forces (gravity), handles collisions, and updates states (`deltaTime`).

*   **Physical Interactions:**
    *   **Gravity:** Constant vertical acceleration (adjustable, toggle on/off).
    *   **Collisions:** Detection by circle overlap; basic elastic resolution with position correction.
    *   **Particle Repulsion:** Force between pairs, based on the inverse square of the distance (adjustable, toggle on/off).
    *   **Mouse Force:** Attraction or repulsion of particles in relation to the mouse position (adjustable, toggle on/off).

## Interaction

*   **Mouse:**
    *   `Left Click`: Standard particle.
    *   `Right Click`: Larger/heavier particle.
    *   `Position`: Center of attraction/repulsion when mouse force is active.
*   **Keyboard:**
    *   `G`: Toggles gravity.
    *   `R`: Toggles particle repulsion.
    *   `M`: Toggles mouse force.
    *   `N`: Switches between attraction and repulsion modes for mouse force.
    *   `+/-`: Adjusts mouse force strength.
    *   `C`: Clears particles.
    *   `Space`: Generates random particles.
    *   `ESC`: Quits.

## 

*   **Requirements:** C++ (11+), SFML (>=2.5).
*   Use the provided `ParticleSimulator.exe` or compile with `compile.bat`.

## Considerations

Performance may vary with many particles/interactions - Use at your own risk.

~It works on my machine~
