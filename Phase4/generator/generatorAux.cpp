#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <string>
#include <stdexcept>
#include <array>
#include <sstream>
#include "generatorAux.h"

//-- MATRIX OPERATIONS --
//Define the 4x4 matrix type
typedef std::array<std::array<float, 4>, 4> Matrix4x4;

// Matrix operations --
Matrix4x4 identityMatrix() {
    return {{
        {{1.0f, 0.0f, 0.0f, 0.0f}},
        {{0.0f, 1.0f, 0.0f, 0.0f}},
        {{0.0f, 0.0f, 1.0f, 0.0f}},
        {{0.0f, 0.0f, 0.0f, 1.0f}}
    }};
}

//Translation matrices
Matrix4x4 translationMatrix(float x, float y, float z) {
    Matrix4x4 m = identityMatrix();
    m[0][3] = x;
    m[1][3] = y;
    m[2][3] = z;
    return m;
}

//Scale matrices
Matrix4x4 scaleMatrix(float x, float y, float z) {
    Matrix4x4 m = identityMatrix();
    m[0][0] = x;
    m[1][1] = y;
    m[2][2] = z;
    return m;
}

//Rotation matrices X Axis
Matrix4x4 rotationMatrixX(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    Matrix4x4 m = identityMatrix();
    m[1][1] = c;
    m[1][2] = -s;
    m[2][1] = s;
    m[2][2] = c;
    return m;
}

//Rotation matrices Y Axis
Matrix4x4 rotationMatrixY(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    Matrix4x4 m = identityMatrix();
    m[0][0] = c;
    m[0][2] = s;
    m[2][0] = -s;
    m[2][2] = c;
    return m;
}

//Rotation matrices Z Axis
Matrix4x4 rotationMatrixZ(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    Matrix4x4 m = identityMatrix();
    m[0][0] = c;
    m[0][1] = -s;
    m[1][0] = s;
    m[1][1] = c;
    return m;
}

// Matrix multiplication
Matrix4x4 multiplyMatrices(const Matrix4x4& a, const Matrix4x4& b) {
    Matrix4x4 result = {{}};
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            result[i][j] = 0;
            for (int k = 0; k < 4; k++) {
                result[i][j] += a[i][k] * b[k][j];
            }
        }
    }
    return result;
}

//-- HELPER FUNCTIONS --
// Cross product of two vectors
void crossProd(float *a, float *b, float *res) {
    res[0] = a[1]*b[2] - a[2]*b[1];
    res[1] = a[2]*b[0] - a[0]*b[2];
    res[2] = a[0]*b[1] - a[1]*b[0];
}

// Normalize a vector
void normalize(float *a) {
    float l = sqrt(a[0]*a[0] + a[1]*a[1] + a[2]*a[2]);
    if (l != 0) {
        a[0] = a[0]/l;
        a[1] = a[1]/l;
        a[2] = a[2]/l;
    }
}

// Matrix-vector multiplication
void multMatrixVector(float m[4][4], float *v, float *res) {
    for (int j = 0; j < 4; ++j) {
        res[j] = 0;
        for (int k = 0; k < 4; ++k) {
            res[j] += v[k] * m[j][k];
        }
    }
}

// Matrix-matrix multiplication
void multMatrixMatrix(float a[4][4], float b[4][4], float res[4][4]) {
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++) {
            res[i][j] = 0;
            for (int k = 0; k < 4; k++)
                res[i][j] += a[i][k] * b[k][j];
        }
}

//-- VERTEX TRANSFORMATION --
// Apply a transformation to a vertex
Vertex transformVertex(const Vertex& v, const Matrix4x4& m) {
    float x = v.x, y = v.y, z = v.z, w = 1.0f;
    
    float newX = m[0][0] * x + m[0][1] * y + m[0][2] * z + m[0][3] * w;
    float newY = m[1][0] * x + m[1][1] * y + m[1][2] * z + m[1][3] * w;
    float newZ = m[2][0] * x + m[2][1] * y + m[2][2] * z + m[2][3] * w;
    float newW = m[3][0] * x + m[3][1] * y + m[3][2] * z + m[3][3] * w;
    
    if (newW != 0 && newW != 1) {
        newX /= newW;
        newY /= newW;
        newZ /= newW;
    }
    
    return {newX, newY, newZ};
}

// Writes vertices to a file
void writeVertices(const std::string& filename, const std::vector<Vertex>& vertices) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Error: Unable to open file " + filename);
    }
    
    for (const auto& v : vertices) {
        file << v.x << " " << v.y << " " << v.z << "\n";
    }
    file.close();
}

// Structure for vertex with normal and texture coordinates
struct VertexFull {
    float x, y, z;         // Position
    float nx, ny, nz;      // Normal
    float u, v;            // Texture coordinates
};

// Writes vertices with normals and texture coordinates to a file
void writeVerticesFull(const std::string& filename, const std::vector<VertexFull>& vertices) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Error: Unable to open file " + filename);
    }
    
    for (const auto& v : vertices) {
        file << v.x << " " << v.y << " " << v.z << " "
             << v.nx << " " << v.ny << " " << v.nz << " "
             << v.u << " " << v.v << "\n";
    }
    file.close();
}

