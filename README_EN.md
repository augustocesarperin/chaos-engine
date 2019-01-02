# Particle Simulator

A simulator with some physics. Made to play around with C++ & SFML, and, who knows, use it in future projects that probably won't happen.

![Particle Simulator Demo](gifrec.gif)

## Features
* **Physics** - Verlet integration for realistic movement and collision simulation
* **Interactions** - Gravitation, repulsion and force fields configurable in real-time

## Controls

**Mouse:**
- Left button: creates normal particle
- Right button: creates heavy particle

**Keyboard:**
- `G`: toggles gravity
- `R`: toggles repulsion
- `M`: toggles mouse force
- `N`: switches between attract and repel
- `+/-`: adjusts force intensity
- `C`: clears all particles
- `Space`: creates random particles
- `ESC`: exits

## How to Use

**Run directly:**
```
ParticleSimulator.exe
```

**Compile and run:**
```
compile.bat
```

**Requirements:** C++ 11+ and SFML 2.5+

Performance may vary with many particles/interactions - Use at your own risk.

~Works on my machine~

Augusto Cesar Perin - 2018 ~ 2019
