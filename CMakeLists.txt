cmake_minimum_required(VERSION 3.5)
project(sudoku-ar)

# Set C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# OpenCV
find_package( OpenCV REQUIRED )

# Source files
file(GLOB CPP_SOURCES src/*.cpp)

# Add executables
add_executable(${PROJECT_NAME} main.cpp ${CPP_SOURCES})

# Specify include dir
include_directories(src/include)

# Link libraries
target_link_libraries( ${PROJECT_NAME} ${OpenCV_LIBS} )