#ifndef ENGINE_STRUCTS_H
#define ENGINE_STRUCTS_H

#include <vector>
#include <string>

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif

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
    std::string textureFile;
    std::vector<Point> vertices;
    GLuint vbo = 0; // <-- Add this line for VBO support
    GLuint textureID = 0; // Texture ID for OpenGL
};

struct Transform {
    // Translation values
    float translateX = 0.0f;
    float translateY = 0.0f;
    float translateZ = 0.0f;
    
    // Rotation values (in degrees)
    float rotateAngle = 0.0f;
    float rotateX = 0.0f;
    float rotateY = 0.0f;
    float rotateZ = 0.0f;
    
    // Scale values
    float scaleX = 1.0f;
    float scaleY = 1.0f;
    float scaleZ = 1.0f;

    // Catmull-Rom curve support
    bool hasCurve = false;
    float curveTime = 0.0f;
    bool align = false;
    std::vector<Point> curvePoints;

    // Time-based rotation
    bool timeRotation = false;
    float rotationTime = 0.0f;
    bool drawCurve = true;
};

struct Group {
    std::vector<Model> models;
    std::vector<Group> childGroups;
    Transform transform;
    std::vector<std::string> transformOrder; // Store the order of transformations
};

struct Light {
    std::string type;
    float posx = 0.0f;
    float posy = 0.0f;
    float posz = 0.0f;
    // Adiciona outros atributos se necessário (ex: direção, cutoff, etc.)
};

struct World {
    Window window;
    Camera camera;
    Group rootGroup; // Replace the models vector with a hierarchical structure
    std::vector<Light> lights;
};

#endif