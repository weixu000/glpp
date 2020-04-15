cmake_minimum_required(VERSION 3.14)
project(glpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (NOT TARGET glad)
    include(FetchContent)
    FetchContent_Declare(glad GIT_REPOSITORY https://github.com/Dav1dde/glad.git)
    FetchContent_MakeAvailable(glad)
endif ()
find_package(glm REQUIRED)

add_subdirectory(src)
add_subdirectory(examples)