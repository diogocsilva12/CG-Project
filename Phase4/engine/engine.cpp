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
#include <OpenGL/gl3.h>
#elif __linux__
#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#else 
#include <GL/glut.h>
#include <GL/glew.h>
#endif

#include "engine.h"
#include "texture.h"
#include "xmlParser.h" 
#include "catmullrom.h" 
#include <fstream>
#include <iostream>
#include <cmath>
#include <vector>
#include <limits>


float cameraSpeed = 0.5f;  
float rotationSpeed = 2.0f;    
bool keys[256] = {false}; // Array to track key presses

//Variable for camera orbiting
float camRadius = 0.0f;        
float camAlpha = 0.0f;         // Horizontal angle
float camBeta = 0.0f;          // Vertical angle

//Mouse control Variables 
int mouseX = 0, mouseY = 0;         
bool mousePressed = false;          // Left-side mouse pressing -> moves the camera
bool rightMousePressed = false;     // Right-side mouse pressing -> zooms in/out
float mouseSensitivity = 0.2f;      

//Variables for debug menu
bool showDebugMenu = false;      //Boolean to check if debug menu is shown
int frameCount = 0;              
float fps = 0.0f;                
int lastTime = 0;                //Fps tracker


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
    int lineNum = 0;
    while (file) {
        ++lineNum;
        if (!(file >> vertex.x >> vertex.y >> vertex.z)) {
            if (!file.eof()) {
                std::cerr << "Malformed line " << lineNum << " in " << model.filename << std::endl;
                file.clear();
                file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
            continue;
        }
        model.vertices.push_back(vertex);
    }

    if (model.vertices.empty()) {
        std::cerr << "Warning: Model " << model.filename << " has no vertices!" << std::endl;
        return;
    }

    if (model.vbo == 0) glGenBuffers(1, &model.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, model.vbo);
    glBufferData(GL_ARRAY_BUFFER, model.vertices.size() * sizeof(Point), model.vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    if (!model.textureFile.empty()) {
        model.textureID = loadTexture(model.textureFile);
    }
}

/**
 * @brief Draws a Catmull-Rom curve
 * 
 * Renders the trajectory of a Catmull-Rom curve using OpenGL line loops.
 * 
 * @param points The control points of the curve
 * @param samples Number of samples to draw the curve
 */
void drawCatmullRomCurve(const std::vector<Point>& points, int samples = 100) {
    if (points.size() < 4) return;
    //glDisable(GL_LIGHTING); // If using lighting, disable for lines
    glColor3f(0.7f, 0.7f, 0.7f); // Light gray for trajectory
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < samples; ++i) {
        float t = (float)i / samples;
        float pos[3], deriv[3];
        getGlobalCatmullRomPoint(t, points, pos, deriv);
        glVertex3f(pos[0], pos[1], pos[2]);
    }
    glEnd();
    glColor3f(1,1,1); // Reset color
}

//Render groups with transformations. Applies transformations in the order specified in the XML file.
void renderGroup(const Group& group) {
    // Só desenha a trajetória se drawCurve for true
    if (group.transform.hasCurve && group.transform.curvePoints.size() >= 4 && group.transform.drawCurve) {
        drawCatmullRomCurve(group.transform.curvePoints);
    }

    glPushMatrix();
    for (const auto& transform : group.transformOrder) {
        if (transform == "translate") {
            glTranslatef(group.transform.translateX, group.transform.translateY, group.transform.translateZ);
        }
        else if (transform == "translate_curve") {
            float t = 0.0f;
            if (group.transform.curveTime > 0.0f) {
                float elapsed = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
                t = fmod(elapsed / group.transform.curveTime, 1.0f);
            }
            float pos[3], deriv[3];
            getGlobalCatmullRomPoint(t, group.transform.curvePoints, pos, deriv);
            glTranslatef(pos[0], pos[1], pos[2]);
            if (group.transform.align) {
                float up[3] = {0, 1, 0}, side[3], m[16];
                cross(deriv, up, side);
                normalize(deriv); normalize(side);
                cross(side, deriv, up);
                buildRotMatrix(deriv, up, side, m);
                glMultMatrixf(m);
            }
        }
        else if (transform == "rotate") {
            if (group.transform.rotateAngle != 0.0f)
                glRotatef(group.transform.rotateAngle, group.transform.rotateX, group.transform.rotateY, group.transform.rotateZ);
        }
        else if (transform == "rotate_time") {
            float elapsed = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
            float angle = fmod(elapsed / group.transform.rotationTime, 1.0f) * 360.0f;
            glRotatef(angle, group.transform.rotateX, group.transform.rotateY, group.transform.rotateZ);
        }
        else if (transform == "scale") {
            glScalef(group.transform.scaleX, group.transform.scaleY, group.transform.scaleZ);
        }
    }
    
    //Renders all models in group.models
    for (const Model& model : group.models) {
        if (model.textureID) {
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, model.textureID);
            // glEnableClientState(GL_TEXTURE_COORD_ARRAY); // se usares VBO para texcoords
            // glTexCoordPointer(...); // se usares VBO para texcoords
        } else {
            glDisable(GL_TEXTURE_2D);
        }

        if (model.vbo != 0 && !model.vertices.empty()) {
            glBindBuffer(GL_ARRAY_BUFFER, model.vbo);
            glEnableClientState(GL_VERTEX_ARRAY);
            glVertexPointer(3, GL_FLOAT, sizeof(Point), 0);
            glDrawArrays(GL_TRIANGLES, 0, model.vertices.size());
            glDisableClientState(GL_VERTEX_ARRAY);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
    }
    
    //Render the child models in xml
    for (const Group& childGroup : group.childGroups) {
        renderGroup(childGroup);
    }
    glPopMatrix(); //Reset the transformation matrix
}

