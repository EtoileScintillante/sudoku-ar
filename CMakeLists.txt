cmake_minimum_required(VERSION 3.2)
project(sudoku-ar)
find_package( OpenCV REQUIRED )
include_directories(src/include)
add_executable(Runner
  main.cpp
  src/sudoku-processing.cpp
  src/sudoku-solver.cpp
)
target_link_libraries( Runner ${OpenCV_LIBS} )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")