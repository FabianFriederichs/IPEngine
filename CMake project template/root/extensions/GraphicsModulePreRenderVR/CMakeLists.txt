## this is a template for CMakeLists.txt files for building extensions. Copy the content of this file into the desired
#  cmake script and search & replace GraphicsModulePreRenderVR with the desired extension name

cmake_minimum_required(VERSION 3.0)
project(GraphicsModulePreRenderVR)

# collect headers and sources -------------------------------------------------------

set(GraphicsModulePreRenderVR_headers)
set(GraphicsModulePreRenderVR_sources)

# headers
list(APPEND GraphicsModulePreRenderVR_headers "include/glerror.h")
list(APPEND GraphicsModulePreRenderVR_headers "include/GraphicsModulePreRenderVR.h")
list(APPEND GraphicsModulePreRenderVR_headers "include/libheaders.h")
# sources
list(APPEND GraphicsModulePreRenderVR_sources "src/glerror.cpp")
list(APPEND GraphicsModulePreRenderVR_sources "src/GraphicsModulePreRenderVR.cpp")

# generate source groups
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${GraphicsModulePreRenderVR_headers} ${GraphicsModulePreRenderVR_sources})

# create the library target ---------------------------------------------------------
add_library(GraphicsModulePreRenderVR SHARED ${GraphicsModulePreRenderVR_headers} ${GraphicsModulePreRenderVR_sources})

# set include path ------------------------------------------------------------------
target_include_directories(GraphicsModulePreRenderVR
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
#target_compile_definitions(GraphicsModulePreRenderVR
#    PRIVATE
#        SOME_PREPROCESSOR_DEFINITION)

# set dependencies ------------------------------------------------------------------
target_link_libraries(GraphicsModulePreRenderVR
    PRIVATE
        ipengine::injector
        ipengine::core
        glm
        OpenGL::GL
        glew::glew
        OpenVR::OpenVR)

# install ---------------------------------------------------------------------------

install(TARGETS GraphicsModulePreRenderVR
        EXPORT GraphicsModulePreRenderVRConfig
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
)

# do this if there are public headers that must be installed (usually they should be located in the api_headers/api directory)
#install(DIRECTORY include/public/ DESTINATION include)

install(EXPORT GraphicsModulePreRenderVRConfig NAMESPACE ipengine:: DESTINATION ${CMAKE_INSTALL_PREFIX}/)

add_library(ipengine::GraphicsModulePreRenderVR ALIAS GraphicsModulePreRenderVR)