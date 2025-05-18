#include "texture.h"

// Platform-specific OpenGL includes
#define GL_SILENCE_DEPRECATION
#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
// Use absolute paths to the DevIL headers
#include "/opt/homebrew/Cellar/devil/1.8.0_6/include/IL/il.h"
#include "/opt/homebrew/Cellar/devil/1.8.0_6/include/IL/ilu.h" // Add this for iluErrorString
#else
#include <GL/glut.h>
#include <GL/gl.h>
#include <IL/il.h>
#include <IL/ilu.h> // Add this for iluErrorString
#include <IL/ilut.h> // Add this for ilutGL
#endif



#include <iostream>
#include <fstream>

GLuint loadTexture(const std::string& filename) {
    std::cout << "Loading texture from: " << filename << std::endl;
    
    static bool devilInitialized = false;
    if (!devilInitialized) {
        std::cout << "Initializing DevIL..." << std::endl;
        ilInit();
        iluInit(); // Initialize ILU as well
        ilEnable(IL_ORIGIN_SET);
        ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
        devilInitialized = true;
        std::cout << "DevIL initialized successfully" << std::endl;
    }

    // Print DevIL version
    std::cout << "DevIL version: " << ilGetInteger(IL_VERSION_NUM) << std::endl;

    ILuint img;
    ilGenImages(1, &img);
    ilBindImage(img);

    // Check if file exists first
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Texture file not found: " << filename << std::endl;
        ilDeleteImages(1, &img);
        return 0;
    }
    file.close();

    // Try to load the image
    if (!ilLoadImage(filename.c_str())) {
        ILenum error = ilGetError();
        std::cerr << "DevIL failed to load image: " << filename << std::endl;
        std::cerr << "DevIL error code: " << error << std::endl;
        ilDeleteImages(1, &img);
        return 0;
    }

    std::cout << "Image loaded successfully, converting to RGBA format" << std::endl;
    ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

    int width = ilGetInteger(IL_IMAGE_WIDTH);
    int height = ilGetInteger(IL_IMAGE_HEIGHT);
    unsigned char* data = ilGetData();

    std::cout << "Image dimensions: " << width << "x" << height << std::endl;

    GLuint texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    std::cout << "Texture created with ID: " << texID << std::endl;

    ilDeleteImages(1, &img);
    return texID;
}