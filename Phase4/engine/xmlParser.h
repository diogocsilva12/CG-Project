#ifndef XML_PARSER_H
#define XML_PARSER_H

#include "engine.h"
#include "tinyxml2.h"
#include <string>
#include <vector>

/**
 * @brief Parses an XML configuration file and constructs a World object
 * 
 * @param filename Path to the XML configuration file
 * @return World object containing parsed configuration data
 */
World parseXMLFile(const char* filename);

/**
 * @brief Parses a group element from the XML file
 * 
 * @param groupElement XML element representing a group
 * @param group Group object to populate
 * @param xmlDir Directory containing the XML file (for relative paths)
 */
void parseGroup(tinyxml2::XMLElement* groupElement, Group& group, const std::string& xmlDir);

#endif // XML_PARSER_H