cmake_minimum_required(VERSION 3.14)
project(glpp)

option(BUILD_EXAMPLES "Enables build of examples" OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
if (NOT TARGET glad)
    FetchContent_Declare(glad GIT_REPOSITORY https://github.com/Dav1dde/glad.git)
    set(GLAD_PROFILE "core")
    FetchContent_MakeAvailable(glad)
endif ()
if (NOT TARGET glm)
    FetchContent_Declare(glm GIT_REPOSITORY https://github.com/g-truc/glm.git)
    FetchContent_MakeAvailable(glm)
endif ()

add_library(glpp INTERFACE)
target_link_libraries(glpp INTERFACE glad glm)
target_include_directories(glpp INTERFACE include)

if(${BUILD_EXAMPLES})
    add_subdirectory(examples)  
endif()
