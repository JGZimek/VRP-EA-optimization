cmake_minimum_required(VERSION 3.10)

# Project name and version
project(VRP_EA_Optimization VERSION 1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set compile flags for strict warnings
if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -pedantic)
endif()

# Include directories for header files
include_directories(${PROJECT_SOURCE_DIR}/include)

# Gather all source files from the src directory
file(GLOB_RECURSE SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")

# Define the executable target
add_executable(${PROJECT_NAME} ${SOURCES})

# Set the output directory for the executable
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin"
)

# Enable testing and add the test subdirectory
enable_testing()
add_subdirectory(test)
