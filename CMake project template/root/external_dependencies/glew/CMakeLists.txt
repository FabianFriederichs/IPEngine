# this is not an original glew file! just for convenient dll building!

cmake_minimum_required(VERSION 3.0)
project(glew)

add_library(glew SHARED include/GL/glew.h src/glew.c)

target_include_directories(glew
    PUBLIC 
        $<INSTALL_INTERFACE:include>    
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_compile_definitions(glew
    PRIVATE
        GLEW_BUILD
)

# find opengl
find_package(OpenGL REQUIRED)
target_link_libraries(glew OpenGL::GL)

install(TARGETS glew
        EXPORT glewConfig
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
)

install(DIRECTORY include/ DESTINATION include)

install(EXPORT glewConfig NAMESPACE glew:: DESTINATION ${CMAKE_INSTALL_PREFIX}/)

add_library(glew::glew ALIAS glew)