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
    // Escreve cada vértice em uma nova linha
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
    float comp = unit / slices;
    float offset = unit / 2.0f;

    for (int i = 0; i < slices; i++) {
        for (int j = 0; j < slices; j++) {
            float x1 = i * comp - offset;
            float z1 = j * comp - offset;
            float x2 = (i + 1) * comp - offset;
            float z2 = (j + 1) * comp - offset;

            // Triângulo 1
            vertices.push_back({x1, 0.0f, z1});
            vertices.push_back({x2, 0.0f, z2});
            vertices.push_back({x2, 0.0f, z1});

            // Triângulo 2
            vertices.push_back({x1, 0.0f, z1});
            vertices.push_back({x1, 0.0f, z2});
            vertices.push_back({x2, 0.0f, z2});
        }
    }

    writeVertices(filename, vertices);
}

void box(float unit, int slices, const std::string& filename) {
    if (unit <= 0 || slices <= 0) {
        throw std::invalid_argument("Error: Unit and slices must be positive.");
    }

    std::vector<Vertex> vertices;
    float comp = unit / slices;
    float offset = unit / 2.0f;

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
    float arch_alfa = 2 * static_cast<float>(M_PI) / slices;
    float ratio = height / radius;
    float stack_size = height / stacks;

    // Base do cone
    for (int i = 0; i < slices; i++) {
        float x1 = radius * sin(arch_alfa * i);
        float z1 = radius * cos(arch_alfa * i);
        float x2 = radius * sin(arch_alfa * (i + 1));
        float z2 = radius * cos(arch_alfa * (i + 1));

        // Triângulo da base
        vertices.push_back({x1, 0.0f, z1});
        vertices.push_back({0.0f, 0.0f, 0.0f});
        vertices.push_back({x2, 0.0f, z2});
    }

    // Lateral do cone
    for (int i = 0; i < stacks; i++) {
        for (int j = 0; j < slices; j++) {
            float h1 = height - (i * stack_size);
            float h2 = height - ((i + 1) * stack_size);
            float r1 = h1 / ratio;
            float r2 = h2 / ratio;

            float x1 = r1 * sin(arch_alfa * j);
            float x2 = r1 * sin(arch_alfa * (j + 1));
            float x3 = r2 * sin(arch_alfa * (j + 1));
            float x4 = r2 * sin(arch_alfa * j);
            float y1 = i * stack_size;
            float y2 = (i + 1) * stack_size;
            float z1 = r1 * cos(arch_alfa * j);
            float z2 = r1 * cos(arch_alfa * (j + 1));
            float z3 = r2 * cos(arch_alfa * (j + 1));
            float z4 = r2 * cos(arch_alfa * j);

            // Triângulo 1
            vertices.push_back({x1, y1, z1});
            vertices.push_back({x2, y1, z2});
            vertices.push_back({x4, y2, z4});

            // Triângulo 2
            if (j != slices - 1) {
                vertices.push_back({x4, y2, z4});
                vertices.push_back({x2, y1, z2});
                vertices.push_back({x3, y2, z3});
            }
        }
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
    float arch_alfa = static_cast<float>(2 * M_PI) / slices;  // Ângulo entre fatias
    float stack_size = static_cast<float>(M_PI) / stacks;      // Tamanho de cada stack

    for (int i = 0; i < stacks; i++) {
        float theta1 = static_cast<float>(i) * stack_size;     // Ângulo theta1
        float theta2 = static_cast<float>(i + 1) * stack_size; // Ângulo theta2

        for (int j = 0; j < slices; j++) {
            float phi1 = static_cast<float>(j) * arch_alfa;    // Ângulo phi1
            float phi2 = static_cast<float>(j + 1) * arch_alfa; // Ângulo phi2

            // Vértices para formar dois triângulos
            Vertex v1 = {
                static_cast<float>(radius * sin(theta1) * cos(phi1)),
                static_cast<float>(radius * cos(theta1)),
                static_cast<float>(radius * sin(theta1) * sin(phi1))
            };

            Vertex v2 = {
                static_cast<float>(radius * sin(theta1) * cos(phi2)),
                static_cast<float>(radius * cos(theta1)),
                static_cast<float>(radius * sin(theta1) * sin(phi2))
            };

            Vertex v3 = {
                static_cast<float>(radius * sin(theta2) * cos(phi2)),
                static_cast<float>(radius * cos(theta2)),
                static_cast<float>(radius * sin(theta2) * sin(phi2))
            };

            Vertex v4 = {
                static_cast<float>(radius * sin(theta2) * cos(phi1)),
                static_cast<float>(radius * cos(theta2)),
                static_cast<float>(radius * sin(theta2) * sin(phi1))
            };

            // Triângulo 1
            vertices.push_back(v1);
            vertices.push_back(v2);
            vertices.push_back(v3);

            // Triângulo 2
            vertices.push_back(v1);
            vertices.push_back(v3);
            vertices.push_back(v4);
        }
    }

    writeVertices(filename, vertices);
}