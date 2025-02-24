#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <string>
#include <stdexcept> // For std::invalid_argument
#include "generatorAux.h"

/**
 * Writes a list of vertices to a file.
 * The first line of the file contains the number of vertices, followed by the coordinates of each vertex.
 *
 * @param filename The name of the file to write the vertices to.
 * @param vertices The list of vertices to write.
 * @throws std::runtime_error If the file cannot be opened.
 */
void writeVertices(const std::string& filename, const std::vector<Vertex>& vertices) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Error: Unable to open file " + filename);
    }
    // Write the number of vertices as the first line
    file << vertices.size() << "\n";
    // Write each vertex on a new line
    for (const auto& v : vertices) {
        file << v.x << " " << v.y << " " << v.z << "\n";
    }
    file.close();
}

/**
 * Generates vertices for a plane in the XZ plane, centered at the origin.
 *
 * @param unit The size of the plane (length of each side).
 * @param slices The number of divisions along each axis.
 * @param filename The name of the file to save the vertices.
 * @throws std::invalid_argument If unit or slices are non-positive.
 */
void plane(float unit, int slices, const std::string& filename) {
    if (unit <= 0 || slices <= 0) {
        throw std::invalid_argument("Error: Unit and slices must be positive.");
    }

    std::vector<Vertex> vertices;
    float half = unit / 2.0f;
    float step = unit / slices;

    // Generate vertices in the XZ plane (Y = 0) for a square centered at the origin
    for (int i = 0; i <= slices; i++) {
        for (int j = 0; j <= slices; j++) {
            Vertex v;
            v.x = -half + j * step;
            v.y = 0.0f;
            v.z = -half + i * step;
            vertices.push_back(v);
        }
    }
    writeVertices(filename, vertices);
}

void box(float unit, int slices, const std::string& filename) {
    if (unit <= 0 || slices <= 0) {
        throw std::invalid_argument("Error: Unit and slices must be positive.");
    }

    std::vector<Vertex> vertices;
    float half = unit / 2.0f;
    float step = unit / slices;

<<<<<<< Updated upstream
    // Front face (z = half) - plane in XY
    for (int i = 0; i <= slices; i++) {
        for (int j = 0; j <= slices; j++) {
            Vertex v;
            v.x = -half + j * step;
            v.y = -half + i * step;
            v.z = half;
            vertices.push_back(v);
        }
    }
    // Back face (z = -half) - plane in XY
    for (int i = 0; i <= slices; i++) {
        for (int j = 0; j <= slices; j++) {
            Vertex v;
            v.x = -half + j * step;
            v.y = -half + i * step;
            v.z = -half;
            vertices.push_back(v);
        }
    }
    // Left face (x = -half) - plane in YZ
    for (int i = 0; i <= slices; i++) {
        for (int j = 0; j <= slices; j++) {
            Vertex v;
            v.x = -half;
            v.y = -half + i * step;
            v.z = -half + j * step;
            vertices.push_back(v);
        }
    }
    // Right face (x = half) - plane in YZ
    for (int i = 0; i <= slices; i++) {
        for (int j = 0; j <= slices; j++) {
            Vertex v;
            v.x = half;
            v.y = -half + i * step;
            v.z = -half + j * step;
            vertices.push_back(v);
        }
    }
    // Top face (y = half) - plane in XZ
    for (int i = 0; i <= slices; i++) {
        for (int j = 0; j <= slices; j++) {
            Vertex v;
            v.x = -half + j * step;
            v.y = half;
            v.z = -half + i * step;
            vertices.push_back(v);
        }
    }
    // Bottom face (y = -half) - plane in XZ
    for (int i = 0; i <= slices; i++) {
        for (int j = 0; j <= slices; j++) {
            Vertex v;
            v.x = -half + j * step;
            v.y = -half;
            v.z = -half + i * step;
            vertices.push_back(v);
        }
    }
=======
    // Front face (Z = offset)
    for (int i = 0; i < slices; i++) {
        for (int j = 0; j < slices; j++) {
            float x1 = i * comp - offset;
            float z1 = j * comp - offset;
            float x2 = (i + 1) * comp - offset;
            float z2 = (j + 1) * comp - offset;

            // Triangle 1
            vertices.push_back({x1, offset, z1});
            vertices.push_back({x2, offset, z2});
            vertices.push_back({x2, offset, z1});

            // Triangle 2
            vertices.push_back({x1, offset, z1});
            vertices.push_back({x1, offset, z2});
            vertices.push_back({x2, offset, z2});
        }
    }

    // Back face (Z = -offset)
    for (int i = 0; i < slices; i++) {
        for (int j = 0; j < slices; j++) {
            float x1 = i * comp - offset;
            float z1 = j * comp - offset;
            float x2 = (i + 1) * comp - offset;
            float z2 = (j + 1) * comp - offset;

            // Triangle 1
            vertices.push_back({x2, -offset, z2});
            vertices.push_back({x1, -offset, z1});
            vertices.push_back({x2, -offset, z1});

            // Triangle 2
            vertices.push_back({x1, -offset, z2});
            vertices.push_back({x1, -offset, z1});
            vertices.push_back({x2, -offset, z2});
        }
    }

    // Right face (X = offset)
    for (int i = 0; i < slices; i++) {
        for (int j = 0; j < slices; j++) {
            float x1 = i * comp - offset;
            float y1 = j * comp - offset;
            float x2 = (i + 1) * comp - offset;
            float y2 = (j + 1) * comp - offset;

            // Triangle 1
            vertices.push_back({x2, y2, offset});
            vertices.push_back({x1, y1, offset});
            vertices.push_back({x2, y1, offset});

            // Triangle 2
            vertices.push_back({x1, y2, offset});
            vertices.push_back({x1, y1, offset});
            vertices.push_back({x2, y2, offset});
        }
    }

    // Left face (X = -offset)
    for (int i = 0; i < slices; i++) {
        for (int j = 0; j < slices; j++) {
            float x1 = i * comp - offset;
            float y1 = j * comp - offset;
            float x2 = (i + 1) * comp - offset;
            float y2 = (j + 1) * comp - offset;

            // Triangle 1
            vertices.push_back({x1, y1, -offset});
            vertices.push_back({x2, y2, -offset});
            vertices.push_back({x2, y1, -offset});

            // Triangle 2
            vertices.push_back({x1, y1, -offset});
            vertices.push_back({x1, y2, -offset});
            vertices.push_back({x2, y2, -offset});
        }
    }

    // Top face (Y = offset)
    for (int i = 0; i < slices; i++) {
        for (int j = 0; j < slices; j++) {
            float z1 = i * comp - offset;
            float y1 = j * comp - offset;
            float z2 = (i + 1) * comp - offset;
            float y2 = (j + 1) * comp - offset;

            // Triangle 1
            vertices.push_back({offset, y1, z1});
            vertices.push_back({offset, y2, z2});
            vertices.push_back({offset, y1, z2});

            // Triangle 2
            vertices.push_back({offset, y1, z1});
            vertices.push_back({offset, y2, z1});
            vertices.push_back({offset, y2, z2});
        }
    }

    // Bottom face (Y = -offset)
    for (int i = 0; i < slices; i++) {
        for (int j = 0; j < slices; j++) {
            float z1 = i * comp - offset;
            float y1 = j * comp - offset;
            float z2 = (i + 1) * comp - offset;
            float y2 = (j + 1) * comp - offset;

            // Triangle 1
            vertices.push_back({-offset, y2, z2});
            vertices.push_back({-offset, y1, z1});
            vertices.push_back({-offset, y1, z2});

            // Triangle 2
            vertices.push_back({-offset, y2, z1});
            vertices.push_back({-offset, y1, z1});
            vertices.push_back({-offset, y2, z2});
        }
    }

>>>>>>> Stashed changes
    writeVertices(filename, vertices);
}

