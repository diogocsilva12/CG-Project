#ifndef ENGINE_STRUCTS_H
#define ENGINE_STRUCTS_H

#include <vector>
#include <string>

struct Point {
    float x, y, z;
};

struct Camera {
    Point position;
    Point lookAt;
    Point up;
    float fov;
    float near;
    float far;
};

struct Window {
    int width;
    int height;
};

struct Model {
    std::string filename;
    std::vector<Point> vertices;
};

struct World {
    Window window;
    Camera camera;
    std::vector<Model> models;
};

#endif