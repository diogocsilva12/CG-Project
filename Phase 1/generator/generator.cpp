// Math definitions and necessary includes
#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>
#include <iostream>
#include <cstring>
#include <algorithm>
#include <fstream>
#include <sstream>
#include "generatorAux.h"  // Contains the primitive generation functions
#include <unordered_map>

int main(int argc, char** argv) {
    // Create a map to associate primitive names with numeric IDs
    // This makes it easier to use a switch statement later
    std::unordered_map<std::string, int> figuraMap;
    figuraMap.insert(std::make_pair("plane", 1));
    figuraMap.insert(std::make_pair("box", 2));
    figuraMap.insert(std::make_pair("cone", 3));
    figuraMap.insert(std::make_pair("sphere", 4));

    // Declare variables to store command line arguments
    std::string figura;     // Will store the primitive type (plane, box, etc.)
    std::string ficheiro;   // Will store the output file path

    // Variables for primitive parameters
    float unit, radius, height;  // Dimensions
    int slices, stacks;         // Subdivisions

    // Check if we have at least one argument (the primitive type)
    if (argc > 1) {
        figura = argv[1];  // Get the primitive type from first argument
        int figuraType = figuraMap[figura];  // Convert string to numeric ID

        // Handle different primitives
        switch (figuraType) {
            case 1: // plane
                if (argc == 5) {
                    std::cout << "Generating plane points..." << std::endl;
                    unit = std::stof(argv[2]);     // Size of the plane
                    slices = std::stoi(argv[3]);   // Number of divisions
                    ficheiro = "tests/" + std::string(argv[4]);  // Output file path
                    plane(unit, slices, ficheiro); // Generate the plane
                    std::cout << "Plane generated successfully!" << std::endl;
                } else {
                    std::cout << "numero incorreto de argumentos\n";
                }
                break;

            case 2: // box
                if (argc == 5) {
                    std::cout << "Generating box points..." << std::endl;
                    unit = std::stof(argv[2]);     // Size of the box
                    slices = std::stoi(argv[3]);   // Number of divisions per edge
                    ficheiro = "tests/" + std::string(argv[4]);  // Output file path
                    box(unit, slices, ficheiro);   // Generate the box
                    std::cout << "Box generated successfully!" << std::endl;
                } else {
                    std::cout << "numero incorreto de argumentos\n";
                }
                break;

            case 3: // cone
                if (argc == 7) {
                    std::cout << "Generating cone points..." << std::endl;
                    radius = std::stof(argv[2]);   // Base radius
                    height = std::stof(argv[3]);   // Height
                    slices = std::stoi(argv[4]);   // Angular divisions (around)
                    stacks = std::stoi(argv[5]);   // Vertical divisions
                    ficheiro = "tests/" + std::string(argv[6]);  // Output file path
                    cone(radius, height, slices, stacks, ficheiro); // Generate the cone
                    std::cout << "Cone generated successfully!" << std::endl;
                } else {
                    std::cout << "numero incorreto de argumentos\n";
                }
                break;

            case 4: // sphere
                if (argc == 6) {
                    std::cout << "Generating sphere points..." << std::endl;
                    radius = std::stof(argv[2]);   // Sphere radius
                    slices = std::stoi(argv[3]);   // Longitude divisions
                    stacks = std::stoi(argv[4]);   // Latitude divisions
                    ficheiro = "tests/" + std::string(argv[5]);  // Output file path
                    sphere(radius, slices, stacks, ficheiro); // Generate the sphere
                    std::cout << "Sphere generated successfully!" << std::endl;
                } else {
                    std::cout << "numero incorreto de argumentos\n";
                }
                break;

            default:
                std::cout << "figura incorreta\n";
        }
    } else {
        std::cout << "numero incorreto de argumentos\n";
    }
}


/*
This code:

Takes command-line arguments specifying what 3D primitive to generate and its parameters
Uses a map to convert primitive names to numeric IDs
Validates the number of arguments for each primitive type
Converts string arguments to appropriate numeric types (float/int)
Calls the corresponding generation function from generatorAux.h
The generation functions (plane, box, cone, sphere) create vertices for the primitive and save them to a .3d file
Output files are saved in a "tests/" directory with the specified filename
For example, running:

Will:

Create a plane of size 1
Divide it into 3x3 segments
Save the vertices to tests/plane.3d
The .3d files contain:

First line: number of vertices
Following lines: x y z coordinates of each vertex
*/