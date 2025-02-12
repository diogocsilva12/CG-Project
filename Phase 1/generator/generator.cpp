#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>
#include <iostream>
#include <cstring>
#include <algorithm>
#include <fstream>
#include <sstream>
#include "generatorAux.h"
#include <unordered_map>


int main(int argc, char** argv)
{

    //Creates a dict to store a pair <figura,id>
    std::unordered_map<std::string, int> figuraMap;
    figuraMap.insert(std::make_pair("plane", 1));
    figuraMap.insert(std::make_pair("box", 2));
    figuraMap.insert(std::make_pair("cone", 3));
    figuraMap.insert(std::make_pair("sphere", 4));

    std::string figura; // Added declaration for figura
    std::string ficheiro;// Added declaration for ficheiro

    float unit, radius, height;
    int slices, stacks;

    if (argc > 1) {
        figura = argv[1];
        int figuraType = figuraMap[figura];

        switch (figuraType) {
            case 1: // plane
                if (argc == 5) {
                    unit = std::stof(argv[2]);
                    slices = std::stoi(argv[3]);
                    ficheiro = "tests/" + std::string(argv[4]);
                    plane(unit, slices, ficheiro);
                } else {
                    std::cout << "numero incorreto de argumentos\n";
                }
                break;
            case 2: // box
                if (argc == 5) {
                    unit = std::stof(argv[2]);
                    slices = std::stoi(argv[3]);
                    ficheiro = "tests/" + std::string(argv[4]);
                    box(unit, slices, ficheiro);
                } else {
                    std::cout << "numero incorreto de argumentos\n";
                }
                break;
            case 3: // cone
                if (argc == 7) {
                    radius = std::stof(argv[2]);
                    height = std::stof(argv[3]);
                    slices = std::stoi(argv[4]);
                    stacks = std::stoi(argv[5]);
                    ficheiro = "tests/" + std::string(argv[6]);
                    cone(radius, height, slices, stacks, ficheiro);
                } else {
                    std::cout << "numero incorreto de argumentos\n";
                }
                break;
            case 4: // sphere
                if (argc == 6) {
                    radius = std::stof(argv[2]);
                    slices = std::stoi(argv[3]);
                    stacks = std::stoi(argv[4]);
                    ficheiro = "tests/" + std::string(argv[5]);
                    sphere(radius, slices, stacks, ficheiro);
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