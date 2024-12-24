cmake_minimum_required(VERSION 3.10)

project(TxtGame VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set source directory
set( TXT_GAME_SRC src/*.cpp )

# Collect source files
file( GLOB_RECURSE TXT_GAME_SRC "src/*.cpp" )

# Add executable
add_executable(${PROJECT_NAME} ${TXT_GAME_SRC})

# Add include directory
target_include_directories(${PROJECT_NAME} PRIVATE ${SRC_DIR})
