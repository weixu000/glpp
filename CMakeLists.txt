cmake_minimum_required(VERSION 3.10)
project(glpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(glad REQUIRED)
find_package(glm REQUIRED)

add_subdirectory(src)
add_subdirectory(examples)