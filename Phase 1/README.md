This document provides an overview of the project, including how to build and run the generator and engine, as well as detailed usage instructions for `generator.cpp`.

---

## Table of Contents
1. [Project Overview](#project-overview)
2. [Build Instructions](#build-instructions)
3. [Program Usage](#program-usage) 
4. [TODO](#todo)
5. [Demo](#demo)

---

## Project Overview

This part 1 of the project consists of two main components:
1. **Generator**: A command-line tool to generate 3D primitive shapes (plane, box, cone, sphere) and save them in `.3d` files.
2. **Engine**: A 3D rendering engine that reads `.3d` files and renders them using OpenGL.

---

## Build Instructions

To build the project, follow these steps:

```bash
# From the Phase 1 directory
cd build
make clean
cmake ..
make
```
This will compile both the generator and engine executables.

## Program Usage
The generator program creates 3D primitive shapes and saves them in .3d files. Below are the usage instructions for generator.cpp:

- General usage:
```bash
./generator <shape> [parameters...] <output_file>
```

- Supported Shapes and Parameters:
1. Plane
```bash 
./generator plane <unit> <slices> <output_file>
```

- unit: Size of the plane.
- slices: Number of divisions.

Example: 
```bash
./generator plane 2 8 plane.3d
```

2. Box
```bash
./generator box <unit> <slices> <output_file>
```
- unit: Size of the box.
- slices: Number of divisions.
Example:
```bash
./generator box 2 3 box.3d
```

3. Cone
```bash
./generator cone <radius> <height> <slices> <stacks> <output_file>

```

- radius: Radius of the cone.
- height: Height of the cone.
- slices: Number of divisions around the cone.
- stacks: Number of divisions along the height of the cone.
Example:
```bash
./generator cone 1 2 4 3 cone.3d
```

4. Sphere
```bash
./generator sphere <radius> <slices> <stacks> <output_file>
```

- radius: Radius of the sphere.
- slices: Number of divisions around the sphere.
- stacks: Number of divisions along the height of the sphere.
Example:
```bash
./generator sphere 1 10 10 sphere.3d
```

Help Command
To display usage instructions:

```bash
Copy
./generator --help
# or
./generator -h
```

Notes
- Generated .3d files are saved in the generator/tests/ directory.
- Creating a .3d file will override any existing file with the same name.

Example Workflow 
1. Generate a plane:
```bash
./generator plane 2 8 plane.3d
```

2. Render the plane using the engine:
```bash
./engine ../engine/plane_config.xml
```
Note: The same applies for the other shapes.

### TODO 
- [ ] Fix some bugs
- [ ] Clean up the code
- [ ] Fix the box shape
- [ ] Document the rest of the code 

### Demo
![Demo](https://github.com/diogocsilva12/CG-Project/raw/main/Phase%201/usage_example.gif)




