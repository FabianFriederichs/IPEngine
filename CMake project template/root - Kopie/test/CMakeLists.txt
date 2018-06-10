cmake_minimum_required(VERSION 3.0)
project(testpro)

add_executable(test test.cpp)

target_link_libraries(test
    glm
    moodycamel_queue
    stb_image
    glew
    Vulkan::Vulkan
    SDL2::SDL2
    OpenVR)