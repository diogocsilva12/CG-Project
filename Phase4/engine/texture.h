#ifndef TEXTURE_H
#define TEXTURE_H

// Platform-specific GL includes
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <string>

// Load a texture from a file and return the OpenGL texture ID
unsigned int loadTexture(const std::string& filename);

#endif // TEXTURE_H