//Update camera position based on orbit angles
void updateOrbit() {
    //Convert angles to radians
    float alphaRad = camAlpha * M_PI / 180.0f;
    float betaRad = camBeta * M_PI / 180.0f;
    
    //Calculate new camera position based on angles and radius
    world.camera.position.x = world.camera.lookAt.x + camRadius * cos(betaRad) * cos(alphaRad);
    world.camera.position.y = world.camera.lookAt.y + camRadius * sin(betaRad);
    world.camera.position.z = world.camera.lookAt.z + camRadius * cos(betaRad) * sin(alphaRad);
}

//Init camera at the starting values
void initCameraAngles() {
    //Calculate the camera radius
    float dx = world.camera.position.x - world.camera.lookAt.x;
    float dy = world.camera.position.y - world.camera.lookAt.y;
    float dz = world.camera.position.z - world.camera.lookAt.z;
    camRadius = sqrt(dx*dx + dy*dy + dz*dz);
    
    //Calculate the initial angles
    camBeta = asin((world.camera.position.y - world.camera.lookAt.y) / camRadius) * 180.0f / M_PI;
    camAlpha = atan2(world.camera.position.z - world.camera.lookAt.z, 
                     world.camera.position.x - world.camera.lookAt.x) * 180.0f / M_PI;
}

