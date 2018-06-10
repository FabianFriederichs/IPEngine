## add an interface target for each dependency. if a dependency should be built
#  along the project, that can be done here too.
#  make sure everything is exported as target

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/external_dependencies/cmake/")

# glm adds an interface target
add_subdirectory(external_dependencies/glm) # target: glm

# moodycamels mpmc queue
add_subdirectory(external_dependencies/moodycamel) # target: moodycamel_queue

# stb_image build dll
add_subdirectory(external_dependencies/stb_image) # target: stb::stb_image

# opengl, just in case, glew does that usually for you
find_package(OpenGL REQUIRED) # target: OpenGL::GL

# glew build dll
add_subdirectory(external_dependencies/glew) # target: glew::glew

# vulkan sould be integrated in current cmale installs
find_package(Vulkan) # target: Vulkan::Vulkan

# sdl2 set SDL2_DIR variable to tell cmake the config path
find_package(SDL2 REQUIRED) # target: SDL2::SDL2

# openvr
set(OpenVR_ROOT "${CMAKE_SOURCE_DIR}/external_dependencies/openvr")
find_package(OpenVR) # target: OpenVR::OpenVR