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

//-- VERTEX TRANSFORMATION --
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


//-- PRIMITIVES POINTS GENERATION --
// Generates points for a plane
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

// Generates points for a box
void box(float unit, int slices, const std::string& filename) {
    if (unit <= 0 || slices <= 0) {
        throw std::invalid_argument("Error: Unit and slices must be positive.");
    }

    std::vector<Vertex> vertices;
    float comp = unit / slices;
    float offset = unit / 2.0f;
    float x1, x2, y1, y2, z1, z2;

    // Top and bottom faces (Y = +offset and Y = -offset)
    for (int i = 0; i < slices; i++) {
        for (int j = 0; j < slices; j++) {
            x1 = i * comp - offset;
            z1 = j * comp - offset;
            x2 = (i + 1) * comp - offset;
            z2 = (j + 1) * comp - offset;

            // Top (+Y)
            vertices.push_back({x1, offset, z1});
            vertices.push_back({x2, offset, z2});
            vertices.push_back({x2, offset, z1});

            vertices.push_back({x1, offset, z1});
            vertices.push_back({x1, offset, z2});
            vertices.push_back({x2, offset, z2});

            // Bottom (-Y)
            vertices.push_back({x2, -offset, z2});
            vertices.push_back({x1, -offset, z1});
            vertices.push_back({x2, -offset, z1});

            vertices.push_back({x1, -offset, z2});
            vertices.push_back({x1, -offset, z1});
            vertices.push_back({x2, -offset, z2});
        }
    }

    // Front and back faces (Z = +offset and Z = -offset)
    for (int i = 0; i < slices; i++) {
        for (int j = 0; j < slices; j++) {
            x1 = i * comp - offset;
            y1 = j * comp - offset;
            x2 = (i + 1) * comp - offset;
            y2 = (j + 1) * comp - offset;

            // Front (+Z)
            vertices.push_back({x2, y2, offset});
            vertices.push_back({x1, y1, offset});
            vertices.push_back({x2, y1, offset});

            vertices.push_back({x1, y2, offset});
            vertices.push_back({x1, y1, offset});
            vertices.push_back({x2, y2, offset});

            // Back (-Z)
            vertices.push_back({x1, y1, -offset});
            vertices.push_back({x2, y2, -offset});
            vertices.push_back({x2, y1, -offset});

            vertices.push_back({x1, y1, -offset});
            vertices.push_back({x1, y2, -offset});
            vertices.push_back({x2, y2, -offset});
        }
    }

    // Right and left faces (X = +offset and X = -offset)
    for (int i = 0; i < slices; i++) {
        for (int j = 0; j < slices; j++) {
            z1 = i * comp - offset;
            y1 = j * comp - offset;
            z2 = (i + 1) * comp - offset;
            y2 = (j + 1) * comp - offset;

            // Right (+X)
            vertices.push_back({offset, y1, z1});
            vertices.push_back({offset, y2, z2});
            vertices.push_back({offset, y1, z2});

            vertices.push_back({offset, y1, z1});
            vertices.push_back({offset, y2, z1});
            vertices.push_back({offset, y2, z2});

            // Left (-X)
            vertices.push_back({-offset, y2, z2});
            vertices.push_back({-offset, y1, z1});
            vertices.push_back({-offset, y1, z2});

            vertices.push_back({-offset, y2, z1});
            vertices.push_back({-offset, y1, z1});
            vertices.push_back({-offset, y2, z2});
        }
    }

    writeVertices(filename, vertices);
}

// Generates points for a sphere
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


// Generates points for a cone 
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


