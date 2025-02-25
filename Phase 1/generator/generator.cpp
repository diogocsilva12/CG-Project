#include <iostream>
#include <string>
#include <unordered_map>
#include <stdexcept>
#include "generatorAux.h"

// Function to display usage instructions
void showUsage(const std::string& programName) {
    std::cout << "Usage: " << programName << " <shape> [parameters...]\n";
    std::cout << "Shapes and their parameters:\n";
    std::cout << "  plane <unit> <slices> <plane.3d>\n";
    std::cout << "  box <unit> <slices> <box.3d>\n";
    std::cout << "  cone <radius> <height> <slices> <stacks> <cone.3d\n";
    std::cout << "  sphere <radius> <slices> <stacks> <sphere.3d>\n";
    std::cout << "Example: " << programName << " plane 10 5 plane.3d\n";
    std::cout << "NOTE: Creating a 3d file will override the previous one!\n";
}

// Function to handle plane generation
void handlePlane(int argc, char** argv) {
    if (argc != 5) {
        throw std::invalid_argument("Error: Plane requires 3 arguments: <unit> <slices> <output_file>");
    }
    float unit = std::stof(argv[2]);
    int slices = std::stoi(argv[3]);
    // Create standardized filename
    std::string filename = "../tests/plane_" + std::string(argv[2]) + "_" + argv[3] + ".3d";
    plane(unit, slices, filename);
    std::cout << "Plane generated successfully! Saved to " << filename << "\n";
}

// Function to handle box generation
void handleBox(int argc, char** argv) {
    if (argc != 5) {
        throw std::invalid_argument("Error: Box requires 3 arguments: <unit> <slices> <output_file>");
    }
    float unit = std::stof(argv[2]);
    int slices = std::stoi(argv[3]);
    // Create standardized filename
    std::string filename = "../tests/box_" + std::string(argv[2]) + "_" + argv[3] + ".3d";
    box(unit, slices, filename);
    std::cout << "Box generated successfully! Saved to " << filename << "\n";
}

// Function to handle cone generation
void handleCone(int argc, char** argv) {
    if (argc != 7) {
        throw std::invalid_argument("Usage: cone radius height slices stacks filename");
    }
    
    float radius = std::stof(argv[2]);
    float height = std::stof(argv[3]);
    int slices = std::stoi(argv[4]);
    int stacks = std::stoi(argv[5]);
    
    // Create standardized filename with correct path
    std::string filename = "../tests/cone_" + std::string(argv[2]) + "_" + argv[3] + "_" + 
                          argv[4] + "_" + argv[5] + ".3d";  // Added "../" prefix
    
    cone(radius, height, slices, stacks, filename);
    std::cout << "Cone generated successfully! Saved to " << filename << "\n";
}

// Function to handle sphere generation
void handleSphere(int argc, char** argv) {
    if (argc != 6) {
        throw std::invalid_argument("Error: Sphere requires 4 arguments: <radius> <slices> <stacks> <output_file>");
    }
    float radius = std::stof(argv[2]);
    int slices = std::stoi(argv[3]);
    int stacks = std::stoi(argv[4]);
    // Create standardized filename
    std::string filename = "../tests/sphere_" + std::string(argv[2]) + "_" + argv[3] + "_" + argv[4] + ".3d";
    sphere(radius, slices, stacks, filename);
    std::cout << "Sphere generated successfully! Saved to " << filename << "\n";
}

int main(int argc, char** argv) {
    // Create a map to associate primitive names with handler functions
    std::unordered_map<std::string, void (*)(int, char**)> shapeHandlers;
    shapeHandlers["plane"] = handlePlane;
    shapeHandlers["box"] = handleBox;
    shapeHandlers["cone"] = handleCone;
    shapeHandlers["sphere"] = handleSphere;

    // Check if the user requested help
    if (argc == 2 && (std::string(argv[1]) == "--help" || std::string(argv[1]) == "-h")) {
        showUsage(argv[0]);
        return 0;
    }

    // Check if the user provided at least the shape name
    if (argc < 2) {
        std::cerr << "Error: No shape specified.\n";
        showUsage(argv[0]);
        return 1;
    }

    // Get the shape name
    std::string shape = argv[1];

    // Check if the shape is valid
    if (shapeHandlers.find(shape) == shapeHandlers.end()) {
        std::cerr << "Error: Invalid shape '" << shape << "'.\n";
        showUsage(argv[0]);
        return 1;
    }

    // Call the appropriate handler function
    try {
        shapeHandlers[shape](argc, argv);
    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        showUsage(argv[0]);
        return 1;
    }

    return 0;
}