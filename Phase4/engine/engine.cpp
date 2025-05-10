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
#include <sstream>


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

    std::vector<Point> vertices;
    std::vector<Point> normals;
    std::vector<float> texCoords;
    
    std::string line;
    int lineNum = 0;
    
    while (std::getline(file, line)) {
        ++lineNum;
        std::istringstream iss(line);
        
        // Try to read as position + normal + texture format
        Point vertex, normal;
        float u, v;
        
        if (iss >> vertex.x >> vertex.y >> vertex.z >> normal.x >> normal.y >> normal.z >> u >> v) {
            // Full format with position, normal, and texture
            vertices.push_back(vertex);
            normals.push_back(normal);
            texCoords.push_back(u);
            texCoords.push_back(v);
        } else {
            // Reset stream for retry
            iss.clear();
            iss.str(line);
            
            // Try to read as position + texture format
            if (iss >> vertex.x >> vertex.y >> vertex.z >> u >> v) {
                vertices.push_back(vertex);
                texCoords.push_back(u);
                texCoords.push_back(v);
            } else {
                // Reset stream for retry
                iss.clear();
                iss.str(line);
                
                // Try to read as position only format
                if (iss >> vertex.x >> vertex.y >> vertex.z) {
                    vertices.push_back(vertex);
                } else {
                    if (!file.eof()) {
                        std::cerr << "Malformed line " << lineNum << " in " << model.filename << std::endl;
                    }
                }
            }
        }
    }

    if (vertices.empty()) {
        std::cerr << "Warning: Model " << model.filename << " has no vertices!" << std::endl;
        return;
    }

    model.vertices = vertices;
    
    // Store normals if available
    if (!normals.empty()) {
        model.normals = normals;
    }
    
    // Store texture coordinates if available
    if (!texCoords.empty()) {
        model.texCoords = texCoords;
    }

    // Create and populate vertex buffer
    if (model.vbo == 0) glGenBuffers(1, &model.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, model.vbo);
    glBufferData(GL_ARRAY_BUFFER, model.vertices.size() * sizeof(Point), model.vertices.data(), GL_STATIC_DRAW);
    
    // Create and populate normal buffer if normals exist
    if (!model.normals.empty()) {
        if (model.nbo == 0) glGenBuffers(1, &model.nbo);
        glBindBuffer(GL_ARRAY_BUFFER, model.nbo);
        glBufferData(GL_ARRAY_BUFFER, model.normals.size() * sizeof(Point), model.normals.data(), GL_STATIC_DRAW);
    }
    
    // Create and populate texture coordinate buffer if texture coordinates exist
    if (!model.texCoords.empty()) {
        if (model.tbo == 0) glGenBuffers(1, &model.tbo);
        glBindBuffer(GL_ARRAY_BUFFER, model.tbo);
        glBufferData(GL_ARRAY_BUFFER, model.texCoords.size() * sizeof(float), model.texCoords.data(), GL_STATIC_DRAW);
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    // Load texture if specified
    if (!model.textureFile.empty()) {
        model.textureID = loadTexture(model.textureFile);
        if (model.textureID == 0) {
            std::cerr << "Failed to load texture: " << model.textureFile << std::endl;
        }
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
    
    // Save current lighting state
    GLboolean lightingEnabled = glIsEnabled(GL_LIGHTING);
    
    // Disable lighting for drawing curves
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    
    glColor3f(0.7f, 0.7f, 0.7f); // Light gray for trajectory
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < samples; ++i) {
        float t = (float)i / samples;
        float pos[3], deriv[3];
        getGlobalCatmullRomPoint(t, points, pos, deriv);
        glVertex3f(pos[0], pos[1], pos[2]);
    }
    glEnd();
    
    // Restore previous lighting state
    if (lightingEnabled) {
        glEnable(GL_LIGHTING);
    }
    
    glColor3f(1.0f, 1.0f, 1.0f); // Reset color
}

// Replace your existing setupLights function with this one