//-- PRIMITIVES POINTS GENERATION --
// Generates points for a plane with normals and texture coordinates
void plane(float unit, int slices, const std::string& filename) {
    if (unit <= 0 || slices <= 0) {
        throw std::invalid_argument("Error: Unit and slices must be positive.");
    }

    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Error: Unable to open file " + filename);
    }

    float comp = unit / slices;
    float offset = unit / 2.0f;
    float x1, x2, z1, z2;
    float text = 1.0f / slices;

    for (int i = 0; i < slices; i++) {
        for (int j = 0; j < slices; j++) {
            x1 = i * comp - offset;
            z1 = j * comp - offset;
            x2 = (i+1) * comp - offset;
            z2 = (j+1) * comp - offset;

            // Triangle 1
            file << x1 << ' ' << 0.0f << ' ' << z1 << ' ';
            file << 0.0f << ' ' << 1.0f << ' ' << 0.0f << ' ';
            file << i*text << ' ' << j*text << '\n';

            file << x2 << ' ' << 0.0f << ' ' << z2 << ' ';
            file << 0.0f << ' ' << 1.0f << ' ' << 0.0f << ' ';
            file << (i+1)*text << ' ' << (j+1)*text << '\n';

            file << x2 << ' ' << 0.0f << ' ' << z1 << ' ';
            file << 0.0f << ' ' << 1.0f << ' ' << 0.0f << ' ';
            file << (i+1)*text << ' ' << j*text << '\n';

            // Triangle 2
            file << x1 << ' ' << 0.0f << ' ' << z1 << ' ';
            file << 0.0f << ' ' << 1.0f << ' ' << 0.0f << ' ';
            file << i*text << ' ' << j*text << '\n';

            file << x1 << ' ' << 0.0f << ' ' << z2 << ' ';
            file << 0.0f << ' ' << 1.0f << ' ' << 0.0f << ' ';
            file << i*text << ' ' << (j+1)*text << '\n';

            file << x2 << ' ' << 0.0f << ' ' << z2 << ' ';
            file << 0.0f << ' ' << 1.0f << ' ' << 0.0f << ' ';
            file << (i+1)*text << ' ' << (j+1)*text << '\n';
        }
    }
    
    file.close();
}