// Generates points for a torus
void torus(float outerRadius, float innerRadius, int slices, int stacks, const std::string& filename) {
    if (outerRadius <= 0 || innerRadius <= 0 || slices <= 0 || stacks <= 0) {
        throw std::invalid_argument("Error: Outer radius, inner radius, slices, and stacks must be positive.");
    }

    std::vector<Vertex> vertices;
    float arch_alpha = (2 * M_PI) / stacks;
    float arch_beta = (2 * M_PI) / slices;

    
    float max_radius = (outerRadius + innerRadius) / 2.0f;
    float min_radius = max_radius - innerRadius;

    for (int i = 0; i < stacks; i++) {
        for (int j = 0; j < slices; j++) {
            float x1 = (max_radius + min_radius * cos(arch_alpha * i)) * cos(arch_beta * j);
            float x2 = (max_radius + min_radius * cos(arch_alpha * (i+1))) * cos(arch_beta * j);
            float x3 = (max_radius + min_radius * cos(arch_alpha * (i+1))) * cos(arch_beta * (j+1));
            float x4 = (max_radius + min_radius * cos(arch_alpha * i)) * cos(arch_beta * (j+1));

            float y1 = min_radius * sin(arch_alpha * i);
            float y2 = min_radius * sin(arch_alpha * (i+1));
            float y3 = min_radius * sin(arch_alpha * (i+1));
            float y4 = min_radius * sin(arch_alpha * i);

            float z1 = (max_radius + min_radius * cos(arch_alpha * i)) * sin(arch_beta * j);
            float z2 = (max_radius + min_radius * cos(arch_alpha * (i+1))) * sin(arch_beta * j);
            float z3 = (max_radius + min_radius * cos(arch_alpha * (i+1))) * sin(arch_beta * (j+1));
            float z4 = (max_radius + min_radius * cos(arch_alpha * i)) * sin(arch_beta * (j+1));

            // Triangle 1
            vertices.push_back({x1, y1, z1});
            vertices.push_back({x2, y2, z2});
            vertices.push_back({x4, y4, z4});

            // Triangle 2
            vertices.push_back({x2, y2, z2});
            vertices.push_back({x3, y3, z3});
            vertices.push_back({x4, y4, z4});
        }
    }
    writeVertices(filename, vertices);
}

//Linear interpolation between two vertices
Vertex lerp(const Vertex& a, const Vertex& b, float t) {
    return {
        a.x + (b.x - a.x) * t,
        a.y + (b.y - a.y) * t,
        a.z + (b.z - a.z) * t
    };
}

//de Casteljau algorithm for Bezier curve (1D)
Vertex deCasteljau1D(const std::vector<Vertex>& points, float t) {
    std::vector<Vertex> temp = points;
    for (int k = 1; k < temp.size(); ++k) {
        for (int i = 0; i < temp.size() - k; ++i) {
            temp[i] = lerp(temp[i], temp[i + 1], t);
        }
    }
    return temp[0];
}

//de Casteljau for Bezier patch (2D)
Vertex calculateBezierPoint(const std::vector<std::vector<Vertex>>& controlPoints, float u, float v) {
    std::vector<Vertex> uCurve(4);
    for (int i = 0; i < 4; ++i) {
        uCurve[i] = deCasteljau1D(controlPoints[i], v);
    }
    return deCasteljau1D(uCurve, u);
}

// Generates a Bezier patch mesh
std::vector<Vertex> generateBezierPatch(const std::vector<std::vector<Vertex>>& controlPoints, int tessellation) {
    std::vector<Vertex> vertices;
    float step = 1.0f / tessellation;

    //Precomputed points for effiency
    std::vector<std::vector<Vertex>> grid(tessellation + 1, std::vector<Vertex>(tessellation + 1));
    for (int i = 0; i <= tessellation; ++i) {
        float u = i * step;
        for (int j = 0; j <= tessellation; ++j) {
            float v = j * step;
            grid[i][j] = calculateBezierPoint(controlPoints, u, v);
        }
    }

    for (int i = 0; i < tessellation; ++i) {
        for (int j = 0; j < tessellation; ++j) {
            // Triangle 1 (swap 2nd and 3rd vertex)
            vertices.push_back(grid[i][j]);
            vertices.push_back(grid[i+1][j+1]);
            vertices.push_back(grid[i+1][j]);
            // Triangle 2 (swap 2nd and 3rd vertex)
            vertices.push_back(grid[i][j]);
            vertices.push_back(grid[i][j+1]);
            vertices.push_back(grid[i+1][j+1]);
        }
    }
    return vertices;
}

