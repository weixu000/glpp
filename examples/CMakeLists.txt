file(GLOB EXAMPLE_DIRS LIST_DIRECTORIES true CONFIGURE_DEPENDS */)

foreach (EXAMPLE_DIR ${EXAMPLE_DIRS})
    if (IS_DIRECTORY "${EXAMPLE_DIR}")
        add_subdirectory("${EXAMPLE_DIR}")
    endif ()
endforeach ()