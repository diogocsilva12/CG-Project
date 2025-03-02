/**
 * @file engine.cpp
 * @brief Main rendering engine for 3D graphics application
 * 
 * This file implements the core rendering engine that processes 3D models
 * defined in XML configuration files, and renders them using OpenGL.
 */

#define GL_SILENCE_DEPRECATION
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
    //Limpa a cor e o buffer 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    
    //Posiciona a camara na cena
    gluLookAt(world.camera.position.x, world.camera.position.y, world.camera.position.z,
              world.camera.lookAt.x, world.camera.lookAt.y, world.camera.lookAt.z,
              world.camera.up.x, world.camera.up.y, world.camera.up.z);

    //Desenha o modelo do XML 
    for (const Model& model : world.models) {
        glBegin(GL_TRIANGLES);
        const std::vector<Point>& vertices = model.vertices;
        
        //Calcula o número de slices para debug
        int slices = sqrt(vertices.size()) - 1;  

        //Desenho dos triângulos
        for (size_t i = 0; i < vertices.size(); i += 3) {
            glVertex3f(vertices[i].x, vertices[i].y, vertices[i].z);
            glVertex3f(vertices[i + 1].x, vertices[i + 1].y, vertices[i + 1].z);
            glVertex3f(vertices[i + 2].x, vertices[i + 2].y, vertices[i + 2].z);
        }
        glEnd();
    }
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
    if (h == 0) h = 1;
    float ratio = w * 1.0f / h;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, w, h);
    
    gluPerspective(world.camera.fov, ratio, world.camera.near, world.camera.far);
    
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
    //Operação controlo erros
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <configs/config.xml>" << std::endl;
        return 1;
    }

    // Faz parsing do ficheiro XML e carrega as configurações da cena
    world = parseXMLFile(argv[1]);

    //Carrega os modelos do ficheiro XML
    for (Model& model : world.models) {
        loadModel(model);
    }

    glutInit(&argc, argv);
    
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(world.window.width, world.window.height);
    
    glutCreateWindow("CG@Fase 1 - G18");

    glutDisplayFunc(renderScene); 
    glutReshapeFunc(changeSize);  

    glEnable(GL_DEPTH_TEST);
    
   
    glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LESS);       
    glCullFace(GL_BACK);        
    glFrontFace(GL_CCW); 
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


    glutMainLoop();

    return 0;
}