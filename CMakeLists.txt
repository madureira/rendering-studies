cmake_minimum_required(VERSION 3.24)
project(rendering-studies)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DDEBUG)
endif()

# Find libraries
find_package(assimp REQUIRED)
find_package(freetype REQUIRED)
find_package(glad REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(spdlog REQUIRED)

# Add all .cpp files in the src directory and its subdirectories
file(GLOB_RECURSE CPP_SOURCES src/*.cpp)

# Add all .h files in the src directory and its subdirectories
file(GLOB_RECURSE HEADER_FILES src/*.h)

# Create the library target
add_executable(${PROJECT_NAME} src/main.cpp ${CPP_SOURCES} ${HEADER_FILES})

# Copy assets to the build directory
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR}/)

# Copy config.ini to the build directory
file(COPY config.ini DESTINATION ${CMAKE_BINARY_DIR}/)

# Link libraries to the target
target_link_libraries(${PROJECT_NAME} assimp::assimp)
target_link_libraries(${PROJECT_NAME} Freetype::Freetype)
target_link_libraries(${PROJECT_NAME} glad::glad)
target_link_libraries(${PROJECT_NAME} glfw)
target_link_libraries(${PROJECT_NAME} glm::glm)
target_link_libraries(${PROJECT_NAME} spdlog::spdlog)

target_include_directories(${PROJECT_NAME} PRIVATE includes)
