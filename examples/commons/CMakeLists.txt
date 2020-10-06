add_library(example-commons common.cpp common.hpp)
target_link_libraries(example-commons PUBLIC glpp glfw)
target_include_directories(example-commons PUBLIC .)