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


// Add this new function to parse a group and its transformations
void parseGroup(XMLElement* groupElement, Group& group, const std::string& xmlDir) {
    // Parse transform elements
    XMLElement* transformElement = groupElement->FirstChildElement("transform");
    if (transformElement) {
        // Process all child elements in order
        for (XMLElement* elem = transformElement->FirstChildElement(); elem; elem = elem->NextSiblingElement()) {
            std::string elemName = elem->Name();
            
            if (elemName == "translate") {
                if (elem->Attribute("time")) {
                    group.transform.hasCurve = true;
                    group.transform.curveTime = elem->FloatAttribute("time");
                    group.transform.align = elem->BoolAttribute("align", false);
                    // Parse points
                    for (XMLElement* pointElem = elem->FirstChildElement("point"); pointElem; pointElem = pointElem->NextSiblingElement("point")) {
                        Point p;
                        p.x = pointElem->FloatAttribute("x");
                        p.y = pointElem->FloatAttribute("y");
                        p.z = pointElem->FloatAttribute("z");
                        group.transform.curvePoints.push_back(p);
                    }
                    group.transformOrder.push_back("translate_curve");
                } else {
                    group.transform.translateX = elem->FloatAttribute("x", 0.0f);
                    group.transform.translateY = elem->FloatAttribute("y", 0.0f);
                    group.transform.translateZ = elem->FloatAttribute("z", 0.0f);
                    group.transformOrder.push_back("translate");
                }
            }
            else if (elemName == "rotate") {
                if (elem->Attribute("time")) {
                    group.transform.timeRotation = true;
                    group.transform.rotationTime = elem->FloatAttribute("time");
                    group.transform.rotateX = elem->FloatAttribute("x", 0.0f);
                    group.transform.rotateY = elem->FloatAttribute("y", 0.0f);
                    group.transform.rotateZ = elem->FloatAttribute("z", 0.0f);
                    group.transformOrder.push_back("rotate_time");
                } else {
                    group.transform.rotateAngle = elem->FloatAttribute("angle", 0.0f);
                    group.transform.rotateX = elem->FloatAttribute("x", 0.0f);
                    group.transform.rotateY = elem->FloatAttribute("y", 0.0f);
                    group.transform.rotateZ = elem->FloatAttribute("z", 0.0f);
                    group.transformOrder.push_back("rotate");
                }
            }
            else if (elemName == "scale") {
                group.transform.scaleX = elem->FloatAttribute("x", 1.0f);
                group.transform.scaleY = elem->FloatAttribute("y", 1.0f);
                group.transform.scaleZ = elem->FloatAttribute("z", 1.0f);
                group.transformOrder.push_back("scale");
            }
        }
    }
    
    // Parse models
    XMLElement* modelsElement = groupElement->FirstChildElement("models");
    if (modelsElement) {
        for (XMLElement* modelElement = modelsElement->FirstChildElement("model");
             modelElement;
             modelElement = modelElement->NextSiblingElement("model")) {
            
            Model model;
            std::string modelFile = modelElement->Attribute("file");
            
            model.filename = "../tests/" + modelFile;
            
            std::cout << "Loading model from: " << model.filename << std::endl;
            
            group.models.push_back(model);
        }
    }
    
    // Parse child groups recursively
    for (XMLElement* childGroupElement = groupElement->FirstChildElement("group");
         childGroupElement;
         childGroupElement = childGroupElement->NextSiblingElement("group")) {
        
        Group childGroup;
        parseGroup(childGroupElement, childGroup, xmlDir);
        group.childGroups.push_back(childGroup);
    }
}


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

    // Parse the root group
    XMLElement* rootGroupElement = rootElement->FirstChildElement("group");
    if (rootGroupElement) {
        parseGroup(rootGroupElement, world.rootGroup, xmlDir);
    }

    return world;
}


