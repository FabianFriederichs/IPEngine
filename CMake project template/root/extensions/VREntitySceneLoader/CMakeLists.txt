## this is a template for CMakeLists.txt files for building extensions. Copy the content of this file into the desired
#  cmake script and search & replace VREntitySceneLoader with the desired extension name

cmake_minimum_required(VERSION 3.0)
project(VREntitySceneLoader)

# collect headers and sources -------------------------------------------------------

set(VREntitySceneLoader_headers)
set(VREntitySceneLoader_sources)

# headers
list(APPEND VREntitySceneLoader_headers "include/glerror.h")
list(APPEND VREntitySceneLoader_headers "include/libheaders.h")
list(APPEND VREntitySceneLoader_headers "include/VREntitySceneLoader.h")
# sources
list(APPEND VREntitySceneLoader_sources "src/glerror.cpp")
list(APPEND VREntitySceneLoader_sources "src/VREntitySceneLoader.cpp")

# generate source groups
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${VREntitySceneLoader_headers} ${VREntitySceneLoader_sources})

# create the library target ---------------------------------------------------------
add_library(VREntitySceneLoader SHARED ${VREntitySceneLoader_headers} ${VREntitySceneLoader_sources})

# set include path ------------------------------------------------------------------
target_include_directories(VREntitySceneLoader
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
#target_compile_definitions(VREntitySceneLoader
#    PRIVATE
#        SOME_PREPROCESSOR_DEFINITION)

# set dependencies ------------------------------------------------------------------
target_link_libraries(VREntitySceneLoader
    PRIVATE
        ipengine::injector
        ipengine::core
        glm
        OpenGL::GL
        glew::glew
        OpenVR::OpenVR)

# install ---------------------------------------------------------------------------

install(TARGETS VREntitySceneLoader
        EXPORT VREntitySceneLoaderConfig
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
)

# do this if there are public headers that must be installed (usually they should be located in the api_headers/api directory)
#install(DIRECTORY include/public/ DESTINATION include)

install(EXPORT VREntitySceneLoaderConfig NAMESPACE ipengine:: DESTINATION ${CMAKE_INSTALL_PREFIX}/)

add_library(ipengine::VREntitySceneLoader ALIAS VREntitySceneLoader)