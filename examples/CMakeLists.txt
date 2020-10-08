file(GLOB EXAMPLE_DIRS LIST_DIRECTORIES true CONFIGURE_DEPENDS */)

include(FetchContent)
if (NOT TARGET glfw)
    FetchContent_Declare(glfw3 GIT_REPOSITORY https://github.com/glfw/glfw.git)
    set(GLFW_INSTALL OFF)
    FetchContent_MakeAvailable(glfw3)
endif ()

add_subdirectory(commons)
add_subdirectory(hello_triangle)
add_subdirectory(many_axes)