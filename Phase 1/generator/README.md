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


Parameter explanation:

Plane: larger size (2) and more divisions (8) for a detailed grid
Box: size 2 with 4 divisions makes each face nicely subdivided
Cone: radius 1, height 2 for good proportions, 16 slices for smooth circular base, 8 stacks for smooth sides
Sphere: radius 1 with 16 slices and 8 stacks gives a smooth spherical shape
You can test each shape individually by modifying your config.xml to include just one model at a time.