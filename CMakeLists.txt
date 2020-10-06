cmake_minimum_required(VERSION 3.14)
project(glpp)

option(BUILD_EXAMPLES "Enables build of examples" OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(extern/glad.cmake)
include(extern/glm.cmake)

file(GLOB_RECURSE GLAD_HEADERS CONFIGURE_DEPENDS ${GLAD_INCLUDE_PATH}/*.h)
file(GLOB_RECURSE GLAD_SOURCES CONFIGURE_DEPENDS ${GLAD_SOURCE_PATH}/*.c)
file(GLOB_RECURSE GLPP_HEADERS CONFIGURE_DEPENDS include/*.hpp)
add_library(glpp STATIC ${GLAD_SOURCES} ${GLAD_HEADERS} ${GLPP_HEADERS})
target_link_libraries(glpp PUBLIC glm)
target_include_directories(glpp PUBLIC include ${GLAD_INCLUDE_PATH})
set_target_properties(glpp PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF)

if (${BUILD_EXAMPLES})
    add_subdirectory(examples)
endif ()
