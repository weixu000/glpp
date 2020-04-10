add_library(example-commons common.cpp common.hpp)
target_link_libraries(example-commons PUBLIC glad::glad glpp glfw glm)
target_include_directories(example-commons PUBLIC .)