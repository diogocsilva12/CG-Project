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
    // Escreve cada vértice numa nova linha
    for (const auto& v : vertices) {
        file << v.x << " " << v.y << " " << v.z << "\n";
    }
    file.close();
}


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


void sphere(float radius, int slices, int stacks, const std::string& filename) {
    if (radius <= 0 || slices <= 0 || stacks <= 0) {
        throw std::invalid_argument("Error: Radius, slices, and stacks must be positive.");
    }

    std::vector<Vertex> vertices;
    float arch_alfa = static_cast<float>(2 * M_PI) / slices;  // angulo entre slices
    float stack_size = static_cast<float>(M_PI) / stacks;      // Tamanho de cada stack

    for (int i = 0; i < stacks; i++) {
        float theta1 = static_cast<float>(i) * stack_size;     // angulo theta1
        float theta2 = static_cast<float>(i + 1) * stack_size; // angulo theta2

        for (int j = 0; j < slices; j++) {
            float phi1 = static_cast<float>(j) * arch_alfa;    // angulo phi1
            float phi2 = static_cast<float>(j + 1) * arch_alfa; // angulo phi2

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

void torus(float outerRadius, float innerRadius, int sides, int rings, const std::string& filename) {
    if (outerRadius <= 0 || innerRadius <= 0 || sides <= 0 || rings <= 0) {
        throw std::invalid_argument("Error: Outer radius, inner radius, sides, and rings must be positive.");
    }

    std::vector<Vertex> vertices;
    float ringRadius = (outerRadius - innerRadius) / 2;
    float torusRadius = innerRadius + ringRadius;

    // Generate vertices for the torus
    for (int i = 0; i < rings; i++) {
        float theta = 2.0f * M_PI * i / rings;
        float theta_next = 2.0f * M_PI * (i + 1) / rings;
        float cos_theta = cos(theta);
        float sin_theta = sin(theta);
        float cos_theta_next = cos(theta_next);
        float sin_theta_next = sin(theta_next);

        for (int j = 0; j < sides; j++) {
            float phi = 2.0f * M_PI * j / sides;
            float phi_next = 2.0f * M_PI * (j + 1) / sides;
            float cos_phi = cos(phi);
            float sin_phi = sin(phi);
            float cos_phi_next = cos(phi_next);
            float sin_phi_next = sin(phi_next);

            // Calculate vertices for the current segment
            Vertex v1 = {
                (torusRadius + ringRadius * cos_phi) * cos_theta,
                ringRadius * sin_phi,
                (torusRadius + ringRadius * cos_phi) * sin_theta
            };

            Vertex v2 = {
                (torusRadius + ringRadius * cos_phi_next) * cos_theta,
                ringRadius * sin_phi_next,
                (torusRadius + ringRadius * cos_phi) * sin_theta
            };

            Vertex v3 = {
                (torusRadius + ringRadius * cos_phi_next) * cos_theta_next,
                ringRadius * sin_phi_next,
                (torusRadius + ringRadius * cos_phi_next) * sin_theta_next
            };

            Vertex v4 = {
                (torusRadius + ringRadius * cos_phi) * cos_theta_next,
                ringRadius * sin_phi,
                (torusRadius + ringRadius * cos_phi) * sin_theta_next
            };

            // First triangle
            vertices.push_back(v1);
            vertices.push_back(v2);
            vertices.push_back(v3);

            // Second triangle
            vertices.push_back(v1);
            vertices.push_back(v3);
            vertices.push_back(v4);
        }
    }

    writeVertices(filename, vertices);
}

void circle(float radius, int segments, const std::string& filename) {
    if (radius <= 0 || segments < 3) {
        throw std::invalid_argument("Error: Radius must be positive and segments must be at least 3.");
    }

    std::vector<Vertex> vertices;
    float angleStep = 2.0f * M_PI / segments;

    // Generate a series of line segments forming a circle
    for (int i = 0; i < segments; i++) {
        float angle1 = i * angleStep;
        float angle2 = (i + 1) * angleStep;

        float x1 = radius * sin(angle1);
        float z1 = radius * cos(angle1);
        float x2 = radius * sin(angle2);
        float z2 = radius * cos(angle2);

        // Add two vertices for each line segment
        vertices.push_back({x1, 0.0f, z1});
        vertices.push_back({x2, 0.0f, z2});
    }

    writeVertices(filename, vertices);
}