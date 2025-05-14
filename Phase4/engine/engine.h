#ifndef ENGINE_STRUCTS_H
#define ENGINE_STRUCTS_H

#include <vector>
#include <string>

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif

// Structure for a 3D point or vector
struct Point {
    float x, y, z;
};

// Structure for color components
struct Color {
    float r, g, b;
    
    Color() : r(200.0f/255.0f), g(200.0f/255.0f), b(200.0f/255.0f) {}  // Default: gray
};

// Structure for material properties
struct Material {
    Color diffuse;    // Default: (200, 200, 200)
    Color ambient;    // Default: (50, 50, 50)
    Color specular;   // Default: (0, 0, 0)
    Color emissive;   // Default: (0, 0, 0)
    float shininess;  // Default: 0
    
    Material() : shininess(0.0f) {
        ambient.r = 50.0f/255.0f;
        ambient.g = 50.0f/255.0f;
        ambient.b = 50.0f/255.0f;
    }
};

// Structure for a light source
struct Light {
    std::string type = "point";  // "point", "directional", or "spotlight"
    float posx = 0.0f, posy = 0.0f, posz = 0.0f;    // Position (for point and spot lights)
    float dirx = 0.0f, diry = 0.0f, dirz = 0.0f;    // Direction (for directional and spot lights)
    float cutoff = 45.0f;              // Cutoff angle in degrees (for spotlights)
    float intensity = 1.0f;           // Intensity of the light
};

// Structure for a 3D model
struct Model {
    std::string filename;
    std::string textureFile;
    GLuint vbo = 0;
    GLuint nbo = 0;  // Normal buffer object
    GLuint tbo = 0;  // Texture coordinate buffer object
    GLuint textureID = 0;
    std::vector<Point> vertices;
    std::vector<Point> normals;
    std::vector<float> texCoords;
    Material material;
};

// Structure for transformation information
struct Transform {
    // Translation
    float translateX = 0.0f, translateY = 0.0f, translateZ = 0.0f;
    
    // Rotation
    float rotateAngle = 0.0f, rotateX = 0.0f, rotateY = 0.0f, rotateZ = 0.0f;
    bool timeRotation = false;
    float rotationTime = 0.0f;
    
    // Scale
    float scaleX = 1.0f, scaleY = 1.0f, scaleZ = 1.0f;
    
    // Catmull-Rom curve
    bool hasCurve = false;
    std::vector<Point> curvePoints;
    float curveTime = 0.0f;
    bool align = false;
    bool drawCurve = true;
};

// Forward declaration of Group
struct Group;

// Structure for a group (contains models and child groups)
struct Group {
    Transform transform;
    std::vector<std::string> transformOrder;
    std::vector<Model> models;
    std::vector<Group> childGroups;
};

// Structure for camera information
struct Camera {
    Point position = {0.0f, 0.0f, 5.0f};
    Point lookAt = {0.0f, 0.0f, 0.0f};
    Point up = {0.0f, 1.0f, 0.0f};
    float fov = 60.0f;
    float near = 1.0f;
    float far = 1000.0f;
};

// Structure for window settings
struct Window {
    int width = 800;
    int height = 600;
};

// Structure for the entire world (root of the scene graph)
struct World {
    Camera camera;
    Window window;
    Group rootGroup;
    std::vector<Light> lights;
    std::string skyboxTexture; // Path to skybox texture
};

// Function declarations for Catmull-Rom splines
void getGlobalCatmullRomPoint(float gt, const std::vector<Point>& points, float* pos, float* deriv);
void cross(const float* a, const float* b, float* res);
void normalize(float* v);
void buildRotMatrix(const float* x, const float* y, const float* z, float* m);

#endif