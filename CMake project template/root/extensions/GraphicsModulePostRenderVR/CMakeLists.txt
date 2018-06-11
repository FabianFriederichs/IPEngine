## this is a template for CMakeLists.txt files for building extensions. Copy the content of this file into the desired
#  cmake script and search & replace GraphicsModulePostRenderVR with the desired extension name

cmake_minimum_required(VERSION 3.0)
project(GraphicsModulePostRenderVR)

# collect headers and sources -------------------------------------------------------

set(GraphicsModulePostRenderVR_headers)
set(GraphicsModulePostRenderVR_sources)

# headers
list(APPEND GraphicsModulePostRenderVR_headers "include/GraphicsModulePostRenderVR.h")
# sources
list(APPEND GraphicsModulePostRenderVR_sources "src/GraphicsModulePostRenderVR.cpp")

# generate source groups
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${GraphicsModulePostRenderVR_headers} ${GraphicsModulePostRenderVR_sources})

# create the library target ---------------------------------------------------------
add_library(GraphicsModulePostRenderVR SHARED ${GraphicsModulePostRenderVR_headers} ${GraphicsModulePostRenderVR_sources})

# set include path ------------------------------------------------------------------
target_include_directories(GraphicsModulePostRenderVR
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
#target_compile_definitions(GraphicsModulePostRenderVR
#    PRIVATE
#        SOME_PREPROCESSOR_DEFINITION)

# set dependencies ------------------------------------------------------------------
target_link_libraries(GraphicsModulePostRenderVR
    PRIVATE
        ipengine::injector
        ipengine::core
        OpenVR::OpenVR)

# install ---------------------------------------------------------------------------

install(TARGETS GraphicsModulePostRenderVR
        EXPORT GraphicsModulePostRenderVRConfig
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
)

# do this if there are public headers that must be installed (usually they should be located in the api_headers/api directory)
#install(DIRECTORY include/public/ DESTINATION include)

install(EXPORT GraphicsModulePostRenderVRConfig NAMESPACE ipengine:: DESTINATION ${CMAKE_INSTALL_PREFIX}/)

add_library(ipengine::GraphicsModulePostRenderVR ALIAS GraphicsModulePostRenderVR)