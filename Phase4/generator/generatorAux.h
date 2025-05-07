#ifndef GENERATORAUX_H
#define GENERATORAUX_H

#include <string>
#include <vector>

struct Vertex {
    float x;
    float y;
    float z;
};

struct VertexTexture {
    float x, y, z;
    float u, v;
};

void writeVertices(const std::string& filename, const std::vector<Vertex>& vertices);

void writeVerticesWithTexture(const std::string& filename, const std::vector<Vertex>& vertices);

void plane(float unit, int slices, const std::string& filename);

void planeWithTexture(float unit, int slices, const std::string& filename);

void box(float unit, int slices, const std::string& filename);

void cone(float radius, float height, int slices, int stacks, const std::string& filename);

void sphere(float radius, int slices, int stacks, const std::string& filename);

void torus(float outerRadius, float innerRadius, int sides, int rings, const std::string& filename);

void circle(float radius, int segments, const std::string& filename);

/**
 * @brief Generate a Bezier surface model from control points
 * 
 * @param patchFile Path to the patch file with control points
 * @param tessellation Level of subdivision for the surface
 * @param outputFile Path to save the generated model
 */
void bezier(const std::string& patchFile, int tessellation, const std::string& outputFile);

#endif // GENERATORAUX_H