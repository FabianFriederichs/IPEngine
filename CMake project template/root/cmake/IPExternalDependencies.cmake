## add all the dependencies here and create single targets which can be used throughout the project

# glm adds an interface target
add_library(glm INTERFACE)
target_include_directories(glm
    INTERFACE
        $<INSTALL_INTERFACE:include>    
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/external_dependencies/glm/>)

target_compile_definitions(glm
    INTERFACE
        GLM_ENABLE_EXPERIMENTAL)   

install(TARGETS glm
    EXPORT glmConfig
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
install(EXPORT glmConfig NAMESPACE glm:: DESTINATION ${CMAKE_INSTALL_PREFIX}/)
install(DIRECTORY external_dependencies/glm/glm/ DESTINATION include/glm)

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

# sdl2, option 1: use a build present on the system: set SDL2_DIR variable to tell cmake the config path
set(SDL2_DIR "" CACHE PATH "SDL root path")
find_package(SDL2 REQUIRED) # target: SDL2::SDL2

# sdl2, option 2: build sdl2 along the project
#add_subdirectory(external_dependencies/SDL2) # target: SDL2
#if(NOT TARGET SDL2::SDL2 AND TARGET SDL2)
#add_library(SDL2::SDL2 ALIAS SDL2) # for consistency with install outpu # target: SDL2::SDL2
#endif()

# openvr
set(OpenVR_ROOT "${CMAKE_SOURCE_DIR}/external_dependencies/openvr")
find_package(OpenVR) # target: OpenVR::OpenVR

# boost
set(Boost_USE_STATIC_LIBS TRUE)
set(Boost_USE_RELEASE_LIBS TRUE)
find_package(Boost REQUIRED COMPONENTS filesystem) # targets: Boost::boost, Boost::filesystem