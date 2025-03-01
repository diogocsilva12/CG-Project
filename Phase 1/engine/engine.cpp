/**
 * @file engine.cpp
 * @brief Main rendering engine for 3D graphics application
 * 
 * This file implements the core rendering engine that processes 3D models
 * defined in XML configuration files, and renders them using OpenGL.
 */

// Suppress deprecation warnings on macOS
#define GL_SILENCE_DEPRECATION

// Platform-specific OpenGL includes
#ifdef __APPLE__
#include <GLUT/glut.h>
#elif __linux__
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#else 
#include <GL/glut.h>
#endif

#include "engine.h"
#include "xmlParser.h" 
#include <fstream>
#include <iostream>
#include <cmath>
 
/**
 * @brief Global world state containing all scene information
 * 
 * This includes models, camera configuration, and window settings
 * loaded from the XML configuration file.
 */
World world;

/**
 * @brief Loads a 3D model from a file into memory
 * 
 * Reads vertex data from the specified file and stores it in the model's
 * vertices vector. Each vertex consists of x, y, z coordinates.
 * 
 * @param model Reference to the Model object to populate with vertex data
 */
void loadModel(Model& model) {
    std::ifstream file(model.filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << model.filename << std::endl;
        return;
    }

    Point vertex;
    while (file >> vertex.x >> vertex.y >> vertex.z) {
        model.vertices.push_back(vertex);
    }
}

/**
 * @brief Main rendering function called by GLUT for each frame
 * 
 * Clears the screen, sets up the camera view based on world configuration,
 * and renders all models in wireframe mode using GL_TRIANGLES.
 */
void renderScene() {
    // Clear the color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reset the modelview matrix
    glLoadIdentity();
    
    // Set up camera position, look-at point, and up vector
    gluLookAt(world.camera.position.x, world.camera.position.y, world.camera.position.z,
              world.camera.lookAt.x, world.camera.lookAt.y, world.camera.lookAt.z,
              world.camera.up.x, world.camera.up.y, world.camera.up.z);

    // Draw each model listed in the XML
    for (const Model& model : world.models) {
        glBegin(GL_TRIANGLES);  // Drawing triangles for 3D models
        const std::vector<Point>& vertices = model.vertices;
        
        // Calculate slices from vertex count (for debugging purposes)
        int slices = sqrt(vertices.size()) - 1;  

        // Process vertices in groups of 3 to form triangles
        for (size_t i = 0; i < vertices.size(); i += 3) {
            glVertex3f(vertices[i].x, vertices[i].y, vertices[i].z);
            glVertex3f(vertices[i + 1].x, vertices[i + 1].y, vertices[i + 1].z);
            glVertex3f(vertices[i + 2].x, vertices[i + 2].y, vertices[i + 2].z);
        }
        glEnd();
    }

    // Swap front and back buffers (double buffering)
    glutSwapBuffers();
}

/**
 * @brief Window resize callback function
 * 
 * Handles viewport and projection matrix updates when the window is resized.
 * Maintains the correct aspect ratio for the perspective projection.
 * 
 * @param w New window width in pixels
 * @param h New window height in pixels
 */
void changeSize(int w, int h) {
    // Prevent division by zero
    if (h == 0) h = 1;
    
    // Calculate the new aspect ratio
    float ratio = w * 1.0f / h;

    // Set up the projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, w, h);
    
    // Set up perspective projection using parameters from XML
    gluPerspective(world.camera.fov, ratio, world.camera.near, world.camera.far);
    
    // Return to modelview matrix for rendering
    glMatrixMode(GL_MODELVIEW);
}

/**
 * @brief Main function - program entry point
 * 
 * Initializes GLUT, loads XML configuration and model data,
 * sets up rendering parameters, and starts the main rendering loop.
 * 
 * @param argc Command line argument count
 * @param argv Command line argument values
 * @return 0 on successful execution, 1 on error
 */
int main(int argc, char** argv) {
    // Check command line arguments
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <configs/config.xml>" << std::endl;
        return 1;
    }

    // Parse XML and load world configuration
    world = parseXMLFile(argv[1]);

    // Load all model files referenced in the XML
    for (Model& model : world.models) {
        loadModel(model);
    }

    // Initialize GLUT
    glutInit(&argc, argv);
    
    // Set display mode (depth testing, double buffering, RGB color)
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    
    // Set initial window position and size from XML configuration
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(world.window.width, world.window.height);
    
    // Create the rendering window
    glutCreateWindow("CG@Fase 1 - G18");

    // Register callback functions
    glutDisplayFunc(renderScene);  // For rendering frames
    glutReshapeFunc(changeSize);   // For window resizing

    // Enable depth testing to properly handle occlusion
    glEnable(GL_DEPTH_TEST);
    
    // Enable back-face culling for improved performance
    glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LESS);       // Depth comparison function
    glCullFace(GL_BACK);        // Cull back faces
    glFrontFace(GL_CCW);        // Define front faces as counter-clockwise
    
    // Use wireframe rendering mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Enter GLUT's main event processing loop
    // (This will handle rendering and user input until program exit)
    glutMainLoop();

    return 0;
}