cmake_minimum_required(VERSION 3.10)
project(CG_Project)

# Set C++11 as required
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

# Handle DevIL cross-platform
if(APPLE)
    # macOS-specific DevIL paths
    find_library(IL_LIBRARY IL HINTS /usr/local/lib /opt/homebrew/lib)
    find_library(ILU_LIBRARY ILU HINTS /usr/local/lib /opt/homebrew/lib)
    find_library(ILUT_LIBRARY ILUT HINTS /usr/local/lib /opt/homebrew/lib)
    
    if(IL_LIBRARY AND ILU_LIBRARY AND ILUT_LIBRARY)
        set(DevIL_FOUND TRUE)
        set(DevIL_INCLUDE_DIR "/opt/homebrew/include") # Default Homebrew path
        
        # Try to find the actual path if the default doesn't work
        execute_process(
            COMMAND find /usr/local/include /opt/homebrew/include -name "il.h"
            OUTPUT_VARIABLE IL_HEADER_PATH
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        
        if(IL_HEADER_PATH)
            get_filename_component(DevIL_INCLUDE_DIR ${IL_HEADER_PATH} DIRECTORY)
        endif()
        
        message(STATUS "Found DevIL: ${IL_LIBRARY};${ILU_LIBRARY};${ILUT_LIBRARY}")
        message(STATUS "DevIL include dir: ${DevIL_INCLUDE_DIR}")
    else()
        message(FATAL_ERROR "DevIL library not found. Please install it: brew install devil")
    endif()
elseif(WIN32)
    # Windows-specific DevIL paths
    # First try to find using CMake's built-in module
    find_package(DevIL QUIET)
    
    if(NOT DevIL_FOUND)
        # Fallback: look in common Windows installation locations
        set(DEVIL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/devil" CACHE PATH "DevIL directory")
        
        # Check for common install paths
        if(EXISTS "${DEVIL_DIR}")
            set(IL_LIBRARY "${DEVIL_DIR}/lib/x64/DevIL.lib" CACHE FILEPATH "IL library")
            set(ILU_LIBRARY "${DEVIL_DIR}/lib/x64/ILU.lib" CACHE FILEPATH "ILU library") 
            set(ILUT_LIBRARY "${DEVIL_DIR}/lib/x64/ILUT.lib" CACHE FILEPATH "ILUT library")
            set(DevIL_INCLUDE_DIR "${DEVIL_DIR}/include" CACHE PATH "DevIL include directory")
            
            if(EXISTS "${IL_LIBRARY}" AND EXISTS "${ILU_LIBRARY}" AND EXISTS "${ILUT_LIBRARY}")
                set(DevIL_FOUND TRUE)
                message(STATUS "Found DevIL in user-specified directory: ${DEVIL_DIR}")
            else()
                message(FATAL_ERROR "DevIL libraries not found in ${DEVIL_DIR}/lib/x64. Please set DEVIL_DIR to your DevIL installation directory.")
            endif()
        else()
            message(FATAL_ERROR "DevIL directory not found. Please set DEVIL_DIR to your DevIL installation directory.")
        endif()
    endif()
else()
    # Linux and other platforms
    find_package(DevIL REQUIRED)
endif()

# Set DevIL include directories
include_directories(${DevIL_INCLUDE_DIR})

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
    ${DevIL_INCLUDE_DIR}
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
)

# GLEW for Windows and non-Apple platforms
if(NOT APPLE)
    find_package(GLEW REQUIRED)
    include_directories(${GLEW_INCLUDE_DIRS})
    target_link_libraries(engine
    PRIVATE
    ${GLEW_LIBRARIES}
    IL
    ILU
    ILUT
    )
endif()

# Copy DLLs to binary directory on Windows
if(WIN32)
    # Add commands to copy DevIL DLLs after build
    add_custom_command(TARGET engine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${DEVIL_DIR}/lib/x64/DevIL.dll"
            "${DEVIL_DIR}/lib/x64/ILU.dll"
            "${DEVIL_DIR}/lib/x64/ILUT.dll"
            $<TARGET_FILE_DIR:engine>
        COMMENT "Copying DevIL DLLs to output directory"
    )
endif()