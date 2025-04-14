#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <string>
#include <stdexcept>
#include <array>
#include "generatorAux.h"

//Define 4x4 matrix type
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

Matrix4x4 translationMatrix(float x, float y, float z) {
    Matrix4x4 m = identityMatrix();
    m[0][3] = x;
    m[1][3] = y;
    m[2][3] = z;
    return m;
}

Matrix4x4 scaleMatrix(float x, float y, float z) {
    Matrix4x4 m = identityMatrix();
    m[0][0] = x;
    m[1][1] = y;
    m[2][2] = z;
    return m;
}

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
//Aplica uma transformacao a um vertice
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

/**
 * Writes a list of vertices to a file.
 */
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


void plane(float unit, int slices, const std::string& filename) {
    if (unit <= 0 || slices <= 0) {
        throw std::invalid_argument("Error: Unit and slices must be positive.");
    }

    std::vector<Vertex> vertices;
    float comp = unit / slices;
    float offset = unit / 2.0f;
    
    Matrix4x4 scaleM = scaleMatrix(comp, 1.0f, comp);
    Matrix4x4 transM = translationMatrix(-offset, 0.0f, -offset);
    Matrix4x4 transform = multiplyMatrices(transM, scaleM);

    for (int i = 0; i < slices; i++) {
        for (int j = 0; j < slices; j++) {
            Vertex v1 = {float(i), 0.0f, float(j)};
            Vertex v2 = {float(i+1), 0.0f, float(j+1)};
            Vertex v3 = {float(i+1), 0.0f, float(j)};
            Vertex v4 = {float(i), 0.0f, float(j+1)};
            
            v1 = transformVertex(v1, transform);
            v2 = transformVertex(v2, transform);
            v3 = transformVertex(v3, transform);
            v4 = transformVertex(v4, transform);

            vertices.push_back(v1);
            vertices.push_back(v2);
            vertices.push_back(v3);
            vertices.push_back(v1);
            vertices.push_back(v4);
            vertices.push_back(v2);
        }
    }
    writeVertices(filename, vertices);
}