// Generates points for a box with normals and texture coordinates
void box(float unit, int slices, const std::string& filename) {
    if (unit <= 0 || slices <= 0) {
        throw std::invalid_argument("Error: Unit and slices must be positive.");
    }

    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Error: Unable to open file " + filename);
    }

    float comp = unit / slices;
    float offset = unit / 2.0f;
    float x1, x2, y1, y2, z1, z2, tx1, tx2, tz1, tz2;
    float text = 1.0f / slices;

    // Top and bottom faces (Y = +offset and Y = -offset)
    for (int i = 0; i < slices; i++) {
        for (int j = 0; j < slices; j++) {
            x1 = i * comp - offset;
            z1 = j * comp - offset;
            x2 = (i+1) * comp - offset;
            z2 = (j+1) * comp - offset;
            tx1 = i*text;
            tx2 = (i+1)*text;
            tz1 = j*text;
            tz2 = (j+1)*text;

            // Top (+Y)
            file << x1 << ' ' << offset << ' ' << z1 << ' ';
            file << 0.0f << ' ' << 1.0f << ' ' << 0.0f << ' ';
            file << tx1 << ' ' << tz1 << '\n';

            file << x2 << ' ' << offset << ' ' << z2 << ' ';
            file << 0.0f << ' ' << 1.0f << ' ' << 0.0f << ' ';
            file << tx2 << ' ' << tz2 << '\n';

            file << x2 << ' ' << offset << ' ' << z1 << ' ';
            file << 0.0f << ' ' << 1.0f << ' ' << 0.0f << ' ';
            file << tx2 << ' ' << tz1 << '\n';

            file << x1 << ' ' << offset << ' ' << z1 << ' ';
            file << 0.0f << ' ' << 1.0f << ' ' << 0.0f << ' ';
            file << tx1 << ' ' << tz1 << '\n';

            file << x1 << ' ' << offset << ' ' << z2 << ' ';
            file << 0.0f << ' ' << 1.0f << ' ' << 0.0f << ' ';
            file << tx1 << ' ' << tz2 << '\n';

            file << x2 << ' ' << offset << ' ' << z2 << ' ';
            file << 0.0f << ' ' << 1.0f << ' ' << 0.0f << ' ';
            file << tx2 << ' ' << tz2 << '\n';

            // Bottom (-Y)
            file << x2 << ' ' << -offset << ' ' << z2 << ' ';
            file << 0.0f << ' ' << -1.0f << ' ' << 0.0f << ' ';
            file << tx2 << ' ' << tz2 << '\n';

            file << x1 << ' ' << -offset << ' ' << z1 << ' ';
            file << 0.0f << ' ' << -1.0f << ' ' << 0.0f << ' ';
            file << tx1 << ' ' << tz1 << '\n';

            file << x2 << ' ' << -offset << ' ' << z1 << ' ';
            file << 0.0f << ' ' << -1.0f << ' ' << 0.0f << ' ';
            file << tx2 << ' ' << tz1 << '\n';

            file << x1 << ' ' << -offset << ' ' << z2 << ' ';
            file << 0.0f << ' ' << -1.0f << ' ' << 0.0f << ' ';
            file << tx1 << ' ' << tz2 << '\n';

            file << x1 << ' ' << -offset << ' ' << z1 << ' ';
            file << 0.0f << ' ' << -1.0f << ' ' << 0.0f << ' ';
            file << tx1 << ' ' << tz1 << '\n';

            file << x2 << ' ' << -offset << ' ' << z2 << ' ';
            file << 0.0f << ' ' << -1.0f << ' ' << 0.0f << ' ';
            file << tx2 << ' ' << tz2 << '\n';
        }
    }

    // Front and back faces (Z = +offset and Z = -offset)
    for (int i = 0; i < slices; i++) {
        for (int j = 0; j < slices; j++) {
            x1 = i * comp - offset;
            y1 = j * comp - offset;
            x2 = (i+1) * comp - offset;
            y2 = (j+1) * comp - offset;
            tx1 = i*text;
            tx2 = (i+1)*text;
            tz1 = j*text;
            tz2 = (j+1)*text;

            // Front (+Z)
            file << x2 << ' ' << y2 << ' ' << offset << ' ';
            file << 0.0f << ' ' << 0.0f << ' ' << 1.0f << ' ';
            file << tx2 << ' ' << tz2 << '\n';

            file << x1 << ' ' << y1 << ' ' << offset << ' ';
            file << 0.0f << ' ' << 0.0f << ' ' << 1.0f << ' ';
            file << tx1 << ' ' << tz1 << '\n';

            file << x2 << ' ' << y1 << ' ' << offset << ' ';
            file << 0.0f << ' ' << 0.0f << ' ' << 1.0f << ' ';
            file << tx2 << ' ' << tz1 << '\n';

            file << x1 << ' ' << y2 << ' ' << offset << ' ';
            file << 0.0f << ' ' << 0.0f << ' ' << 1.0f << ' ';
            file << tx1 << ' ' << tz2 << '\n';

            file << x1 << ' ' << y1 << ' ' << offset << ' ';
            file << 0.0f << ' ' << 0.0f << ' ' << 1.0f << ' ';
            file << tx1 << ' ' << tz1 << '\n';

            file << x2 << ' ' << y2 << ' ' << offset << ' ';
            file << 0.0f << ' ' << 0.0f << ' ' << 1.0f << ' ';
            file << tx2 << ' ' << tz2 << '\n';

            // Back (-Z)
            file << x1 << ' ' << y1 << ' ' << -offset << ' ';
            file << 0.0f << ' ' << 0.0f << ' ' << -1.0f << ' ';
            file << tx1 << ' ' << tz1 << '\n';

            file << x2 << ' ' << y2 << ' ' << -offset << ' ';
            file << 0.0f << ' ' << 0.0f << ' ' << -1.0f << ' ';
            file << tx2 << ' ' << tz2 << '\n';

            file << x2 << ' ' << y1 << ' ' << -offset << ' ';
            file << 0.0f << ' ' << 0.0f << ' ' << -1.0f << ' ';
            file << tx2 << ' ' << tz1 << '\n';

            file << x1 << ' ' << y1 << ' ' << -offset << ' ';
            file << 0.0f << ' ' << 0.0f << ' ' << -1.0f << ' ';
            file << tx1 << ' ' << tz1 << '\n';

            file << x1 << ' ' << y2 << ' ' << -offset << ' ';
            file << 0.0f << ' ' << 0.0f << ' ' << -1.0f << ' ';
            file << tx1 << ' ' << tz2 << '\n';

            file << x2 << ' ' << y2 << ' ' << -offset << ' ';
            file << 0.0f << ' ' << 0.0f << ' ' << -1.0f << ' ';
            file << tx2 << ' ' << tz2 << '\n';
        }
    }

    // Right and left faces (X = +offset and X = -offset)
    for (int i = 0; i < slices; i++) {
        for (int j = 0; j < slices; j++) {
            z1 = i * comp - offset;
            y1 = j * comp - offset;
            z2 = (i+1) * comp - offset;
            y2 = (j+1) * comp - offset;
            tx1 = i*text;
            tx2 = (i+1)*text;
            tz1 = j*text;
            tz2 = (j+1)*text;

            // Right (+X)
            file << offset << ' ' << y1 << ' ' << z1 << ' ';
            file << 1.0f << ' ' << 0.0f << ' ' << 0.0f << ' ';
            file << tx1 << ' ' << tz1 << '\n';

            file << offset << ' ' << y2 << ' ' << z2 << ' ';
            file << 1.0f << ' ' << 0.0f << ' ' << 0.0f << ' ';
            file << tx2 << ' ' << tz2 << '\n';

            file << offset << ' ' << y1 << ' ' << z2 << ' ';
            file << 1.0f << ' ' << 0.0f << ' ' << 0.0f << ' ';
            file << tx2 << ' ' << tz1 << '\n';

            file << offset << ' ' << y1 << ' ' << z1 << ' ';
            file << 1.0f << ' ' << 0.0f << ' ' << 0.0f << ' ';
            file << tx1 << ' ' << tz1 << '\n';

            file << offset << ' ' << y2 << ' ' << z1 << ' ';
            file << 1.0f << ' ' << 0.0f << ' ' << 0.0f << ' ';
            file << tx1 << ' ' << tz2 << '\n';

            file << offset << ' ' << y2 << ' ' << z2 << ' ';
            file << 1.0f << ' ' << 0.0f << ' ' << 0.0f << ' ';
            file << tx2 << ' ' << tz2 << '\n';

            // Left (-X)
            file << -offset << ' ' << y2 << ' ' << z2 << ' ';
            file << -1.0f << ' ' << 0.0f << ' ' << 0.0f << ' ';
            file << tx2 << ' ' << tz2 << '\n';

            file << -offset << ' ' << y1 << ' ' << z1 << ' ';
            file << -1.0f << ' ' << 0.0f << ' ' << 0.0f << ' ';
            file << tx1 << ' ' << tz1 << '\n';

            file << -offset << ' ' << y1 << ' ' << z2 << ' ';
            file << -1.0f << ' ' << 0.0f << ' ' << 0.0f << ' ';
            file << tx2 << ' ' << tz1 << '\n';

            file << -offset << ' ' << y2 << ' ' << z1 << ' ';
            file << -1.0f << ' ' << 0.0f << ' ' << 0.0f << ' ';
            file << tx1 << ' ' << tz2 << '\n';

            file << -offset << ' ' << y1 << ' ' << z1 << ' ';
            file << -1.0f << ' ' << 0.0f << ' ' << 0.0f << ' ';
            file << tx1 << ' ' << tz1 << '\n';

            file << -offset << ' ' << y2 << ' ' << z2 << ' ';
            file << -1.0f << ' ' << 0.0f << ' ' << 0.0f << ' ';
            file << tx2 << ' ' << tz2 << '\n';
        }
    }

    file.close();
}