void setupLights(const std::vector<Light>& lights) {
    // Disable lighting first, then re-enable lights as needed
    glDisable(GL_LIGHTING);
    
    if (lights.empty()) {
        return; // No lights to setup
    }
    
    // Enable lighting
    glEnable(GL_LIGHTING);
    
    // Configure up to 8 lights (OpenGL limit)
    for (size_t i = 0; i < lights.size() && i < 8; i++) {
        GLenum lightID = GL_LIGHT0 + i;
        
        // Disable the light first
        glDisable(lightID);
        
        const Light& light = lights[i];
        
        // Define light colors (using white by default)
        float ambient[4] = {0.2f, 0.2f, 0.2f, 1.0f};  // Subtle ambient
        float diffuse[4] = {1.0f, 1.0f, 1.0f, 1.0f};  // Full diffuse
        float specular[4] = {1.0f, 1.0f, 1.0f, 1.0f}; // Full specular
        
        glLightfv(lightID, GL_AMBIENT, ambient);
        glLightfv(lightID, GL_DIFFUSE, diffuse);
        glLightfv(lightID, GL_SPECULAR, specular);
        
        if (light.type == "point") {
            // Point light
            float position[4] = {light.posx, light.posy, light.posz, 1.0f};  // w=1 for position
            glLightfv(lightID, GL_POSITION, position);
            
            // Default attenuation parameters
            glLightf(lightID, GL_CONSTANT_ATTENUATION, 1.0f);
            glLightf(lightID, GL_LINEAR_ATTENUATION, 0.0f);
            glLightf(lightID, GL_QUADRATIC_ATTENUATION, 0.0f);
        }
        else if (light.type == "directional") {
            // Directional light
            float direction[4] = {light.dirx, light.diry, light.dirz, 0.0f};  // w=0 for direction
            glLightfv(lightID, GL_POSITION, direction);  // In OpenGL, direction is stored in position with w=0
        }
        else if (light.type == "spotlight") {
            // Spotlight
            float position[4] = {light.posx, light.posy, light.posz, 1.0f};
            float direction[3] = {light.dirx, light.diry, light.dirz};
            
            glLightfv(lightID, GL_POSITION, position);
            glLightfv(lightID, GL_SPOT_DIRECTION, direction);
            glLightf(lightID, GL_SPOT_CUTOFF, light.cutoff);
            glLightf(lightID, GL_SPOT_EXPONENT, 2.0f);  // Controls spotlight focus
        }
        
        // Now enable the light
        glEnable(lightID);
    }
}

// Update your setupMaterial function to include debug output

