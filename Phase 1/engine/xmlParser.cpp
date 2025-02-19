#include "tinyxml2.h"
#include "engine.h"
#include <iostream>

using namespace tinyxml2;

World parseXMLFile(const char* filename) {
    World world;
    XMLDocument doc;
    
    // Load XML file
    if (doc.LoadFile(filename) != XML_SUCCESS) {
        std::cerr << "Error loading XML file!" << std::endl;
        return world;
    }

    XMLElement* rootElement = doc.RootElement();
    
    // Parse window settings
    XMLElement* windowElement = rootElement->FirstChildElement("window");
    if (windowElement) {
        world.window.width = windowElement->IntAttribute("width");
        world.window.height = windowElement->IntAttribute("height");
    }

    // Parse camera settings
    XMLElement* cameraElement = rootElement->FirstChildElement("camera");
    if (cameraElement) {
        XMLElement* posElement = cameraElement->FirstChildElement("position");


       Point position = {
            posElement->FloatAttribute("x"),
            posElement->FloatAttribute("y"),
            posElement->FloatAttribute("z")
        };
        world.camera.position = position;

        XMLElement* lookAtElement = cameraElement->FirstChildElement("lookAt");
        // Create Point struct for lookAt
        Point lookAt = {
            lookAtElement->FloatAttribute("x"),
            lookAtElement->FloatAttribute("y"),
            lookAtElement->FloatAttribute("z")
        };
        world.camera.lookAt = lookAt;

        XMLElement* upElement = cameraElement->FirstChildElement("up");
        // Create Point struct for up vector
        Point up = {
            upElement->FloatAttribute("x"),
            upElement->FloatAttribute("y"),
            upElement->FloatAttribute("z")
        };
        world.camera.up = up;

        XMLElement* projElement = cameraElement->FirstChildElement("projection");
        world.camera.fov = projElement->FloatAttribute("fov");
        world.camera.near = projElement->FloatAttribute("near");
        world.camera.far = projElement->FloatAttribute("far");
    }

    // Parse models
    XMLElement* groupElement = rootElement->FirstChildElement("group");
    if (groupElement) {
        XMLElement* modelsElement = groupElement->FirstChildElement("models");
        if (modelsElement) {
            for (XMLElement* modelElement = modelsElement->FirstChildElement("model");
                 modelElement;
                 modelElement = modelElement->NextSiblingElement("model")) {
                Model model;
                model.filename = modelElement->Attribute("file");
                world.models.push_back(model);
            }
        }
    }

    return world;
}



/*This code reads the XML file and extracts:

Camera position (x,y,z)
Look-at point (x,y,z)
Up vector (x,y,z)
Field of view (fov), near and far planes
For example, when parsing this XML:

It will create Point structures with those values and store them in the World structure.*/