// Generates points for a sphere with normals and texture coordinates
void sphere(float radius, int slices, int stacks, const std::string& filename) {
    if (radius <= 0 || slices <= 0 || stacks <= 0) {
        throw std::invalid_argument("Error: Radius, slices, and stacks must be positive.");
    }

    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Error: Unable to open file " + filename);
    }

    float arch_alfa = 2 * M_PI / slices;
    float arch_beta = M_PI / stacks;
    float x1, x2, x3, x4, y1, y2, y3, y4, z1, z2, z3, z4;
    float p1n[3], p2n[3], p3n[3], p4n[3];

    for (int i = 0; i < slices; i++) {
        for (int j = 0; j < stacks; j++) {
            x1 = radius * cos(M_PI_2 - arch_beta * j) * sin(arch_alfa * i);
            p1n[0] = cos(M_PI_2 - arch_beta * j) * sin(arch_alfa * i);
            
            x2 = radius * cos(M_PI_2 - arch_beta * (j+1)) * sin(arch_alfa * i);
            p2n[0] = cos(M_PI_2 - arch_beta * (j+1)) * sin(arch_alfa * i);
            
            x3 = radius * cos(M_PI_2 - arch_beta * (j+1)) * sin(arch_alfa * (i+1));
            p3n[0] = cos(M_PI_2 - arch_beta * (j+1)) * sin(arch_alfa * (i+1));
            
            x4 = radius * cos(M_PI_2 - arch_beta * j) * sin(arch_alfa * (i+1));
            p4n[0] = cos(M_PI_2 - arch_beta * j) * sin(arch_alfa * (i+1));

            y1 = radius * sin(M_PI_2 - arch_beta * j);
            p1n[1] = sin(M_PI_2 - arch_beta * j);
            
            y2 = radius * sin(M_PI_2 - arch_beta * (j+1));
            p2n[1] = sin(M_PI_2 - arch_beta * (j+1));
            
            y3 = radius * sin(M_PI_2 - arch_beta * (j+1));
            p3n[1] = sin(M_PI_2 - arch_beta * (j+1));
            
            y4 = radius * sin(M_PI_2 - arch_beta * j);
            p4n[1] = sin(M_PI_2 - arch_beta * j);

            z1 = radius * cos(M_PI_2 - arch_beta * j) * cos(arch_alfa * i);
            p1n[2] = cos(M_PI_2 - arch_beta * j) * cos(arch_alfa * i);
            
            z2 = radius * cos(M_PI_2 - arch_beta * (j+1)) * cos(arch_alfa * i);
            p2n[2] = cos(M_PI_2 - arch_beta * (j+1)) * cos(arch_alfa * i);
            
            z3 = radius * cos(M_PI_2 - arch_beta * (j+1)) * cos(arch_alfa * (i+1));
            p3n[2] = cos(M_PI_2 - arch_beta * (j+1)) * cos(arch_alfa * (i+1));
            
            z4 = radius * cos(M_PI_2 - arch_beta * j) * cos(arch_alfa * (i+1));
            p4n[2] = cos(M_PI_2 - arch_beta * j) * cos(arch_alfa * (i+1));

            normalize(p1n);
            normalize(p2n);
            normalize(p3n);
            normalize(p4n);

            if (j != stacks-1) {
                file << x1 << ' ' << y1 << ' ' << z1 << ' ';
                file << p1n[0] << ' ' << p1n[1] << ' ' << p1n[2] << ' ';
                file << (float)i/slices << ' ' << 1.0f - (float)j/stacks << '\n';
                
                file << x2 << ' ' << y2 << ' ' << z2 << ' ';
                file << p2n[0] << ' ' << p2n[1] << ' ' << p2n[2] << ' ';
                file << (float)i/slices << ' ' << 1.0f - (float)(j+1)/stacks << '\n';
                
                file << x3 << ' ' << y3 << ' ' << z3 << ' ';
                file << p3n[0] << ' ' << p3n[1] << ' ' << p3n[2] << ' ';
                file << (float)(i+1)/slices << ' ' << 1.0f - (float)(j+1)/stacks << '\n';
            }
            if (j != 0) {
                file << x1 << ' ' << y1 << ' ' << z1 << ' ';
                file << p1n[0] << ' ' << p1n[1] << ' ' << p1n[2] << ' ';
                file << (float)i/slices << ' ' << 1.0f - (float)j/stacks << '\n';
                
                file << x3 << ' ' << y3 << ' ' << z3 << ' ';
                file << p3n[0] << ' ' << p3n[1] << ' ' << p3n[2] << ' ';
                file << (float)(i+1)/slices << ' ' << 1.0f - (float)(j+1)/stacks << '\n';
                
                file << x4 << ' ' << y4 << ' ' << z4 << ' ';
                file << p4n[0] << ' ' << p4n[1] << ' ' << p4n[2] << ' ';
                file << (float)(i+1)/slices << ' ' << 1.0f - (float)j/stacks << '\n';
            }
        }
    }
    
    file.close();
}