//This updates camera position based on keys and orbit angles
void updateCamera() {
    float dirX = world.camera.lookAt.x - world.camera.position.x;
    float dirY = world.camera.lookAt.y - world.camera.position.y;
    float dirZ = world.camera.lookAt.z - world.camera.position.z;
    
    float length = sqrt(dirX*dirX + dirY*dirY + dirZ*dirZ);
    dirX /= length;
    dirY /= length;
    dirZ /= length;
    
    float rightX = dirZ * world.camera.up.y - dirY * world.camera.up.z;
    float rightY = dirX * world.camera.up.z - dirZ * world.camera.up.x;
    float rightZ = dirY * world.camera.up.x - dirX * world.camera.up.y;
    
    length = sqrt(rightX*rightX + rightY*rightY + rightZ*rightZ);
    rightX /= length;
    rightY /= length;
    rightZ /= length;
    
    //Handle key presses for movement
    if (keys['w'] || keys['W']) {  
        world.camera.position.x += dirX * cameraSpeed;
        world.camera.position.y += dirY * cameraSpeed;
        world.camera.position.z += dirZ * cameraSpeed;
        
        world.camera.lookAt.x += dirX * cameraSpeed;
        world.camera.lookAt.y += dirY * cameraSpeed;
        world.camera.lookAt.z += dirZ * cameraSpeed;
    }
    
    if (keys['s'] || keys['S']) {  
        world.camera.position.x -= dirX * cameraSpeed;
        world.camera.position.y -= dirY * cameraSpeed;
        world.camera.position.z -= dirZ * cameraSpeed;
        
        world.camera.lookAt.x -= dirX * cameraSpeed;
        world.camera.lookAt.y -= dirY * cameraSpeed;
        world.camera.lookAt.z -= dirZ * cameraSpeed;
    }
    
    if (keys['a'] || keys['A']) {  
        world.camera.position.x -= rightX * cameraSpeed;
        world.camera.position.y -= rightY * cameraSpeed;
        world.camera.position.z -= rightZ * cameraSpeed;
        
        world.camera.lookAt.x -= rightX * cameraSpeed;
        world.camera.lookAt.y -= rightY * cameraSpeed;
        world.camera.lookAt.z -= rightZ * cameraSpeed;
    }
    
    if (keys['d'] || keys['D']) {  
        world.camera.position.x += rightX * cameraSpeed;
        world.camera.position.y += rightY * cameraSpeed;
        world.camera.position.z += rightZ * cameraSpeed;
        
        world.camera.lookAt.x += rightX * cameraSpeed;
        world.camera.lookAt.y += rightY * cameraSpeed;
        world.camera.lookAt.z += rightZ * cameraSpeed;
    }
    
    if (keys['q'] || keys['Q']) {  
        world.camera.position.x += world.camera.up.x * cameraSpeed;
        world.camera.position.y += world.camera.up.y * cameraSpeed;
        world.camera.position.z += world.camera.up.z * cameraSpeed;
        
        world.camera.lookAt.x += world.camera.up.x * cameraSpeed;
        world.camera.lookAt.y += world.camera.up.y * cameraSpeed;
        world.camera.lookAt.z += world.camera.up.z * cameraSpeed;
    }
    
    if (keys['e'] || keys['E']) {  
        world.camera.position.x -= world.camera.up.x * cameraSpeed;
        world.camera.position.y -= world.camera.up.y * cameraSpeed;
        world.camera.position.z -= world.camera.up.z * cameraSpeed;
        
        world.camera.lookAt.x -= world.camera.up.x * cameraSpeed;
        world.camera.lookAt.y -= world.camera.up.y * cameraSpeed;
        world.camera.lookAt.z -= world.camera.up.z * cameraSpeed;
    }
    
    //Zoom in/out 
    if (keys['+'] || keys['=']) {  
        camRadius *= 0.95; 
        updateOrbit();
    }
    
    if (keys['-'] || keys['_']) { 
        camRadius *= 1.05;  
        updateOrbit();
    }
    
    if (keys['j'] || keys['J']) {
        camAlpha -= rotationSpeed;
        updateOrbit();
    }
    
    if (keys['l'] || keys['L']) { 
        camAlpha += rotationSpeed;
        updateOrbit();
    }
    
    if (keys['i'] || keys['I']) {  
        camBeta += rotationSpeed;
        if (camBeta > 89.0f) camBeta = 89.0f;
        updateOrbit();
    }
    
    if (keys['k'] || keys['K']) { 
        camBeta -= rotationSpeed;
        if (camBeta < -89.0f) camBeta = -89.0f;
        updateOrbit();
    }
    
    //Reset camera values
    if (keys['r'] || keys['R']) {
        world.camera.position.x = 20;
        world.camera.position.y = 25;
        world.camera.position.z = 20;
        world.camera.lookAt.x = 0;
        world.camera.lookAt.y = 0;
        world.camera.lookAt.z = 0;
        
        initCameraAngles();
    }
    
   //Toggle wireframe mode, default -> wireframe
    static bool wireframeMode = true;  
    static bool togglePressed = false;
    if (keys['f'] || keys['F']) {
        if (!togglePressed) {
            wireframeMode = !wireframeMode;
            if (wireframeMode) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            } else {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
            togglePressed = true;
        }
    } else {
        togglePressed = false;
    }
}

//Mouse implementation
void mouseButton(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            mousePressed = true;
            mouseX = x;
            mouseY = y;
        }
        else if (state == GLUT_UP) {
            mousePressed = false;
        }
    }
    else if (button == GLUT_RIGHT_BUTTON) {
        if (state == GLUT_DOWN) {
            rightMousePressed = true;
            mouseX = x;
            mouseY = y;
        }
        else if (state == GLUT_UP) {
            rightMousePressed = false;
        }
    }
}

void mouseMotion(int x, int y) {
   //left mouse button pressed
    if (mousePressed) {
        int deltaX = x - mouseX;
        int deltaY = y - mouseY;
        camAlpha += deltaX * mouseSensitivity;
        camBeta += deltaY * mouseSensitivity;
        if (camBeta > 89.0f) camBeta = 89.0f;
        if (camBeta < -89.0f) camBeta = -89.0f;
        updateOrbit();
    }
    //right mouse button pressed
    else if (rightMousePressed) {
        int deltaY = y - mouseY;
        camRadius += deltaY * mouseSensitivity * 0.5f;         
        if (camRadius < 5.0f) camRadius = 5.0f;
        if (camRadius > 500.0f) camRadius = 500.0f;         
        updateOrbit();
    }
    
    mouseX = x;
    mouseY = y;
    
    glutPostRedisplay();
}

void keyPressed(unsigned char key, int x, int y) {
    keys[key] = true;
    
    //For debug menu
    if (key == 'h' || key == 'H') {
        showDebugMenu = !showDebugMenu;
    }
    //ESC key to quit
    if (key == 27) 
        exit(0);
    glutPostRedisplay();
}


