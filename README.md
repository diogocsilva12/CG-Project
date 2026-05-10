# CG-Project — Computer Graphics with OpenGL

![C++](https://img.shields.io/badge/C++-11-blue.svg)
![OpenGL](https://img.shields.io/badge/OpenGL-Rendering-green.svg)
![CMake](https://img.shields.io/badge/Build-CMake-orange.svg)
![Grade](https://img.shields.io/badge/Final%20Grade-18%2F20-success.svg)

Final project developed for the **Computer Graphics (CG)** course at the **University of Minho**.

**Final Grade:** `18 / 20`

This repository implements a small real-time 3D graphics system built with **C++**, **OpenGL**, **GLUT/GLEW**, **DevIL**, and **TinyXML2**. The project evolved across four phases, starting with procedural generation of geometric primitives and culminating in a textured solar-system scene with hierarchical transformations, camera interaction, XML-based scene configuration, and animated rendering.

---

## Table of Contents

- [Overview](#overview)
- [Demo](#demo)
- [Main Features](#main-features)
- [Project Evolution](#project-evolution)
- [Repository Structure](#repository-structure)
- [Technologies](#technologies)
- [Build and Run](#build-and-run)
- [Generator Usage](#generator-usage)
- [Solar System Example](#solar-system-example)
- [Learning Outcomes](#learning-outcomes)
- [Authors](#authors)

---

## Overview

The goal of this project was to design and implement a complete graphics application capable of:

- generating 3D geometry procedurally;
- storing generated models in `.3d` files;
- loading scene descriptions from XML configuration files;
- rendering 3D scenes using OpenGL;
- applying geometric transformations, materials, textures, and camera controls;
- simulating a hierarchical animated solar system.

The final version combines a **model generator** and an **OpenGL rendering engine**. The generator produces primitive meshes such as planes, boxes, cones, spheres, tori, circles, and Bezier patches. The engine reads generated geometry and XML scene files, then renders the configured 3D world interactively.

---

## Demo

The original Phase 4 README included the project demonstration assets. They are now surfaced directly in the main README.

### Interactive Generator and Viewer

![Demo](https://github.com/diogocsilva12/CG-Project/raw/9a2d0f71306a7a6b5c4671d0377e751600867192/Phase2/usage_example.gif)

### Solar System Simulation

![Solar System Simulation](https://github.com/diogocsilva12/CG-Project/blob/main/Phase3/solarsystem.gif?raw=true)

### Solar System Simulation with Textures

![Solar System Simulation with Textures](https://github.com/diogocsilva12/CG-Project/blob/3a21e267f26b1e10327d0099ac75db01ab9596fa/Phase4/demo%20solar%20system%20with%20textures.gif?raw=true)

---

## Main Features

### Geometry Generator

The command-line generator can create multiple 3D objects and save them as `.3d` model files:

- plane;
- box;
- cone;
- sphere;
- torus;
- circle/orbit paths;
- Bezier patch surfaces.

Generated models are saved in the `tests/` directory and can be loaded by the rendering engine.

### Rendering Engine

The OpenGL engine supports:

- loading `.3d` model files;
- XML-based scene configuration;
- real-time rendering;
- camera/view manipulation;
- hierarchical transformations;
- translation, rotation and scaling;
- animated transformations;
- Catmull-Rom curves;
- textured models;
- material and lighting support;
- solar-system style scene composition.

### Interactive Script

The Phase 4 `run.sh` script provides a terminal menu for:

1. cleaning and rebuilding the project;
2. generating figures;
3. viewing generated figures;
4. running predefined test scenes;
5. launching solar-system configurations.

---

## Project Evolution

The project is organized into progressive development phases:

| Phase | Focus |
|---|---|
| `Phase 1` | Initial geometry generation and basic rendering pipeline |
| `Phase2` | Interactive usage workflow and expanded primitive generation |
| `Phase3` | Solar-system simulation and hierarchical scene structure |
| `Phase4` | Textures, Bezier patches, improved engine structure, CMake build, XML parsing, and final demo |

---

## Repository Structure

```text
CG-Project/
├── Phase 1/                  # Initial project phase
├── Phase2/                   # Second development phase
├── Phase3/                   # Solar-system simulation phase
├── Phase4/                   # Final version of the project
│   ├── engine/               # OpenGL rendering engine
│   ├── generator/            # 3D model generator
│   ├── external/tinyxml2/    # XML parser dependency
│   ├── patches/              # Bezier patch files
│   ├── tests/                # Generated/test .3d models
│   ├── CMakeLists.txt        # CMake build configuration
│   ├── run.sh                # Interactive build/run script
│   ├── Readme.md             # Original Phase 4 notes and demo links
│   └── *.gif / *.jpeg        # Demo media
├── test files/               # Additional test resources
├── Trabalho CG.eng.pdf       # Project statement/report material
└── README.md                 # Main repository documentation
```

---

## Technologies

- **C++11**
- **OpenGL**
- **GLUT**
- **GLEW**
- **DevIL / IL / ILU / ILUT** for texture loading
- **TinyXML2** for XML parsing
- **CMake**
- **Shell scripting**

---

## Build and Run

The final implementation is located in `Phase4/`.

### 1. Clone the repository

```bash
git clone https://github.com/diogocsilva12/CG-Project.git
cd CG-Project/Phase4
```

### 2. Install dependencies

On Ubuntu/Debian-like systems:

```bash
sudo apt update
sudo apt install build-essential cmake freeglut3-dev libglew-dev libdevil-dev
```

On macOS with Homebrew:

```bash
brew install cmake freeglut glew devil
```

### 3. Run the interactive script

```bash
chmod +x run.sh
./run.sh
```

The script can clean/build the project, generate figures, view generated models, and run predefined scene configurations.

### 4. Manual CMake build

```bash
mkdir -p build
cd build
cmake ..
make
```

This creates the main executables:

- `generator/generator` — procedural model generator;
- `build/engine` — OpenGL rendering engine.

---

## Generator Usage

General form:

```bash
./generator <primitive> <parameters> <output_file.3d>
```

Examples:

```bash
# Plane
./generator plane 1 3 ../tests/plane_1_3.3d

# Box
./generator box 2 3 ../tests/box_2_3.3d

# Sphere
./generator sphere 1 10 10 ../tests/sphere_1_10_10.3d

# Cone
./generator cone 1 2 16 8 ../tests/cone_1_2_16_8.3d

# Torus
./generator torus 1 0.5 16 8 ../tests/torus_1_0.5_16_8.3d

# Bezier patch
./generator bezier ../patches/teapot.patch 10 ../tests/bezier_teapot_10.3d
```

---

## Solar System Example

The final phase includes a solar-system simulation using generated spheres, orbit curves, hierarchical transformations, and textures.

Example model generation commands:

```bash
# Generate sphere for celestial bodies
./generator sphere 1 8 8 sphere_1_8_8.3d

# Generate torus for Saturn's rings
./generator torus 1 0.5 16 8 torus_1_0.5_16_8.3d

# Generate circles for orbits
./generator circle 30 64 circle_30_64.3d   # Mercury orbit
./generator circle 40 64 circle_40_64.3d   # Venus orbit
./generator circle 50 64 circle_50_64.3d   # Earth orbit
./generator circle 3 32 circle_3_32.3d     # Moon orbit
./generator circle 60 64 circle_60_64.3d   # Mars orbit
./generator circle 2.5 32 circle_2.5_32.3d # Phobos orbit
./generator circle 3.5 32 circle_3.5_32.3d # Deimos orbit
./generator circle 75 64 circle_75_64.3d   # Jupiter orbit
./generator circle 4 32 circle_4_32.3d     # Various moon orbits
./generator circle 90 64 circle_90_64.3d   # Saturn orbit
./generator circle 102 64 circle_102_64.3d # Uranus orbit
./generator circle 110 64 circle_110_64.3d # Neptune orbit
```

The Phase 4 engine can then load XML scene files from `engine/configs/`, including static and dynamic solar-system configurations.

---

## Learning Outcomes

This project demonstrates practical knowledge of:

- procedural mesh generation;
- real-time 3D rendering;
- graphics pipeline organization;
- OpenGL transformations and camera control;
- hierarchical modelling;
- XML-driven scene descriptions;
- texture mapping;
- Bezier surface tessellation;
- interactive graphics application development in C++.

---

## Authors

- Diogo Coelho da Silva
- Eduardo Pereira
- Pedro Oliveira
- João Barbosa

**BSc in Computer Science**  
**University of Minho**

---

## Academic Context

Developed as part of the **Computer Graphics (CG)** course at the **University of Minho**.

**Final Grade:** `18 / 20`