// Generates points for a cone with normals and texture coordinates
void cone(float radius, float height, int slices, int stacks, const std::string& filename) {
    if (radius <= 0 || height <= 0 || slices <= 0 || stacks <= 0) {
        throw std::invalid_argument("Error: Radius, height, slices, and stacks must be positive.");
    }

    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Error: Unable to open file " + filename);
    }

    float arch_alfa = 2 * M_PI / slices;
    float ratio = height / radius;
    float stack_size = height / stacks;
    float x1, x2, x3, x4, y1, y2, z1, z2, z3, z4, h1, h2, r1, r2;
    float p1n[3], p2n[3], p3n[3], p4n[3];

    // Base of the cone
    for (int i = 0; i < slices; i++) {
        x1 = radius * sin(arch_alfa * i);
        x2 = radius * sin(arch_alfa * (i+1));
        z1 = radius * cos(arch_alfa * i);
        z2 = radius * cos(arch_alfa * (i+1));
     
        file << x1 << ' ' << 0.0f << ' ' << z1 << ' ';
        file << 0.0f << ' ' << -1.0f << ' ' << 0.0f << ' ';
        file << 0.5f + 0.5f * sin(arch_alfa * i) << ' ' << 0.5f + 0.5f * cos(arch_alfa * i) << '\n';

        file << 0.0f << ' ' << 0.0f << ' ' << 0.0f << ' ';
        file << 0.0f << ' ' << -1.0f << ' ' << 0.0f << ' ';
        file << 0.5f << ' ' << 0.5f << '\n';

        file << x2 << ' ' << 0.0f << ' ' << z2 << ' ';
        file << 0.0f << ' ' << -1.0f << ' ' << 0.0f << ' ';
        file << 0.5f + 0.5f * sin(arch_alfa * (i+1)) << ' ' << 0.5f + 0.5f * cos(arch_alfa * (i+1)) << '\n';
    }

    // Sides of the cone
    for (int i = 0; i < stacks; i++) {
        for (int j = 0; j < slices; j++) {
            h1 = height - (i * stack_size);
            h2 = height - ((i+1) * stack_size);
            r1 = h1 / ratio;
            r2 = h2 / ratio;

            x1 = r1 * sin(arch_alfa * j);
            p1n[0] = sin(arch_alfa * j);
            x2 = r1 * sin(arch_alfa * (j+1));
            p2n[0] = sin(arch_alfa * (j+1));
            x3 = r2 * sin(arch_alfa * (j+1));
            p3n[0] = sin(arch_alfa * (j+1));
            x4 = r2 * sin(arch_alfa * j);
            p4n[0] = sin(arch_alfa * j);

            y1 = (i * stack_size);
            y2 = (i+1) * stack_size;
            p1n[1] = sin(atan(radius/height));
            p2n[1] = sin(atan(radius/height));
            p3n[1] = sin(atan(radius/height));
            p4n[1] = sin(atan(radius/height));

            z1 = r1 * cos(arch_alfa * j);
            p1n[2] = cos(arch_alfa * j);
            z2 = r1 * cos(arch_alfa * (j+1));
            p2n[2] = cos(arch_alfa * (j+1));
            z3 = r2 * cos(arch_alfa * (j+1));
            p3n[2] = cos(arch_alfa * (j+1));
            z4 = r2 * cos(arch_alfa * j);
            p4n[2] = cos(arch_alfa * j);

            normalize(p1n);
            normalize(p2n);
            normalize(p3n);
            normalize(p4n);
  
            file << x1 << ' ' << y1 << ' ' << z1 << ' ';
            file << p1n[0] << ' ' << p1n[1] << ' ' << p1n[2] << ' ';
            file << (float)j/slices << ' ' << (float)i/stacks << '\n';

            file << x2 << ' ' << y1 << ' ' << z2 << ' ';
            file << p2n[0] << ' ' << p2n[1] << ' ' << p2n[2] << ' ';
            file << (float)(j+1)/slices << ' ' << (float)i/stacks << '\n';

            file << x4 << ' ' << y2 << ' ' << z4 << ' ';
            file << p4n[0] << ' ' << p4n[1] << ' ' << p4n[2] << ' ';
            file << (float)j/slices << ' ' << (float)(i+1)/stacks << '\n';

            if (i != stacks - 1) {
                file << x4 << ' ' << y2 << ' ' << z4 << ' ';
                file << p4n[0] << ' ' << p4n[1] << ' ' << p4n[2] << ' ';
                file << (float)j/slices << ' ' << (float)(i+1)/stacks << '\n';

                file << x2 << ' ' << y1 << ' ' << z2 << ' ';
                file << p2n[0] << ' ' << p2n[1] << ' ' << p2n[2] << ' ';
                file << (float)(j+1)/slices << ' ' << (float)i/stacks << '\n';

                file << x3 << ' ' << y2 << ' ' << z3 << ' ';
                file << p3n[0] << ' ' << p3n[1] << ' ' << p3n[2] << ' ';
                file << (float)(j+1)/slices << ' ' << (float)(i+1)/stacks << '\n';
            }
        }
    }

    file.close();
}

