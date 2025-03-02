/**
 * @file xmlParser.cpp
 * @brief XML configuration parser for 3D graphics engine
 * 
 * This file implements functionality to parse XML configuration files that define
 * camera settings, window dimensions, and 3D model references for the rendering engine.
 */

#include "tinyxml2.h"
#include "engine.h"
#include <iostream>

using namespace tinyxml2;

/**
 * @brief Parses an XML configuration file and constructs a World object
 * 
 * This function reads an XML file that defines:
 * - Window dimensions (width and height)
 * - Camera parameters (position, lookAt point, up vector, projection settings)
 * - 3D models to be rendered
 * 
 * XML structure expected:
 * <world>
 *   <window width="X" height="Y" />
 *   <camera>
 *     <position x="X" y="Y" z="Z" />
 *     <lookAt x="X" y="Y" z="Z" />
 *     <up x="X" y="Y" z="Z" />
 *     <projection fov="F" near="N" far="F" />
 *   </camera>
 *   <group>
 *     <models>
 *       <model file="filename.3d" />
 *       ...
 *     </models>
 *   </group>
 * </world>
 * 
 * @param filename Path to the XML configuration file
 * @return World object containing parsed configuration data
 */
World parseXMLFile(const char* filename) {
    World world;
    XMLDocument doc;
    
    //Procura o ficheiro XML 
    std::string xmlPath = filename;
    size_t lastSlash = xmlPath.find_last_of("/\\");
    std::string xmlDir = (lastSlash != std::string::npos) ? 
                        xmlPath.substr(0, lastSlash + 1) : "";
    
    //Carrega o ficheiro XML 
    if (doc.LoadFile(filename) != XML_SUCCESS) {
        std::cerr << "Error loading XML file: " << filename << std::endl;
        return world;
    }

    XMLElement* rootElement = doc.RootElement();
    
    /**
     * Parse window settings
     * 
     * Example: <window width="512" height="512" />
     * 
     * Extracts width and height attributes to define the window dimensions.
     */
    XMLElement* windowElement = rootElement->FirstChildElement("window");
    if (windowElement) {
        world.window.width = windowElement->IntAttribute("width");
        world.window.height = windowElement->IntAttribute("height");
    }

    /**
     * Parse camera settings
     * 
     * Example:
     * <camera>
     *   <position x="5" y="-2" z="3" />
     *   <lookAt x="0" y="0" z="0" />
     *   <up x="0" y="1" z="0" />
     *   <projection fov="60" near="1" far="1000" />
     * </camera>
     * 
     * Extracts camera position, lookAt point, up vector, and projection parameters.
     */
    XMLElement* cameraElement = rootElement->FirstChildElement("camera");
    if (cameraElement) {
        // Parse camera position (x,y,z coordinates)
        XMLElement* posElement = cameraElement->FirstChildElement("position");
        Point position = {
            posElement->FloatAttribute("x"),
            posElement->FloatAttribute("y"),
            posElement->FloatAttribute("z")
        };
        world.camera.position = position;

        // Parse lookAt point (x,y,z coordinates)
        XMLElement* lookAtElement = cameraElement->FirstChildElement("lookAt");
        Point lookAt = {
            lookAtElement->FloatAttribute("x"),
            lookAtElement->FloatAttribute("y"),
            lookAtElement->FloatAttribute("z")
        };
        world.camera.lookAt = lookAt;

        // Parse up vector (x,y,z coordinates)
        XMLElement* upElement = cameraElement->FirstChildElement("up");
        Point up = {
            upElement->FloatAttribute("x"),
            upElement->FloatAttribute("y"),
            upElement->FloatAttribute("z")
        };
        world.camera.up = up;

        // Parse projection parameters (field of view, near and far planes)
        XMLElement* projElement = cameraElement->FirstChildElement("projection");
        world.camera.fov = projElement->FloatAttribute("fov");
        world.camera.near = projElement->FloatAttribute("near");
        world.camera.far = projElement->FloatAttribute("far");
    }

    /**
     * Parse 3D model references
     * 
     * Example:
     * <group>
     *   <models>
     *     <model file="cone_1_2_4_3.3d" />
     *   </models>
     * </group>
     * 
     * Extracts model filenames and constructs paths to the 3D model files.
     */
    XMLElement* groupElement = rootElement->FirstChildElement("group");
    if (groupElement) {
        XMLElement* modelsElement = groupElement->FirstChildElement("models");
        if (modelsElement) {
            // Iterate through all model elements
            for (XMLElement* modelElement = modelsElement->FirstChildElement("model");
                 modelElement;
                 modelElement = modelElement->NextSiblingElement("model")) {
                
                Model model;
                std::string modelFile = modelElement->Attribute("file");
                
                model.filename = "../tests/" + modelFile;
                
                std::cout << "Loading model from: " << model.filename << std::endl;
                
                world.models.push_back(model);
            }
        }
    }

    return world;
}
