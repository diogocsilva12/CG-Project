cmake_minimum_required(VERSION 3.10)
project(CG_Project)

# Set C++11 as required
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

# Find DevIL package for texture loading
if(APPLE)
    # Try to locate IL headers directly
    execute_process(
        COMMAND find /usr/local/include /opt/homebrew/include -name "il.h"
        OUTPUT_VARIABLE IL_HEADER_PATH
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    
    if(IL_HEADER_PATH)
        get_filename_component(IL_INCLUDE_DIR ${IL_HEADER_PATH} DIRECTORY)
        message(STATUS "Found DevIL include at: ${IL_INCLUDE_DIR}")
        include_directories(${IL_INCLUDE_DIR})
    endif()
    
    # Try Homebrew paths for libraries
    find_library(IL_LIBRARY IL HINTS /usr/local/lib /opt/homebrew/lib)
    find_library(ILU_LIBRARY ILU HINTS /usr/local/lib /opt/homebrew/lib)
    find_library(ILUT_LIBRARY ILUT HINTS /usr/local/lib /opt/homebrew/lib)
    
    if(IL_LIBRARY AND ILU_LIBRARY AND ILUT_LIBRARY)
        set(DevIL_LIBRARIES ${IL_LIBRARY} ${ILU_LIBRARY} ${ILUT_LIBRARY})
        set(DevIL_FOUND TRUE)
        message(STATUS "Found DevIL: ${DevIL_LIBRARIES}")
    else()
        message(FATAL_ERROR "DevIL library not found. Please install it: brew install devil")
    endif()

    include_directories("/opt/homebrew/Cellar/devil/1.8.0_6/include")
else()
    # Standard DevIL finding for other platforms
    find_package(DevIL REQUIRED)
endif()

# Add TinyXML2
add_library(tinyxml2 STATIC
    external/tinyxml2/tinyxml2.cpp
    external/tinyxml2/tinyxml2.h
)
target_include_directories(tinyxml2 PUBLIC external/tinyxml2)

# Generator executable
add_executable(generator
    generator/generator.cpp
    generator/generatorAux.cpp
)
set_target_properties(generator PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/generator
)

# Engine executable
add_executable(engine
    engine/engine.cpp
    engine/xmlParser.cpp
    engine/catmullrom.cpp
    engine/texture.cpp
)

target_include_directories(engine PRIVATE 
    engine 
    ${OPENGL_INCLUDE_DIRS} 
    ${GLUT_INCLUDE_DIRS}
    "/opt/homebrew/Cellar/devil/1.8.0_6/include"  # Add this line
)

# Link libraries for engine
target_link_libraries(engine
    PRIVATE
    ${OPENGL_LIBRARIES}
    ${GLUT_LIBRARIES}
    ${IL_LIBRARY}
    ${ILU_LIBRARY}
    ${ILUT_LIBRARY}
    tinyxml2
    IL
    ILU
)

# GLEW for non-Apple platforms
if(NOT APPLE)
    find_package(GLEW REQUIRED)
    include_directories(${GLEW_INCLUDE_DIRS})
    target_link_libraries(engine PRIVATE ${GLEW_LIBRARIES})

endif()


