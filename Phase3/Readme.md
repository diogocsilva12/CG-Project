This document provides an overview of the project and instructions for running the 3D figure generator and viewer.

---

-- PHASE 3 CONCLUDED --

## Table of Contents
1. [Project Overview](#project-overview)
2. [Running Instructions](#running-instructions)
3. [Program Features](#program-features)
4. [Demo](#demo)

---

## Project Overview

This part 1 of the project consists of two main components:
1. **Generator**: A command-line tool to generate 3D primitive shapes (plane, box, cone, sphere) and save them in `.3d` files.
2. **Engine**: A 3D rendering engine that reads `.3d` files and renders them using OpenGL.

---

## Running Instructions

To run the program, follow these simple steps:

1. Make the script executable:
```bash
chmod +x run.sh
```
2. Run the script:
```bash
./run.sh
```


## Program Features
The generator program creates 3D primitive shapes and saves them in .3d files. 
Program Features
The interactive menu provides the following options:

1. **Clean and Build**
- Cleans the build directory
- Rebuilds both generator and engine

2. **Generate Figure**
- Plane (parameters: unit size, slices)
- Box (parameters: unit size, slices)
- Sphere (parameters: radius, slices, stacks)
- Cone (parameters: radius, height, slices, stacks)
- Torus (parameters: inner radius, outer radius, slices, stacks)
- Bezier patch 

All generated figures are automatically saved in tests

3. **View Figure**

- Lists all available generated figures
- Renders selected figure using OpenGL
- Config files are stored in configs

4. **Exit**
- Exits the program

Notes:

- Generated .3d files are saved in the tests directory
- XML configuration files are stored in configs folder 
- Creating a new figure will override any existing file with the same name


### TODO 
- [x] Fix some bugs
- [x] Clean up the code
- [x] Fix the box shape
- [x] Document the rest of the code 

### Demo
![Demo](https://github.com/diogocsilva12/CG-Project/raw/9a2d0f71306a7a6b5c4671d0377e751600867192/Phase2/usage_example.gif)

### Solar System Simulation
![Solar System Simulation](https://github.com/diogocsilva12/CG-Project/blob/main/Phase3/solarsystem.gif)



# Generate sphere for celestial bodies
- ./generator sphere 1 8 8 sphere_1_8_8.3d

# Generate torus for Saturn's rings
- ./generator torus 1 0.5 16 8 torus_1_0.5_16_8.3d

# Generate circles for orbits
- ./generator circle 30 64 circle_30_64.3d   # Mercury orbit
- ./generator circle 40 64 circle_40_64.3d   # Venus orbit
- ./generator circle 50 64 circle_50_64.3d   # Earth orbit
- ./generator circle 3 32 circle_3_32.3d     # Moon orbit
- ./generator circle 60 64 circle_60_64.3d   # Mars orbit
- ./generator circle 2.5 32 circle_2.5_32.3d # Phobos orbit
- ./generator circle 3.5 32 circle_3.5_32.3d # Deimos orbit
- ./generator circle 75 64 circle_75_64.3d   # Jupiter orbit
- ./generator circle 4 32 circle_4_32.3d     # Various moon orbits
- ./generator circle 90 64 circle_90_64.3d   # Saturn orbit
- ./generator circle 102 64 circle_102_64.3d # Uranus orbit
- ./generator circle 110 64 circle_110_64.3d # Neptune orbit
