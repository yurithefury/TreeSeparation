cmake_minimum_required (VERSION 3.10)

project ("TreeSeparation")

set(CMAKE_CXX_STANDARD 14)

add_executable (TreeSeparation "Source.cpp" "FoxTree.cpp" "FoxTree.h" "nanoflann.hpp")