/**
 * Generates vertices for a cone with a circular base and a pointed apex.
 *
 * @param radius The radius of the base of the cone.
 * @param height The height of the cone.
 * @param slices The number of divisions around the circumference.
 * @param stacks The number of divisions along the height.
 * @param filename The name of the file to save the vertices.
 * @throws std::invalid_argument If radius, height, slices, or stacks are non-positive.
 */
void cone(float radius, float height, int slices, int stacks, const std::string& filename) {
    if (radius <= 0 || height <= 0 || slices <= 0 || stacks <= 0) {
        throw std::invalid_argument("Error: Radius, height, slices, and stacks must be positive.");
    }

    std::vector<Vertex> vertices;

    // Lateral Surface: generate vertices for each (stack, slice)
    for (int i = 0; i <= stacks; i++) {
        float t = static_cast<float>(i) / stacks;
        float y = t * height;
        // Linearly decrease radius from base to apex
        float r_current = radius * (1 - t);
        for (int j = 0; j <= slices; j++) {
            float theta = 2 * M_PI * j / slices;
            Vertex v;
            v.x = r_current * cos(theta);
            v.y = y;
            v.z = r_current * sin(theta);
            vertices.push_back(v);
        }
    }

    // Base: generate center and circumference vertices
    Vertex center = {0.0f, 0.0f, 0.0f};
    vertices.push_back(center);
    for (int j = 0; j <= slices; j++) {
        float theta = 2 * M_PI * j / slices;
        Vertex v;
        v.x = radius * cos(theta);
        v.y = 0.0f;
        v.z = radius * sin(theta);
        vertices.push_back(v);
    }
    writeVertices(filename, vertices);
}

/**
 * Generates vertices for a sphere using spherical coordinates.
 *
 * @param radius The radius of the sphere.
 * @param slices The number of divisions around the equator.
 * @param stacks The number of divisions from pole to pole.
 * @param filename The name of the file to save the vertices.
 * @throws std::invalid_argument If radius, slices, or stacks are non-positive.
 */
void sphere(float radius, int slices, int stacks, const std::string& filename) {
    if (radius <= 0 || slices <= 0 || stacks <= 0) {
        throw std::invalid_argument("Error: Radius, slices, and stacks must be positive.");
    }

    std::vector<Vertex> vertices;
    // Generate vertices using spherical coordinates
    for (int i = 0; i <= stacks; i++) {
        float theta = M_PI * i / stacks;  // theta from 0 to PI
        for (int j = 0; j <= slices; j++) {
            float phi = 2 * M_PI * j / slices;  // phi from 0 to 2PI
            Vertex v;
            v.x = radius * sin(theta) * cos(phi);
            v.y = radius * cos(theta);
            v.z = radius * sin(theta) * sin(phi);
            vertices.push_back(v);
        }
    }
    writeVertices(filename, vertices);
}