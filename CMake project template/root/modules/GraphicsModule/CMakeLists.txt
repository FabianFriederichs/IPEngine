## this is a template for CMakeLists.txt files for building modules. Copy the content of this file into the desired
#  cmake script and search & replace GraphicsModule with the desired module name

cmake_minimum_required(VERSION 3.0)
project(GraphicsModule)

# collect headers and sources -------------------------------------------------------

set(GraphicsModule_headers)
set(GraphicsModule_sources)

# headers
list(APPEND GraphicsModule_headers "include/glerror.h")
list(APPEND GraphicsModule_headers "include/GraphicsModule.h")
list(APPEND GraphicsModule_headers "include/libheaders.h")
list(APPEND GraphicsModule_headers "include/Primitives.h")
list(APPEND GraphicsModule_headers "include/Utils")
# sources
list(APPEND GraphicsModule_sources "src/glerror.cpp")
list(APPEND GraphicsModule_sources "src/GraphicsModule.cpp")
list(APPEND GraphicsModule_sources "src/Primitives.cpp")
list(APPEND GraphicsModule_sources "src/Utils.cpp")

# generate source groups
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${GraphicsModule_headers} ${GraphicsModule_sources})

# create the library target ---------------------------------------------------------
add_library(GraphicsModule SHARED ${GraphicsModule_headers} ${GraphicsModule_sources})

# set include path ------------------------------------------------------------------
target_include_directories(GraphicsModule
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/include
# uncomment and use the following lines if there are usage requirements
#    PUBLIC
#        $<INSTALL_INTERFACE:...>    
#        $<BUILD_INTERFACE:...>
#    INTERFACE
#        $<INSTALL_INTERFACE:...>    
#        $<BUILD_INTERFACE:...>
)

# set compile definitions -----------------------------------------------------------
#target_compile_definitions(GraphicsModule
#    PRIVATE
#        SOME_PREPROCESSOR_DEFINITION)

# set dependencies ------------------------------------------------------------------
target_link_libraries(GraphicsModule
    PRIVATE
        ipengine::injector
        ipengine::core
        OpenGL::GL
        glew::glew
        glm
        SDL2::SDL2
        stb::stb_image)

# install ---------------------------------------------------------------------------

install(TARGETS GraphicsModule
        EXPORT GraphicsModuleConfig
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
)

# do this if there are public headers that must be installed (usually they should be located in the api_headers/api directory)
#install(DIRECTORY include/public/ DESTINATION include)

install(EXPORT GraphicsModuleConfig NAMESPACE ipengine:: DESTINATION ${CMAKE_INSTALL_PREFIX}/)

add_library(ipengine::GraphicsModule ALIAS GraphicsModule)