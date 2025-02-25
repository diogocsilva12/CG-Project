#define GL_SILENCE_DEPRECATION
#ifdef __APPLE__
#include <GLUT/glut.h>
#elif __linux__
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#else 
#include <GL/glut.h>
#include <GL/glew.h>
#endif
#include "engine.h"
#include "xmlParser.h" 
#include <fstream>
#include <iostream>
#include <cmath>
 

World world;  // Global world state




void loadModel(Model& model) {
    std::ifstream file(model.filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << model.filename << std::endl;
        return;
    }

    Point vertex;
    std::vector<float> vertexData;  // Store vertices contiguously
    while (file >> vertex.x >> vertex.y >> vertex.z) {
        vertexData.push_back(vertex.x);
        vertexData.push_back(vertex.y);
        vertexData.push_back(vertex.z);
    }
    model.vertexCount = vertexData.size() / 3;

    // Generate and bind VBO
    glGenBuffers(1, &model.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, model.vbo);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);
}

void renderScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();
    gluLookAt(world.camera.position.x, world.camera.position.y, world.camera.position.z,
              world.camera.lookAt.x, world.camera.lookAt.y, world.camera.lookAt.z,
              world.camera.up.x, world.camera.up.y, world.camera.up.z);

    // Draw each model using VBOs
    for (const Model& model : world.models) {
        glBindBuffer(GL_ARRAY_BUFFER, model.vbo);
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, 0);
        glDrawArrays(GL_TRIANGLES, 0, model.vertexCount);
        glDisableClientState(GL_VERTEX_ARRAY);
    }

    glutSwapBuffers();
}

void changeSize(int w, int h) {
    if (h == 0) h = 1;
    float ratio = w * 1.0f / h;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, w, h);
    gluPerspective(world.camera.fov, ratio, world.camera.near, world.camera.far);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <configs/config.xml>" << std::endl;
        return 1;
    }

    // Parse XML and load world
    world = parseXMLFile(argv[1]);

    // Load all models
    for (Model& model : world.models) {
        loadModel(model);
    }

    // Init GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(world.window.width, world.window.height);
    glutCreateWindow("CG@DI-UM");

    // Callback registration
    glutDisplayFunc(renderScene);
    glutReshapeFunc(changeSize);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);      // Enable back-face culling
    glDepthFunc(GL_LESS); 
    glCullFace(GL_BACK);         // Cull back faces
    glFrontFace(GL_CCW);         // Define front faces as counter-clockwise
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  // Show wireframe

    // Enter GLUT's main loop

    // Enter GLUT's main loop
    glutMainLoop();

    return 0;
}