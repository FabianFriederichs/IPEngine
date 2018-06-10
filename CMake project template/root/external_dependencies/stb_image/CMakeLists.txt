cmake_minimum_required(VERSION 3.0)
project(stb_image)

add_library(stb_image STATIC include/stb_image.h src/stb_image.c)

target_include_directories(stb_image
    PUBLIC 
        $<INSTALL_INTERFACE:include>    
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

install(TARGETS stb_image
        EXPORT stb_imageConfig
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
)

install(DIRECTORY include/ DESTINATION include)

install(EXPORT stb_imageConfig NAMESPACE stb:: DESTINATION ${CMAKE_INSTALL_PREFIX}/)

add_library(stb::stb_image ALIAS stb_image)