// Generates points for a torus with normals and texture coordinates
void torus(float max_radius, float min_radius, int slices, int stacks, const std::string& filename) {
    if (max_radius <= 0 || min_radius <= 0 || slices <= 0 || stacks <= 0) {
        throw std::invalid_argument("Error: Radii, slices, and stacks must be positive.");
    }

    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Error: Unable to open file " + filename);
    }

    float arch_alpha = (2 * M_PI) / stacks;
    float arch_beta = (2 * M_PI) / slices;
    float x1, x2, x3, x4, y1, y2, y3, y4, z1, z2, z3, z4;
    float p1n[3], p2n[3], p3n[3], p4n[3];

    float outer_radius = (max_radius + min_radius) / 2;
    float inner_radius = max_radius - min_radius;

    for (int i = 0; i < stacks; i++) {
        for (int j = 0; j < slices; j++) {
            x1 = (outer_radius + inner_radius * cos(arch_alpha * i)) * cos(arch_beta * j);
            p1n[0] = (inner_radius * cos(arch_alpha * i)) * cos(arch_beta * j);
            
            x2 = (outer_radius + inner_radius * cos(arch_alpha * (i+1))) * cos(arch_beta * j);
            p2n[0] = (inner_radius * cos(arch_alpha * (i+1))) * cos(arch_beta * j);
            
            x3 = (outer_radius + inner_radius * cos(arch_alpha * (i+1))) * cos(arch_beta * (j+1));
            p3n[0] = (inner_radius * cos(arch_alpha * (i+1))) * cos(arch_beta * (j+1));
            
            x4 = (outer_radius + inner_radius * cos(arch_alpha * i)) * cos(arch_beta * (j+1));
            p4n[0] = (inner_radius * cos(arch_alpha * i)) * cos(arch_beta * (j+1));

            y1 = inner_radius * sin(arch_alpha * i);
            p1n[1] = inner_radius * sin(arch_alpha * i);
            
            y2 = inner_radius * sin(arch_alpha * (i+1));
            p2n[1] = inner_radius * sin(arch_alpha * (i+1));
            
            y3 = inner_radius * sin(arch_alpha * (i+1));
            p3n[1] = inner_radius * sin(arch_alpha * (i+1));
            
            y4 = inner_radius * sin(arch_alpha * i);
            p4n[1] = inner_radius * sin(arch_alpha * i);

            z1 = (outer_radius + inner_radius * cos(arch_alpha * i)) * sin(arch_beta * j);
            p1n[2] = (inner_radius * cos(arch_alpha * i)) * sin(arch_beta * j);
            
            z2 = (outer_radius + inner_radius * cos(arch_alpha * (i+1))) * sin(arch_beta * j);
            p2n[2] = (inner_radius * cos(arch_alpha * (i+1))) * sin(arch_beta * j);
            
            z3 = (outer_radius + inner_radius * cos(arch_alpha * (i+1))) * sin(arch_beta * (j+1));
            p3n[2] = (inner_radius * cos(arch_alpha * (i+1))) * sin(arch_beta * (j+1));
            
            z4 = (outer_radius + inner_radius * cos(arch_alpha * i)) * sin(arch_beta * (j+1));
            p4n[2] = (inner_radius * cos(arch_alpha * i)) * sin(arch_beta * (j+1));

            normalize(p1n);
            normalize(p2n);
            normalize(p3n);
            normalize(p4n);

            file << x1 << ' ' << y1 << ' ' << z1 << ' ';
            file << p1n[0] << ' ' << p1n[1] << ' ' << p1n[2] << ' ';
            file << (float)i/stacks << ' ' << (float)j/slices << '\n';

            file << x2 << ' ' << y2 << ' ' << z2 << ' ';
            file << p2n[0] << ' ' << p2n[1] << ' ' << p2n[2] << ' ';
            file << (float)(i+1)/stacks << ' ' << (float)j/slices << '\n';

            file << x4 << ' ' << y4 << ' ' << z4 << ' ';
            file << p4n[0] << ' ' << p4n[1] << ' ' << p4n[2] << ' ';
            file << (float)i/stacks << ' ' << (float)(j+1)/slices << '\n';

            file << x2 << ' ' << y2 << ' ' << z2 << ' ';
            file << p2n[0] << ' ' << p2n[1] << ' ' << p2n[2] << ' ';
            file << (float)(i+1)/stacks << ' ' << (float)j/slices << '\n';

            file << x3 << ' ' << y3 << ' ' << z3 << ' ';
            file << p3n[0] << ' ' << p3n[1] << ' ' << p3n[2] << ' ';
            file << (float)(i+1)/stacks << ' ' << (float)(j+1)/slices << '\n';

            file << x4 << ' ' << y4 << ' ' << z4 << ' ';
            file << p4n[0] << ' ' << p4n[1] << ' ' << p4n[2] << ' ';
            file << (float)i/stacks << ' ' << (float)(j+1)/slices << '\n';
        }
    }
    
    file.close();
}

