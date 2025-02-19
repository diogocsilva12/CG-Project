cmake_minimum_required(VERSION 3.10)
project(CG_Project)

# Set C++11 as required
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

# Add TinyXML2
add_library(tinyxml2 STATIC
    external/tinyxml2/tinyxml2.cpp
    external/tinyxml2/tinyxml2.h
)
target_include_directories(tinyxml2 PUBLIC external/tinyxml2)

# Create tests directory
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/generator/tests)

# Generator executable
add_executable(generator 
    generator/generator.cpp
    generator/generatorAux.cpp)

# Set working directory for generator
set_target_properties(generator PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/generator)

# Engine executable
add_executable(engine
    engine/engine.cpp
    engine/xmlparser.cpp)

target_link_libraries(engine
    PRIVATE
    OpenGL::GL
    GLUT::GLUT
    tinyxml2)

# Include directories for engine
target_include_directories(engine PRIVATE engine)