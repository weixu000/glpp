file(GLOB EXAMPLE_DIRS LIST_DIRECTORIES true CONFIGURE_DEPENDS */)

include(FetchContent)
if (NOT TARGET glfw)
    FetchContent_Declare(glfw3 GIT_REPOSITORY https://github.com/glfw/glfw.git)
    FetchContent_MakeAvailable(glfw3)
endif ()

foreach (EXAMPLE_DIR ${EXAMPLE_DIRS})
    if (IS_DIRECTORY "${EXAMPLE_DIR}")
        add_subdirectory("${EXAMPLE_DIR}")
    endif ()
endforeach ()