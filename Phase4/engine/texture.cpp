#include <IL/il.h>
#include <GL/gl.h>
#include <iostream>
#include <string>

GLuint loadTexture(const std::string& filename) {
    static bool devilInitialized = false;
    if (!devilInitialized) {
        ilInit();
        ilEnable(IL_ORIGIN_SET);
        ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
        devilInitialized = true;
    }

    ILuint img;
    ilGenImages(1, &img);
    ilBindImage(img);

    if (!ilLoadImage(filename.c_str())) {
        std::cerr << "DevIL failed to load image: " << filename << std::endl;
        ilDeleteImages(1, &img);
        return 0;
    }

    ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

    int width = ilGetInteger(IL_IMAGE_WIDTH);
    int height = ilGetInteger(IL_IMAGE_HEIGHT);
    unsigned char* data = ilGetData();

    GLuint texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    ilDeleteImages(1, &img);
    return texID;
}