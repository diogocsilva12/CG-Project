## Command Format
```bash
./generator <type> <parameters> <outputfile>
```
```
    # From the generator directory
    # Format: ./generator <type> <parameters> <outputfile>

    # Plane: size 2 units, 8 divisions
    ./generator plane 2 8 plane.3d

    # Box: size 2 units, 4 divisions per edge
    ./generator box 2 4 box.3d

    # Cone: radius 1, height 2, 16 slices, 8 stacks
    ./generator cone 1 2 16 8 cone.3d

    # Sphere: radius 1, 16 slices, 8 stacks
    ./generator sphere 1 16 8 sphere.3d
```

## Parameter Explanation
- **Plane**: larger size (2) and more divisions (8) for a detailed grid
- **Box**: size 2 with 4 divisions makes each face nicely subdivided
- **Cone**: radius 1, height 2 for good proportions, 16 slices for smooth circular base, 8 stacks for smooth sides
- **Sphere**: radius 1 with 16 slices and 8 stacks gives a smooth spherical shape

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