// Bezier functions
float B(float U, float V, float m[4][4]) {
    float aux[4];
    float v[4];
    float r;

    v[0] = powf(V, 3);
    v[1] = powf(V, 2);
    v[2] = V;
    v[3] = 1;

    multMatrixVector(m, v, aux);
    r = powf(U, 3) * aux[0] + powf(U, 2) * aux[1] + U * aux[2] + aux[3];

    return r;
}

float derivU(float U, float V, float m[4][4]) {
    float aux[4];
    float v[4];
    float r;

    v[0] = powf(V, 3);
    v[1] = powf(V, 2);
    v[2] = V;
    v[3] = 1;

    multMatrixVector(m, v, aux);
    r = 3 * powf(U, 2) * aux[0] + 2 * U * aux[1] + aux[2];

    return r;
}

float derivV(float U, float V, float m[4][4]) {
    float aux[4];
    float v[4];
    float r;

    v[0] = 3 * powf(V, 2);
    v[1] = 2 * V;
    v[2] = 1;
    v[3] = 0;

    multMatrixVector(m, v, aux);
    r = powf(U, 3) * aux[0] + powf(U, 2) * aux[1] + U * aux[2] + aux[3];

    return r;
}

// Generate Bezier surface patch
std::string surface(float mx[4][4], float my[4][4], float mz[4][4], int tesselation) {
    std::stringstream buffer;
    float x1, x2, x3, x4, y1, y2, y3, y4, z1, z2, z3, z4;
    float p1u[3], p2u[3], p3u[3], p4u[3], p1v[3], p2v[3], p3v[3], p4v[3], p1n[3], p2n[3], p3n[3], p4n[3]; 
    float tesselation_level = 1.0f / tesselation;

    for (float i = 0; i < 1; i += tesselation_level) {
        for (float j = 0; j < 1; j += tesselation_level) {
            // Calculate points
            x1 = B(i, j, mx);
            p1u[0] = derivU(i, j, mx);
            p1v[0] = derivV(i, j, mx);

            x2 = B(i + tesselation_level, j, mx);
            p2u[0] = derivU(i + tesselation_level, j, mx);
            p2v[0] = derivV(i + tesselation_level, j, mx);

            x3 = B(i + tesselation_level, j + tesselation_level, mx);
            p3u[0] = derivU(i + tesselation_level, j + tesselation_level, mx);
            p3v[0] = derivV(i + tesselation_level, j + tesselation_level, mx);

            x4 = B(i, j + tesselation_level, mx);
            p4u[0] = derivU(i, j + tesselation_level, mx);
            p4v[0] = derivV(i, j + tesselation_level, mx);

            y1 = B(i, j, my);
            p1u[1] = derivU(i, j, my);
            p1v[1] = derivV(i, j, my);

            y2 = B(i + tesselation_level, j, my);
            p2u[1] = derivU(i + tesselation_level, j, my);
            p2v[1] = derivV(i + tesselation_level, j, my);

            y3 = B(i + tesselation_level, j + tesselation_level, my);
            p3u[1] = derivU(i + tesselation_level, j + tesselation_level, my);
            p3v[1] = derivV(i + tesselation_level, j + tesselation_level, my);
            
            y4 = B(i, j + tesselation_level, my);
            p4u[1] = derivU(i, j + tesselation_level, my);
            p4v[1] = derivV(i, j + tesselation_level, my);

            z1 = B(i, j, mz);
            p1u[2] = derivU(i, j, mz);
            p1v[2] = derivV(i, j, mz);

            z2 = B(i + tesselation_level, j, mz);
            p2u[2] = derivU(i + tesselation_level, j, mz);
            p2v[2] = derivV(i + tesselation_level, j, mz);

            z3 = B(i + tesselation_level, j + tesselation_level, mz);
            p3u[2] = derivU(i + tesselation_level, j + tesselation_level, mz);
            p3v[2] = derivV(i + tesselation_level, j + tesselation_level, mz);

            z4 = B(i, j + tesselation_level, mz);
            p4u[2] = derivU(i, j + tesselation_level, mz);
            p4v[2] = derivV(i, j + tesselation_level, mz);

            // Calculate normals
            crossProd(p1u, p1v, p1n);
            crossProd(p2u, p2v, p2n);
            crossProd(p3u, p3v, p3n);
            crossProd(p4u, p4v, p4n);

            normalize(p1n);
            normalize(p2n);
            normalize(p3n);
            normalize(p4n);

            // First triangle
            buffer << x1 << ' ' << y1 << ' ' << z1 << ' ';
            buffer << p1n[0] << ' ' << p1n[1] << ' ' << p1n[2] << ' ';
            buffer << j << ' ' << i << '\n';

            buffer << x2 << ' ' << y2 << ' ' << z2 << ' ';
            buffer << p2n[0] << ' ' << p2n[1] << ' ' << p2n[2] << ' ';
            buffer << j << ' ' << i + tesselation_level << '\n';

            buffer << x4 << ' ' << y4 << ' ' << z4 << ' ';
            buffer << p4n[0] << ' ' << p4n[1] << ' ' << p4n[2] << ' ';
            buffer << j + tesselation_level << ' ' << i << '\n';

            // Second triangle
            buffer << x2 << ' ' << y2 << ' ' << z2 << ' ';
            buffer << p2n[0] << ' ' << p2n[1] << ' ' << p2n[2] << ' ';
            buffer << j << ' ' << i + tesselation_level << '\n';

            buffer << x3 << ' ' << y3 << ' ' << z3 << ' ';
            buffer << p3n[0] << ' ' << p3n[1] << ' ' << p3n[2] << ' ';
            buffer << j + tesselation_level << ' ' << i + tesselation_level << '\n';

            buffer << x4 << ' ' << y4 << ' ' << z4 << ' ';
            buffer << p4n[0] << ' ' << p4n[1] << ' ' << p4n[2] << ' ';
            buffer << j + tesselation_level << ' ' << i << '\n';
        }
    }
    
    return buffer.str();
}

