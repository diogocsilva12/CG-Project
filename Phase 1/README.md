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
- [ ] Fix some bugs
- [ ] Clean up the code
- [ ] Fix the box shape
- [ ] Document the rest of the code 

### Demo
![Demo](https://github.com/diogocsilva12/CG-Project/raw/main/Phase%201/usage_example.gif)




