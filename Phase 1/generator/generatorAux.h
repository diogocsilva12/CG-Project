#ifndef GENERATORAUX_H
#define GENERATORAUX_H

#include <string>
#include <vector>

struct Vertex {
    float x;
    float y;
    float z;
};

void writeVertices(const std::string& filename, const std::vector<Vertex>& vertices);

void plane(float unit, int slices, const std::string& filename);

void box(float unit, int slices, const std::string& filename);

void cone(float radius, float height, int slices, int stacks, const std::string& filename);

void sphere(float radius, int slices, int stacks, const std::string& filename);

#endif // GENERATORAUX_H