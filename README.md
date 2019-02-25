<div align="right">🇧🇷 <a href="README_pt-br.md">Ler em Português</a></div>

An engine with some physics and a lot of optimizations. Made for educational and procrastinatory purposes in C++ & SFML.

<p align="center">
  <img src="gifs/v09.gif" alt="Demonstration of the Final Version" width="650"/>
</p>

## Verlet Integration

This program uses Verlet integration for motion calculations. Unlike the Euler method (used in early versions), the Verlet method prevents particles from tunneling through each other at high speeds or when the FPS drops significantly. The system also simulates energy loss in collisions, preventing them from bouncing around ad infinitum.

## Main Optimizations

-   Spatial Grid
-   Object Pooling
-   Structure of Arrays (SoA)

## Controls

**Mouse:**
- Left-click: creates a normal particle
- Right-click: creates a large particle

**Keyboard:**
- `G`: toggles gravity
- `R`: toggles repulsion
- `M`: toggles mouse force
- `N`: switches between attract and repel
- `F`: changes the mouse force style
- `+/-`: adjusts force intensity
- `C`: clears all particles
- `Space`: creates random particles
- `ESC`: bye


## How to Compile

### Windows
Run the file:
```
compile.bat
```
> You need to have [MSYS2](https://www.msys2.org/) with MinGW tools (g++, make, cmake) and SFML installed and in the system's PATH.

### Linux
```sh
sudo apt update && sudo apt install build-essential git cmake libsfml-dev

git clone https://github.com/augustocesarperin/chaos-engine.git
cd chaos-engine
mkdir build && cd build
cmake ..
make
./Chaos
```

Despite being optimized, performance may vary with many particles/interactions. Use at your own risk.

(~Works on my machine~)

### Previous Versions

<p align="center">
  <b>v0.5</b><br>
  <img src="gifs/gifrec.gif" alt="Demonstration of Version 0.5" width="580"/>
</p>
<p align="center">
  <b>v0.1</b><br>
  <img src="gifs/v01.gif" alt="Demonstration of Version 0.1" width="580"/>
</p>

<div align="right">
  <sub><a href="https://github.com/augustocesarperin">Augusto Cesar Perin</a> | 2018-2019</sub>
</div> 