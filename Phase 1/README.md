# Phase 1 Requirements:
## Generator: 

1. Generator Application
```
    // The main() function handles command line parameters exactly as specified:
    ./generator sphere 1 10 10 sphere.3d
    ./generator box 2 3 box.3d
    ./generator cone 1 2 4 3 cone.3d
    ./generator plane 1 3 plane.3d
```

2. Primitives Implementation
✓ Plane: Square in XZ plane, centered at origin
✓ Box: Centered at origin, with divisions
✓ Sphere: Centered at origin, with radius, slices, stacks
✓ Cone: Base on XZ plane, with radius, height, slices, stacks

3. File Format
```
    // Example output format (plane.3d):
    16              // Number of vertices
    -0.5 0 -0.5    // Vertex coordinates (x y z)
    -0.166667 0 -0.5
...
```

4. Parameter Handling: 
```
    // Each primitive correctly handles its required parameters:
    case 1: // plane: unit size, divisions
    case 2: // box: unit size, divisions
    case 3: // cone: radius, height, slices, stacks
    case 4: // sphere: radius, slices,
```
## Engine: 
1. XML Configuration
```
    // The XML file is correctly parsed and the scene is loaded:
    <scene>
        <model file="sphere.3d" />
        <model file="box.3d" />
        <model file="cone.3d" />
        <model file="plane.3d" />
    </scene>
```


2. Implementation Features:
- ✓ Reads XML configuration file
- ✓ Loads and renders 3D models from .3d files
- ✓ Configurable camera settings
- ✓ OpenGL wireframe rendering
- ✓ Window size configuration

3. Usage:
```bash
# From build directory
./engine ../engine/config.xml
```

4. Controls:
- Views models in wireframe mode
- Camera position set through XML
- Models rendered as triangles
- Multiple models can be displayed


## TODO:
- [ ] Fix some bugs
- [ ] Clean the code
- [ ] Add more camera controls
- [ ] Implement solid rendering mode

## How to Run

1. Build the project:
```bash
# From Phase 1 directory
mkdir build
cd build
cmake ..
make
```

2. Generate models:
```bash
# From build directory
./generator plane 2 8 plane.3d
```

3. Check generated files:
```bash
# Files are created in generator/tests/
ls ../generator/tests/
```

4. Test individual models:
- Modify config.xml to include the model you want to test
- Run the engine with the config file:
```bash
./engine ../engine/config.xml
```
or just run the engine with the model file (example):
```bash
./engine ../generator/tests/plane.3d
```
Note: Generated .3d files are saved in the `generator/tests/` directory.