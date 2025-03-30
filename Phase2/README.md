This document provides an overview of the project and instructions for running the 3D figure generator and viewer.

---

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
- [ ] Clean up the code
- [x] Fix the box shape
- [ ] Document the rest of the code 

### Demo
![Demo](https://github.com/diogocsilva12/CG-Project/raw/main/Phase%202/usage_example.gif)



### Code Review
Code Review for Phase 2 - Geometric Transforms
After reviewing your code, I can confirm that your implementation properly supports the geometric transformations requirements for Phase 2. Here's an analysis:

Requirements Compliance
✅ Hierarchical Scene Structure: Your code correctly implements a tree structure where each node (Group) can contain:

Geometric transformations (translate, rotate, scale)
Models
Child groups
✅ Transformation Types: You properly handle all required transformations:

Translate (x, y, z)
Rotate (angle, x, y, z)
Scale (x, y, z)
✅ XML Parsing: Your parser correctly reads the transformation parameters from the XML file.

✅ Transformation Inheritance: Children groups inherit transformations from their parents through the OpenGL matrix stack (using glPushMatrix() and glPopMatrix()).

✅ Correct Transformation Order: Your renderGroup function now applies transformations in the correct order:

Translate
Rotate
Scale
This order ensures proper transformation application in OpenGL's matrix stack.

Specific Analysis
Your parseGroup function correctly:

Reads transformation parameters from XML
Stores them in the group's transform structure
Recursively processes child groups
Your renderGroup function correctly:

Uses glPushMatrix() to save the current transformation state
Applies the group's transformations in the proper order
Renders all models in the group
Recursively renders child groups
Uses glPopMatrix() to restore the transformation state
Recommendation
The only minor issue I see is in the XML parser - it should be able to handle syntax errors like the extra ">" character in your test XML file. However, since the XML file loads and renders correctly despite this issue, it doesn't affect functionality.

Overall, your code fully meets the requirements for Phase 2. The geometric transformations are properly implemented, and the hierarchical scene structure is correctly handled.





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