// Read patches file
std::vector<std::vector<std::vector<float>>> readPatchesFile(const std::string& patch) {
    std::vector<std::vector<int>> indexPatch;
    std::vector<std::vector<float>> controlPoints;
    std::vector<std::vector<std::vector<float>>> ret;

    std::fstream file(patch);
    if (!file.is_open()) {
        return ret;
    }

    std::string line;

    // Read patches and control points
    for (int j = 0; j < 2; j++) {
        // Number of patches/control points (first line)
        if (!std::getline(file, line)) return ret;
        int n = std::stoi(line);

        // If j == 0, reading patches; otherwise reading control points
        for (int i = 0; i < n; i++) {
            if (!std::getline(file, line)) return ret;
            std::vector<int> index;
            std::vector<float> point;
            std::istringstream iss(line);
            std::string token;
            
            while (std::getline(iss, token, ',')) {
                if (j == 0) {
                    index.push_back(std::stoi(token));
                } else {
                    point.push_back(std::stof(token));
                }
            }
            
            if (j == 0) {
                indexPatch.push_back(index);
            } else {
                controlPoints.push_back(point);
            }
        }
    }

    for (const auto& indexes : indexPatch) {
        std::vector<std::vector<float>> patch;
        for (int index : indexes) {
            patch.push_back(controlPoints[index]);
        }
        ret.push_back(patch);
    }
    
    file.close();
    return ret;
}

// Generate Bezier patches
void bezier_patches(const std::string& patch, int tesselation, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Error: Unable to open file " + filename);
    }

    std::vector<std::vector<std::vector<float>>> patches = readPatchesFile(patch);

    float bezier[4][4] = {
        {-1.0f, 3.0f, -3.0f, 1.0f},
        {3.0f, -6.0f, 3.0f, 0.0f},
        {-3.0f, 3.0f, 0.0f, 0.0f},
        {1.0f, 0.0f, 0.0f, 0.0f}
    };

    float mx[4][4];
    float my[4][4];
    float mz[4][4];
    float aux[4][4];

    for (size_t i = 0; i < patches.size(); i++) {
        int posi = 0;
        int posj = 0;

        for (int j = 0; j < 16; j++) {
            mx[posj][posi] = patches[i][j][0];
            my[posj][posi] = patches[i][j][1];
            mz[posj][posi] = patches[i][j][2];

            posj++;
            if (j == 3 || j == 7 || j == 11) {
                posi++;
                posj = 0;
            }
        }

        multMatrixMatrix(bezier, mx, aux);
        multMatrixMatrix(aux, bezier, mx);

        multMatrixMatrix(bezier, my, aux);
        multMatrixMatrix(aux, bezier, my);

        multMatrixMatrix(bezier, mz, aux);
        multMatrixMatrix(aux, bezier, mz);

        file << surface(mx, my, mz, tesselation);
    }

    file.close();
}

// Enhanced version of bezier using the correct function name
void bezier(const std::string& patchFile, int tessellation, const std::string& outputFile) {
    if (tessellation <= 0) {
        throw std::invalid_argument("Error: Tessellation level must be positive.");
    }
    
    bezier_patches(patchFile, tessellation, outputFile);
}