void box(float unit, int slices, const std::string& filename) {
    if (unit <= 0 || slices <= 0) {
        throw std::invalid_argument("Error: Unit and slices must be positive.");
    }

    std::vector<Vertex> vertices;
    float offset = unit / 2.0f;
    
    for (int face = 0; face < 6; face++) {
        Matrix4x4 faceTransform = identityMatrix();
        switch(face) {
            case 0: 
                faceTransform = multiplyMatrices(translationMatrix(0, offset, 0), rotationMatrixX(0)); 
                break;
            case 1: 
                faceTransform = multiplyMatrices(translationMatrix(0, -offset, 0),rotationMatrixX(M_PI)); 
                break;
            case 2: 
                faceTransform = multiplyMatrices(translationMatrix(0, 0, offset), rotationMatrixX(M_PI/2)); 
                break;
            case 3: 
                faceTransform = multiplyMatrices(translationMatrix(0, 0, -offset), multiplyMatrices(rotationMatrixX(-M_PI/2), rotationMatrixZ(M_PI))); 
                break;
            case 4: 
                faceTransform = multiplyMatrices(translationMatrix(offset, 0, 0), rotationMatrixZ(-M_PI/2)); 
                break;
            case 5:
                faceTransform = multiplyMatrices(translationMatrix(-offset, 0, 0), rotationMatrixZ(M_PI/2)); 
                break;
        }
        
        float comp = unit / slices;
        Matrix4x4 scaleM = scaleMatrix(comp, 1.0f, comp);
        Matrix4x4 positionM = translationMatrix(-unit/2 + comp/2, 0, -unit/2 + comp/2);
        
        for (int i = 0; i < slices; i++) {
            for (int j = 0; j < slices; j++) {
                Vertex v1 = {float(i), 0.0f, float(j)};
                Vertex v2 = {float(i+1), 0.0f, float(j)};
                Vertex v3 = {float(i+1), 0.0f, float(j+1)};
                Vertex v4 = {float(i), 0.0f, float(j+1)};
                
                Matrix4x4 totalTransform = multiplyMatrices(faceTransform, 
                                         multiplyMatrices(positionM, scaleM));
                
                v1 = transformVertex(v1, totalTransform);
                v2 = transformVertex(v2, totalTransform);
                v3 = transformVertex(v3, totalTransform);
                v4 = transformVertex(v4, totalTransform);

                if (face == 0 || face == 2 || face == 4) { 
                    vertices.push_back(v1);
                    vertices.push_back(v2);
                    vertices.push_back(v3);
                    
                    vertices.push_back(v1);
                    vertices.push_back(v3);
                    vertices.push_back(v4);
                } else { 
                    vertices.push_back(v1);
                    vertices.push_back(v3);
                    vertices.push_back(v2);
                    
                    vertices.push_back(v1);
                    vertices.push_back(v4);
                    vertices.push_back(v3);
                }
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
    float arch_alfa = static_cast<float>(2 * M_PI) / slices;  
    float stack_size = static_cast<float>(M_PI) / stacks;       
    Matrix4x4 scaleM = scaleMatrix(radius, radius, radius);

    for (int i = 0; i < stacks; i++) {
        float theta1 = static_cast<float>(i) * stack_size;    
        float theta2 = static_cast<float>(i + 1) * stack_size; 

        for (int j = 0; j < slices; j++) {
            float phi1 = static_cast<float>(j) * arch_alfa;   
            float phi2 = static_cast<float>(j + 1) * arch_alfa;

            Vertex v1 = {
                static_cast<float>(sin(theta1) * cos(phi1)),
                static_cast<float>(cos(theta1)),
                static_cast<float>(sin(theta1) * sin(phi1))
            };

            Vertex v2 = {
                static_cast<float>(sin(theta1) * cos(phi2)),
                static_cast<float>(cos(theta1)),
                static_cast<float>(sin(theta1) * sin(phi2))
            };

            Vertex v3 = {
                static_cast<float>(sin(theta2) * cos(phi2)),
                static_cast<float>(cos(theta2)),
                static_cast<float>(sin(theta2) * sin(phi2))
            };

            Vertex v4 = {
                static_cast<float>(sin(theta2) * cos(phi1)),
                static_cast<float>(cos(theta2)),
                static_cast<float>(sin(theta2) * sin(phi1))
            };

            v1 = transformVertex(v1, scaleM);
            v2 = transformVertex(v2, scaleM);
            v3 = transformVertex(v3, scaleM);
            v4 = transformVertex(v4, scaleM);
            vertices.push_back(v1);
            vertices.push_back(v2);
            vertices.push_back(v3);
            vertices.push_back(v1);
            vertices.push_back(v3);
            vertices.push_back(v4);
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
    

    Vertex center = {0.0f, 0.0f, 0.0f};
    for (int i = 0; i < slices; i++) {
        float angle1 = arch_alfa * i;
        float angle2 = arch_alfa * (i + 1);
        
        Vertex v1 = {radius * sin(angle1), 0.0f, radius * cos(angle1)};
        Vertex v2 = {radius * sin(angle2), 0.0f, radius * cos(angle2)};

        vertices.push_back(v1);
        vertices.push_back(center);
        vertices.push_back(v2);
    }
    

    Vertex apex = {0.0f, height, 0.0f}; 
    
    for (int i = 0; i < stacks; i++) {
        float h1 = height * static_cast<float>(i) / stacks;
        float h2 = height * static_cast<float>(i + 1) / stacks;
        float r1 = radius * (1.0f - static_cast<float>(i) / stacks);
        float r2 = radius * (1.0f - static_cast<float>(i + 1) / stacks);
        
        for (int j = 0; j < slices; j++) {
            float angle1 = arch_alfa * j;
            float angle2 = arch_alfa * (j + 1);
            
            Vertex v1 = {r1 * sin(angle1), h1, r1 * cos(angle1)};
            Vertex v2 = {r1 * sin(angle2), h1, r1 * cos(angle2)};
            Vertex v3 = {r2 * sin(angle2), h2, r2 * cos(angle2)};
            Vertex v4 = {r2 * sin(angle1), h2, r2 * cos(angle1)};
  
            vertices.push_back(v1);
            vertices.push_back(v2);
            vertices.push_back(v3);
            
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

    for (int i = 0; i < rings; i++) {
        float theta = 2.0f * M_PI * i / rings;
        float theta_next = 2.0f * M_PI * (i + 1) / rings;

        Matrix4x4 rotateM = rotationMatrixY(theta);
        Matrix4x4 rotateNextM = rotationMatrixY(theta_next);

        Matrix4x4 transM = translationMatrix(torusRadius, 0.0f, 0.0f);
    
        Matrix4x4 transform = multiplyMatrices(rotateM, transM);
        Matrix4x4 transformNext = multiplyMatrices(rotateNextM, transM);

        for (int j = 0; j < sides; j++) {
            float phi = 2.0f * M_PI * j / sides;
            float phi_next = 2.0f * M_PI * (j + 1) / sides;
            
    
            Vertex p1 = {ringRadius * cos(phi), ringRadius * sin(phi), 0.0f};
            Vertex p2 = {ringRadius * cos(phi_next), ringRadius * sin(phi_next), 0.0f};
   
            Vertex v1 = transformVertex(p1, transform);
            Vertex v2 = transformVertex(p2, transform);
       
            Vertex v3 = transformVertex(p2, transformNext);
            Vertex v4 = transformVertex(p1, transformNext);

            
            vertices.push_back(v1);
            vertices.push_back(v2);
            vertices.push_back(v3);

          
            vertices.push_back(v1);
            vertices.push_back(v3);
            vertices.push_back(v4);
        }
    }

    writeVertices(filename, vertices);
}