void setupMaterial(const Material& material) {
    // Create material property arrays with alpha=1.0
    float diffuse[4] = {material.diffuse.r, material.diffuse.g, material.diffuse.b, 1.0f};
    float ambient[4] = {material.ambient.r, material.ambient.g, material.ambient.b, 1.0f};
    float specular[4] = {material.specular.r, material.specular.g, material.specular.b, 1.0f};
    float emissive[4] = {material.emissive.r, material.emissive.g, material.emissive.b, 1.0f};
    
    // Print more detailed debugging information
    static int materialCount = 0;
    if (materialCount < 10) {  // Increased to see more materials
        std::cout << "Setting material " << materialCount << ":" << std::endl;
        std::cout << "  Diffuse: (" << diffuse[0] << ", " << diffuse[1] << ", " << diffuse[2] << ")" << std::endl;
        
        // Check for common error conditions
        if (diffuse[0] > 1.0f || diffuse[1] > 1.0f || diffuse[2] > 1.0f) {
            std::cout << "  WARNING: Diffuse color components > 1.0 - these need to be normalized!" << std::endl;
        }
        if (diffuse[0] == 0.0f && diffuse[1] == 0.0f && diffuse[2] == 0.0f) {
            std::cout << "  WARNING: Diffuse color is black - check if colors are properly parsed!" << std::endl;
        }
        
        materialCount++;
    }
    
    // Apply material properties
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT, GL_EMISSION, emissive);
    glMaterialf(GL_FRONT, GL_SHININESS, material.shininess * 128.0f); // OpenGL expects 0-128 range
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
        // Disable color material before setting material properties
        glDisable(GL_COLOR_MATERIAL);
        
        // Setup material properties
        setupMaterial(model.material);
        
        // Setup texture if available
        if (model.textureID > 0) {
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, model.textureID);
        } else {
            glDisable(GL_TEXTURE_2D);
        }

        if (model.vbo != 0 && !model.vertices.empty()) {
            glBindBuffer(GL_ARRAY_BUFFER, model.vbo);
            glEnableClientState(GL_VERTEX_ARRAY);
            glVertexPointer(3, GL_FLOAT, sizeof(Point), 0);
            
            // Use normals if available
            if (model.nbo != 0 && !model.normals.empty()) {
                glBindBuffer(GL_ARRAY_BUFFER, model.nbo);
                glEnableClientState(GL_NORMAL_ARRAY);
                glNormalPointer(GL_FLOAT, sizeof(Point), 0);
            }
            
            // Use texture coordinates if available
            if (model.tbo != 0 && !model.texCoords.empty()) {
                glBindBuffer(GL_ARRAY_BUFFER, model.tbo);
                glEnableClientState(GL_TEXTURE_COORD_ARRAY);
                glTexCoordPointer(2, GL_FLOAT, 0, 0);
            }
            
            glDrawArrays(GL_TRIANGLES, 0, model.vertices.size());
            
            // Disable client states
            glDisableClientState(GL_VERTEX_ARRAY);
            if (!model.normals.empty()) glDisableClientState(GL_NORMAL_ARRAY);
            if (!model.texCoords.empty()) glDisableClientState(GL_TEXTURE_COORD_ARRAY);
            
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
    
    // Print which key was pressed (for debugging)
    std::cout << "Key pressed: '" << key << "' (ASCII: " << (int)key << ")" << std::endl;
    
    // For debug menu
    if (key == 'h' || key == 'H') {
        showDebugMenu = !showDebugMenu;
        std::cout << "Debug menu toggled: " << (showDebugMenu ? "ON" : "OFF") << std::endl;
    }
    
    // ESC key to quit
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
    // Save current states
    GLboolean depthEnabled = glIsEnabled(GL_DEPTH_TEST);
    GLboolean lightingEnabled = glIsEnabled(GL_LIGHTING);
    GLboolean textureEnabled = glIsEnabled(GL_TEXTURE_2D);
    
    // Save current matrices
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, world.window.width, 0, world.window.height, -1, 1);
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    // Disable features that could interfere with text rendering
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    
    // Set text color to white
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(x, y);
    
    // Render each character. For debug menu.
    for (const char* c = text; *c; c++) {
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *c);
    }
    
    // Restore settings
    if (depthEnabled) glEnable(GL_DEPTH_TEST);
    if (lightingEnabled) glEnable(GL_LIGHTING);
    if (textureEnabled) glEnable(GL_TEXTURE_2D);
    
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
    
    setupLights(world.lights);
    renderGroup(world.rootGroup);
    
    if (showDebugMenu) {
        char buffer[256]; // Increase buffer size to prevent overflow
        
        // Save OpenGL states that might affect text rendering
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        
        // Uses a buffer to store information on fps and "print" it on the screen with snprintf
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
        
        snprintf(buffer, sizeof(buffer), "Lights: %zu", world.lights.size());
        renderText(buffer, 10, world.window.height - 120);
        
        if (!world.lights.empty()) {
            const Light& light = world.lights[0];
            if (light.type == "point") {
                snprintf(buffer, sizeof(buffer), "Light 0: Point at (%.1f, %.1f, %.1f)", 
                        light.posx, light.posy, light.posz);
            } else if (light.type == "directional") {
                snprintf(buffer, sizeof(buffer), "Light 0: Directional (%.1f, %.1f, %.1f)", 
                        light.dirx, light.diry, light.dirz);
            } else if (light.type == "spotlight") {
                snprintf(buffer, sizeof(buffer), "Light 0: Spotlight at (%.1f, %.1f, %.1f) dir(%.1f, %.1f, %.1f) cutoff %.1f°", 
                        light.posx, light.posy, light.posz, light.dirx, light.diry, light.dirz, light.cutoff);
            }
            renderText(buffer, 10, world.window.height - 140);
        }

        // Add material information for the first model in the first group (for debugging)
        if (!world.rootGroup.models.empty()) {
            const Model& model = world.rootGroup.models[0];
            snprintf(buffer, sizeof(buffer), "First Model Material: Diff=(%.2f,%.2f,%.2f) Spec=(%.2f,%.2f,%.2f) Shin=%.1f", 
                    model.material.diffuse.r, model.material.diffuse.g, model.material.diffuse.b,
                    model.material.specular.r, model.material.specular.g, model.material.specular.b,
                    model.material.shininess);
            renderText(buffer, 10, world.window.height - 160);
        }
        
        renderText("Controls: WASD=Move QE=Up/Down IJKL=Rotate +-=Zoom R=Reset F=Toggle Wireframe H=Toggle Debug ESC=Quit",
                  10, 20);
                  
        // Restore OpenGL state
        if (glIsEnabled(GL_LIGHTING)) glEnable(GL_LIGHTING);
        if (glIsEnabled(GL_TEXTURE_2D)) glEnable(GL_TEXTURE_2D);
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

    // Initialize OpenGL lighting system (single clean setup)
    glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);  // This ensures normals are normalized
    glShadeModel(GL_SMOOTH); // Enable smooth shading

    // Set up global ambient light
    float globalAmbient[4] = {0.2f, 0.2f, 0.2f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);  // Lighting only on front faces
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);  // More accurate specular highlights

    // GL_COLOR_MATERIAL allows per-vertex color with lighting
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

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
    glEnable(GL_NORMALIZE);  // Normalize normals when scaling is applied
    glDepthFunc(GL_LESS);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    
    // GL_COLOR_MATERIAL allows per-vertex color with lighting
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    
    // Start in filled mode instead of wireframe for better visualization with lighting
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Add this in your OpenGL initialization
    glEnable(GL_TEXTURE_2D);

    // Initialize FPS timer variables
    lastTime = glutGet(GLUT_ELAPSED_TIME);
    frameCount = 0;

    glutMainLoop();

    return 0;
}