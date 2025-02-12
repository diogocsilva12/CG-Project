#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <string>
#include "generatorAux.h"



void writeVertices(const std::string& filename, const std::vector<Vertex>& vertices) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Erro ao abrir o arquivo: " << filename << "\n";
        return;
    }
    // Write the number of vertices as the first line
    file << vertices.size() << "\n";
    // Write each vertex on a new line
    for (const auto& v : vertices) {
        file << v.x << " " << v.y << " " << v.z << "\n";
    }
    file.close();
}

void plane(float unit, int slices, const std::string& filename) {
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
    std::vector<Vertex> vertices;
    float half = unit / 2.0f;
    float step = unit / slices;

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
    writeVertices(filename, vertices);
}

void cone(float radius, float height, int slices, int stacks, const std::string& filename) {
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

void sphere(float radius, int slices, int stacks, const std::string& filename) {
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