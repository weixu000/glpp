include(FetchContent)
if (NOT TARGET glfw)
    FetchContent_Declare(glfw3 GIT_REPOSITORY https://github.com/glfw/glfw.git)
    set(GLFW_INSTALL OFF)
    set(GLFW_BUILD_DOCS OFF)
    set(GLFW_BUILD_EXAMPLES OFF)
    FetchContent_MakeAvailable(glfw3)
endif ()

add_subdirectory(commons)
add_subdirectory(hello_triangle)
add_subdirectory(many_axes)
add_subdirectory(hello_opengl)
add_subdirectory(many_dices)
add_subdirectory(n_body)