/**
 * @brief Parse a Bezier patch file and generate the 3D model
 * 
 * The patch file format should contain:
 * - First line: number of patches
 * - For each patch: 16 indices referring to the control points
 * - After all patches: the coordinates of all control points
 * 
 * @param patchFile Path to the file containing Bezier patch definitions
 * @param tessellation Level of detail (number of subdivisions)
 * @param outputFile Output file path for the generated 3D model
 */
void bezier(const std::string& patchFile, int tessellation, const std::string& outputFile) {
    if (tessellation <= 0) {
        throw std::invalid_argument("Error: Tessellation level must be positive.");
    }
    
    std::ifstream file(patchFile);
    if (!file.is_open()) {
        throw std::runtime_error("Error: Unable to open patch file " + patchFile);
    }
    
    // Read number of patches
    std::string line;
    std::getline(file, line);
    int numPatches = std::stoi(line);
    
    // Read patch indices
    std::vector<std::vector<int>> patchIndices;
    for (int i = 0; i < numPatches; i++) {
        std::vector<int> patchIdx;
        std::getline(file, line);
        
        // Parse indices, handling both comma-separated and space-separated formats
        std::istringstream iss(line);
        std::string token;
        
        // Check if comma-separated
        if (line.find(',') != std::string::npos) {
            while (std::getline(iss, token, ',')) {
                // Trim whitespace
                token.erase(0, token.find_first_not_of(" \t"));
                token.erase(token.find_last_not_of(" \t") + 1);
                if (!token.empty()) {
                    patchIdx.push_back(std::stoi(token));
                }
            }
        } else {
            // Space-separated
            int idx;
            while (iss >> idx) {
                patchIdx.push_back(idx);
            }
        }
        
        // Verify we have 16 control points per patch
        if (patchIdx.size() != 16) {
            throw std::runtime_error("Error: Each patch must have exactly 16 control points");
        }
        
        patchIndices.push_back(patchIdx);
    }
    
    // Read number of control points
    std::getline(file, line);
    int numControlPoints = std::stoi(line);
    
    // Read control points
    std::vector<Vertex> controlPoints;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        
        std::istringstream iss(line);
        float x, y, z;
        
        // Check if comma-separated
        if (line.find(',') != std::string::npos) {
            std::string xStr, yStr, zStr;
            std::getline(iss, xStr, ',');
            std::getline(iss, yStr, ',');
            std::getline(iss, zStr, ',');
            
            // Trim whitespace
            xStr.erase(0, xStr.find_first_not_of(" \t"));
            xStr.erase(xStr.find_last_not_of(" \t") + 1);
            yStr.erase(0, yStr.find_first_not_of(" \t"));
            yStr.erase(yStr.find_last_not_of(" \t") + 1);
            zStr.erase(0, zStr.find_first_not_of(" \t"));
            zStr.erase(zStr.find_last_not_of(" \t") + 1);
            
            x = std::stof(xStr);
            y = std::stof(yStr);
            z = std::stof(zStr);
        } else {
            // Space-separated
            iss >> x >> y >> z;
        }
        
        controlPoints.push_back({x, y, z});
    }
    
    // Verify we have the correct number of control points
    if (controlPoints.size() != numControlPoints) {
        std::cerr << "Warning: Expected " << numControlPoints << " control points but read " 
                  << controlPoints.size() << std::endl;
    }
    
    // Generate mesh for each patch
    std::vector<Vertex> allVertices;
    for (const auto& patch : patchIndices) {
        // Convert flat indices to 4x4 grid of control points
        std::vector<std::vector<Vertex>> patchControlPoints(4, std::vector<Vertex>(4));
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                int index = patch[i * 4 + j];
                if (index >= 0 && index < controlPoints.size()) {
                    patchControlPoints[i][j] = controlPoints[index];
                } else {
                    throw std::runtime_error("Error: Invalid control point index: " + std::to_string(index));
                }
            }
        }
        
        // Generate and add vertices for this patch
        std::vector<Vertex> patchVertices = generateBezierPatch(patchControlPoints, tessellation);
        allVertices.insert(allVertices.end(), patchVertices.begin(), patchVertices.end());
    }
    

    // Write all vertices to the output file
    writeVertices(outputFile, allVertices);
}

