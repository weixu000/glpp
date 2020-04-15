file(GLOB_RECURSE GLPP_HEADERS CONFIGURE_DEPENDS ../include/*.hpp)
file(GLOB_RECURSE GLPP_SOURCES CONFIGURE_DEPENDS *.cpp)

add_library(glpp INTERFACE)
target_link_libraries(glpp INTERFACE glad glm)
target_include_directories(glpp INTERFACE ../include)