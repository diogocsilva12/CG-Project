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
    engine/xmlParser.cpp)

# Include directories for engine
target_include_directories(engine PRIVATE engine ${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS})

# Link libraries for engine
target_link_libraries(engine
    PRIVATE
    ${OPENGL_LIBRARIES}
    ${GLUT_LIBRARIES}
    tinyxml2)

# Add to your CMakeLists.txt
if(NOT APPLE)
    find_package(GLEW REQUIRED)
    include_directories(${GLEW_INCLUDE_DIRS})
    target_link_libraries(engine ${GLEW_LIBRARIES})
endif()
