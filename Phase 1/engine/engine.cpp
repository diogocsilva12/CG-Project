#define GL_SILENCE_DEPRECATION
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include "engine.h"
#include "xmlParser.h" 
#include <fstream>
#include <iostream>

World world;  // Global world state

void loadModel(Model& model) {
    std::ifstream file(model.filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << model.filename << std::endl;
        return;
    }

    int numVertices;
    file >> numVertices;

    for (int i = 0; i < numVertices; i++) {
        Point vertex;
        file >> vertex.x >> vertex.y >> vertex.z;
        model.vertices.push_back(vertex);
    }
}

void renderScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();
    gluLookAt(world.camera.position.x, world.camera.position.y, world.camera.position.z,
              world.camera.lookAt.x, world.camera.lookAt.y, world.camera.lookAt.z,
              world.camera.up.x, world.camera.up.y, world.camera.up.z);

    // Draw each model listed in the XML
    for (const Model& model : world.models) {
        glBegin(GL_TRIANGLES);  // Changed from GL_POINTS to GL_TRIANGLES
        const std::vector<Point>& vertices = model.vertices;
        int slices = sqrt(vertices.size()) - 1;  // Calculate slices from vertex count

        // For each grid cell, create two triangles
        for (int i = 0; i < slices; i++) {
            for (int j = 0; j < slices; j++) {
                // First triangle
                Point v1 = vertices[i * (slices + 1) + j];
                Point v2 = vertices[i * (slices + 1) + j + 1];
                Point v3 = vertices[(i + 1) * (slices + 1) + j];
                
                glVertex3f(v1.x, v1.y, v1.z);
                glVertex3f(v2.x, v2.y, v2.z);
                glVertex3f(v3.x, v3.y, v3.z);

                // Second triangle
                Point v4 = vertices[(i + 1) * (slices + 1) + j];
                Point v5 = vertices[i * (slices + 1) + j + 1];
                Point v6 = vertices[(i + 1) * (slices + 1) + j + 1];
                
                glVertex3f(v4.x, v4.y, v4.z);
                glVertex3f(v5.x, v5.y, v5.z);
                glVertex3f(v6.x, v6.y, v6.z);
            }
        }
        glEnd();
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
        std::cout << "Usage: " << argv[0] << " <config.xml>" << std::endl;
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

    // OpenGL settings
    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  // Show wireframe

    // Enter GLUT's main loop
    glutMainLoop();

    return 0;
}