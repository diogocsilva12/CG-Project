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
                    // Lê o atributo draw (por defeito true)
                    const char* drawAttr = elem->Attribute("draw");
                    group.transform.drawCurve = true;
                    if (drawAttr && std::string(drawAttr) == "false")
                        group.transform.drawCurve = false;
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
            // Adiciona o parsing da textura:
            XMLElement* textureElem = modelElement->FirstChildElement("texture");
            if (textureElem) {
                const char* texFile = textureElem->Attribute("file");
                if (texFile) {
                    model.textureFile = "../engine/textures/" + std::string(texFile);
                    std::cout << "Associating texture: " << model.textureFile << " with model: " << model.filename << std::endl;
                }
            }

            // Parse material/color information
            XMLElement* colorElem = modelElement->FirstChildElement("color");
            if (colorElem) {
                // Parse diffuse color
                XMLElement* diffuseElem = colorElem->FirstChildElement("diffuse");
                if (diffuseElem) {
                    int r = 200, g = 200, b = 200; // Default values
                    diffuseElem->QueryIntAttribute("R", &r);
                    diffuseElem->QueryIntAttribute("G", &g);
                    diffuseElem->QueryIntAttribute("B", &b);
                    
                    // Convert from 0-255 integer range to 0.0-1.0 float range that OpenGL expects
                    model.material.diffuse.r = r / 255.0f;
                    model.material.diffuse.g = g / 255.0f;
                    model.material.diffuse.b = b / 255.0f;
                    
                    // Debug output
                    std::cout << "Parsed diffuse color: RGB(" << r << "," << g << "," << b << ") -> "
                              << "(" << model.material.diffuse.r << "," 
                              << model.material.diffuse.g << "," 
                              << model.material.diffuse.b << ")" << std::endl;
                } else {
                    // Default diffuse to light gray if not specified
                    model.material.diffuse.r = 0.78f;
                    model.material.diffuse.g = 0.78f;
                    model.material.diffuse.b = 0.78f;
                }
                
                // Parse ambient color
                XMLElement* ambientElem = colorElem->FirstChildElement("ambient");
                if (ambientElem) {
                    int r = 50, g = 50, b = 50;
                    ambientElem->QueryIntAttribute("R", &r);
                    ambientElem->QueryIntAttribute("G", &g);
                    ambientElem->QueryIntAttribute("B", &b);
                    model.material.ambient.r = r / 255.0f;
                    model.material.ambient.g = g / 255.0f;
                    model.material.ambient.b = b / 255.0f;
                } else {
                    // Default ambient to dark gray if not specified
                    model.material.ambient.r = 0.2f;
                    model.material.ambient.g = 0.2f;
                    model.material.ambient.b = 0.2f;
                }
                
                // Parse specular color
                XMLElement* specularElem = colorElem->FirstChildElement("specular");
                if (specularElem) {
                    int r = 0, g = 0, b = 0;
                    specularElem->QueryIntAttribute("R", &r);
                    specularElem->QueryIntAttribute("G", &g);
                    specularElem->QueryIntAttribute("B", &b);
                    model.material.specular.r = r / 255.0f;
                    model.material.specular.g = g / 255.0f;
                    model.material.specular.b = b / 255.0f;
                } else {
                    // Default specular to black if not specified
                    model.material.specular.r = 0.0f;
                    model.material.specular.g = 0.0f;
                    model.material.specular.b = 0.0f;
                }
                
                // Parse emissive color
                XMLElement* emissiveElem = colorElem->FirstChildElement("emissive");
                if (emissiveElem) {
                    int r = 0, g = 0, b = 0;
                    emissiveElem->QueryIntAttribute("R", &r);
                    emissiveElem->QueryIntAttribute("G", &g);
                    emissiveElem->QueryIntAttribute("B", &b);
                    model.material.emissive.r = r / 255.0f;
                    model.material.emissive.g = g / 255.0f;
                    model.material.emissive.b = b / 255.0f;
                } else {
                    // Default emissive to black if not specified
                    model.material.emissive.r = 0.0f;
                    model.material.emissive.g = 0.0f;
                    model.material.emissive.b = 0.0f;
                }
                
                // Parse shininess
                XMLElement* shininessElem = colorElem->FirstChildElement("shininess");
                if (shininessElem) {
                    model.material.shininess = shininessElem->FloatAttribute("value", 0.0f);
                } else {
                    // Default shininess to 0 if not specified
                    model.material.shininess = 0.0f;
                }
                
                // Debug output for material properties
                std::cout << "Model: " << model.filename << " Material properties:" << std::endl;
                std::cout << "  Diffuse: (" << model.material.diffuse.r << ", " 
                          << model.material.diffuse.g << ", " << model.material.diffuse.b << ")" << std::endl;
                std::cout << "  Ambient: (" << model.material.ambient.r << ", " 
                          << model.material.ambient.g << ", " << model.material.ambient.b << ")" << std::endl;
                std::cout << "  Specular: (" << model.material.specular.r << ", " 
                          << model.material.specular.g << ", " << model.material.specular.b << ")" << std::endl;
                std::cout << "  Shininess: " << model.material.shininess << std::endl;
            } else {
                // Set default material properties if color element is not present
                model.material.diffuse.r = 0.78f;
                model.material.diffuse.g = 0.78f;
                model.material.diffuse.b = 0.78f;
                
                model.material.ambient.r = 0.2f;
                model.material.ambient.g = 0.2f;
                model.material.ambient.b = 0.2f;
                
                model.material.specular.r = 0.0f;
                model.material.specular.g = 0.0f;
                model.material.specular.b = 0.0f;
                
                model.material.emissive.r = 0.0f;
                model.material.emissive.g = 0.0f;
                model.material.emissive.b = 0.0f;
                
                model.material.shininess = 0.0f;
            }
            
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
     * Parse skybox settings
     * 
     * Example: <skybox texture="skybox_texture.png" />
     * 
     * Extracts the texture attribute to define the skybox texture.
     */
    XMLElement* skyboxElement = rootElement->FirstChildElement("skybox");
    if (skyboxElement) {
        const char* texture = skyboxElement->Attribute("texture");
        if (texture) {
            world.skyboxTexture = "../engine/textures/" + std::string(texture);
            std::cout << "Loading skybox texture: " << world.skyboxTexture << std::endl;
        }
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

    // Parse lights
    XMLElement* lightsElement = rootElement->FirstChildElement("lights");
    if (lightsElement) {
        for (XMLElement* lightElem = lightsElement->FirstChildElement("light");
                lightElem;
                lightElem = lightElem->NextSiblingElement("light")) {
            Light light;
            const char* type = lightElem->Attribute("type");
            if (type) light.type = std::string(type);
            
            // Add this line to parse the intensity attribute
            light.intensity = lightElem->FloatAttribute("intensity", 1.0f);
            
            // Normalize light type (handle "spot" as "spotlight")
            if (light.type == "spot") {
                light.type = "spotlight";
            }
            
            // Parse position for point and spot lights
            if (light.type == "point" || light.type == "spotlight") {
                light.posx = lightElem->FloatAttribute("posx", 0.0f);
                light.posy = lightElem->FloatAttribute("posy", 0.0f);
                light.posz = lightElem->FloatAttribute("posz", 0.0f);
            }
            
            // Parse direction for directional and spot lights
            if (light.type == "directional" || light.type == "spotlight") {
                light.dirx = lightElem->FloatAttribute("dirx", 0.0f);
                light.diry = lightElem->FloatAttribute("diry", 0.0f);
                light.dirz = lightElem->FloatAttribute("dirz", 0.0f);
            }
            
            // Parse cutoff for spotlight
            if (light.type == "spotlight") {
                light.cutoff = lightElem->FloatAttribute("cutoff", 45.0f);
            }
            
            world.lights.push_back(light);
            
            // Update debug message to include intensity
            std::cout << "Added " << light.type << " light";
            std::cout << " with intensity " << light.intensity;
            if (light.type == "point" || light.type == "spotlight") {
                std::cout << " at position (" << light.posx << ", " << light.posy << ", " << light.posz << ")";
            }
            if (light.type == "directional" || light.type == "spotlight") {
                std::cout << " with direction (" << light.dirx << ", " << light.diry << ", " << light.dirz << ")";
            }
            if (light.type == "spotlight") {
                std::cout << " and cutoff " << light.cutoff;
            }
            std::cout << std::endl;
        }
    }

    // Parse the root group
    XMLElement* rootGroupElement = rootElement->FirstChildElement("group");
    if (rootGroupElement) {
        parseGroup(rootGroupElement, world.rootGroup, xmlDir);
    }
    
    return world;
}