void keyReleased(unsigned char key, int x, int y) {
    keys[key] = false;
}

 /**
 * @brief Renders text in 2D screen space
 * 
 * Uses GLUT bitmap characters to display text at the specified 
 * screen coordinates. Temporarily switches to orthographic projection.
 * 
 * @param text The text string to display
 * @param x X coordinate in screen space (pixels from left)
 * @param y Y coordinate in screen space (pixels from bottom)
 */
void renderText(const char* text, float x, float y) {
    // Save current matrices
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, world.window.width, 0, world.window.height, -1, 1);
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    // Disable depth testing for text
    glDisable(GL_DEPTH_TEST);
    
    // Set text color to white
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(x, y);
    
    // Render each character. For debug menu.
    for (const char* c = text; *c; c++) {
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *c);
    }
    
    // Restore settings
    glEnable(GL_DEPTH_TEST);
    
    // Restore matrices
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

/**
 * @brief Recursively counts models in the scene graph
 * 
 * @param group The group to count models in
 * @param count Reference to the counter variable
 */
void countModels(const Group& group, int& count) {
    count += group.models.size();
    for (const Group& childGroup : group.childGroups) {
        countModels(childGroup, count);
    }
}

void renderScene() {
    frameCount++;
    int currentTime = glutGet(GLUT_ELAPSED_TIME);
    int timeInterval = currentTime - lastTime;
    //update fps every second
    if (timeInterval >= 1000) { 
        fps = frameCount * 1000.0f / timeInterval;
        lastTime = currentTime;
        frameCount = 0;
    }
    updateCamera();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    
    gluLookAt(world.camera.position.x, world.camera.position.y, world.camera.position.z,
              world.camera.lookAt.x, world.camera.lookAt.y, world.camera.lookAt.z,
              world.camera.up.x, world.camera.up.y, world.camera.up.z);
    
    renderGroup(world.rootGroup);
    
    if (showDebugMenu) {
        char buffer[128];
        
        //uses a buffer to store information o fps and "print" it on the screen with snprintf
        snprintf(buffer, sizeof(buffer), "FPS: %.1f", fps);
        renderText(buffer, 10, world.window.height - 20);
        
        snprintf(buffer, sizeof(buffer), "Camera Position: (%.2f, %.2f, %.2f)",
                world.camera.position.x, world.camera.position.y, world.camera.position.z);
        renderText(buffer, 10, world.window.height - 40);
     
        snprintf(buffer, sizeof(buffer), "Look At: (%.2f, %.2f, %.2f)",
                world.camera.lookAt.x, world.camera.lookAt.y, world.camera.lookAt.z);
        renderText(buffer, 10, world.window.height - 60);
       
        snprintf(buffer, sizeof(buffer), "Orbit: Radius=%.2f Alpha=%.2f Beta=%.2f",
                camRadius, camAlpha, camBeta);
        renderText(buffer, 10, world.window.height - 80);
        
       
        int totalModels = 0;
        countModels(world.rootGroup, totalModels);
        snprintf(buffer, sizeof(buffer), "Total Models: %d", totalModels);
        renderText(buffer, 10, world.window.height - 100);
        
        
        renderText("Controls: WASD=Move QE=Up/Down IJKL=Rotate +-=Zoom R=Reset F=Toggle Wireframe H=Toggle Debug ESC=Quit",
                  10, 20);
    }
    
    glutSwapBuffers();
}

void loadModels(Group& group) {
    for (Model& model : group.models) {
        loadModel(model);
    }
    
    for (Group& childGroup : group.childGroups) {
        loadModels(childGroup);
    }
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

void idleFunction() {
    glutPostRedisplay();
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
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <configs/config.xml>" << std::endl;
        return 1;
    }

    world = parseXMLFile(argv[1]);
    initCameraAngles();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(world.window.width, world.window.height);
    glutCreateWindow("CG@Fase 2 - G18");

    //glewExperimental = GL_TRUE;
    
#ifndef __APPLE__
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        std::cerr << "GLEW init failed: " << glewGetErrorString(err) << std::endl;
        return 1;
    }
#endif

    loadModels(world.rootGroup);

    glutDisplayFunc(renderScene);
    glutReshapeFunc(changeSize);
    
    //Keyboard controls
    glutKeyboardFunc(keyPressed);
    glutKeyboardUpFunc(keyReleased);
    
    //Mouse controls
    glutMouseFunc(mouseButton);
    glutMotionFunc(mouseMotion);
    
    glutIdleFunc(idleFunction);
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LESS);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    
    // Start in wireframe mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Initialize FPS timer variables
    lastTime = glutGet(GLUT_ELAPSED_TIME);
    frameCount = 0;

    glutMainLoop();

    return 0;
}