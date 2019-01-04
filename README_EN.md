# Particle Simulator

A simulator with some physics. Made to play around with C++ & SFML, and, who knows, use it in future projects that probably won't happen.


<br>

<img src="gifrec.gif" alt="Particle Simulator Demo" width="650"/>

<br>

## Verlet

The simulator uses Verlet integration for movement calculations. Unlike the Euler method (used in the initial versions), the Verlet method prevents particles from passing through each other when they're moving fast, or when the FPS drops. The system also simulates energy loss in collisions, preventing particles from bouncing around like there's no tomorrow.
In summary: Verlet method doesn't just consider current position and velocity, but also remembers previous position and acceleration to predict where the particle should be, as if each particle had a "memory". This makes movements smoother and more realistic.

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

### Compile:
```
compile.bat
```

### Run:
```
ParticleSimulator.exe
```

### Or using the script:
```
run.bat
```


**Requirements:** C++ 11+ and SFML 2.5+

Performance may vary with many particles/interactions - Use at your own risk.

~Works on my machine~

<div align="right">
  <sub><a href="https://github.com/augustoperin">Augusto Cesar Perin</a> | 2018-2019</sub>
</div>
