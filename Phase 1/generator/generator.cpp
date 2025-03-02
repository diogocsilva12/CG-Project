#include <iostream>
#include <string>
#include <unordered_map>
#include <stdexcept>
#include "generatorAux.h"



/**
 * @brief Handles the generation of a plane primitive
 * 
 * Creates a plane centered at the origin with specified dimensions and complexity.
 * The plane is constructed in the XZ plane (with Y as up axis).
 * 
 * @param argc The total number of command line arguments
 * @param argv The array of command line arguments
 *        argv[2]: unit - The half-length of the plane sides
 *        argv[3]: slices - The number of divisions along each axis
 *        argv[4]: output filename (ignored; standardized name is used)
 * @throws std::invalid_argument If the required parameters are missing or invalid
 */
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

/**
 * @brief Handles the generation of a box primitive
 * 
 * Creates a box centered at the origin with specified dimensions and complexity.
 * Each face of the box is divided according to the slices parameter.
 * 
 * @param argc The total number of command line arguments
 * @param argv The array of command line arguments
 *        argv[2]: unit - The half-length of the box sides
 *        argv[3]: slices - The number of divisions along each axis of each face
 *        argv[4]: output filename (ignored; standardized name is used)
 * @throws std::invalid_argument If the required parameters are missing or invalid
 */
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

/**
 * @brief Handles the generation of a cone primitive
 * 
 * Creates a cone with its base centered at the origin and apex along the positive Y axis.
 * The cone's complexity is controlled by slices (around the circular base) and stacks (along height).
 * 
 * @param argc The total number of command line arguments
 * @param argv The array of command line arguments
 *        argv[2]: radius - The radius of the cone base
 *        argv[3]: height - The height of the cone
 *        argv[4]: slices - The number of divisions around the circular base
 *        argv[5]: stacks - The number of divisions along the height
 *        argv[6]: output filename (ignored; standardized name is used)
 * @throws std::invalid_argument If the required parameters are missing or invalid
 */
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

/**
 * @brief Handles the generation of a sphere primitive
 * 
 * Creates a sphere centered at the origin with specified radius and complexity.
 * The sphere's complexity is controlled by slices (longitude) and stacks (latitude).
 * 
 * @param argc The total number of command line arguments
 * @param argv The array of command line arguments
 *        argv[2]: radius - The radius of the sphere
 *        argv[3]: slices - The number of divisions along the longitude (around vertical axis)
 *        argv[4]: stacks - The number of divisions along the latitude (from pole to pole)
 *        argv[5]: output filename (ignored; standardized name is used) 
 * @throws std::invalid_argument If the required parameters are missing or invalid
 */
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

/**
 * @brief Main function - program entry point
 * 
 * Parses command-line arguments and dispatches to the appropriate shape handler function.
 * Uses a map to associate primitive names with their handler functions for extensibility.
 * 
 * @param argc The total number of command line arguments
 * @param argv The array of command line arguments
 *        argv[0]: program name
 *        argv[1]: shape type (plane, box, cone, sphere)
 *        argv[2+]: shape-specific parameters
 * @return 0 on successful execution, 1 on error
 */
int main(int argc, char** argv) {
    std::unordered_map<std::string, void (*)(int, char**)> shapeHandlers;
    shapeHandlers["plane"] = handlePlane;
    shapeHandlers["box"] = handleBox;
    shapeHandlers["cone"] = handleCone;
    shapeHandlers["sphere"] = handleSphere;

    //Controle de erros
    if (argc < 2) {
        std::cerr << "Error: No shape specified.\n";
        showUsage(argv[0]);
        return 1;
    }

    std::string shape = argv[1];

    //Valida um input
    if (shapeHandlers.find(shape) == shapeHandlers.end()) {
        std::cerr << "Error: Invalid shape '" << shape << "'.\n";
        showUsage(argv[0]);
        return 1;
    }

    //
    try {
        shapeHandlers[shape](argc, argv);
    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        showUsage(argv[0]);
        return 1;
    }

    return 0;
}