## this is a template for CMakeLists.txt files for building extensions. Copy the content of this file into the desired
#  cmake script and search & replace exSSMExtendedPBRPhysicsLoader with the desired extension name

cmake_minimum_required(VERSION 3.0)
project(exSSMExtendedPBRPhysicsLoader)

# collect headers and sources -------------------------------------------------------

set(exSSMExtendedPBRPhysicsLoader_headers)
set(exSSMExtendedPBRPhysicsLoader_sources)

# headers
list(APPEND exSSMExtendedPBRPhysicsLoader_headers "include/exSSMExtendedPBRPhysicsLoader.h")
# sources
list(APPEND exSSMExtendedPBRPhysicsLoader_sources "src/exSSMExtendedPBRPhysicsLoader.cpp")

# generate source groups
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${exSSMExtendedPBRPhysicsLoader_headers} ${exSSMExtendedPBRPhysicsLoader_sources})

# create the library target ---------------------------------------------------------
add_library(exSSMExtendedPBRPhysicsLoader SHARED ${exSSMExtendedPBRPhysicsLoader_headers} ${exSSMExtendedPBRPhysicsLoader_sources})

# set include path ------------------------------------------------------------------
target_include_directories(exSSMExtendedPBRPhysicsLoader
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
#target_compile_definitions(exSSMExtendedPBRPhysicsLoader
#    PRIVATE
#        SOME_PREPROCESSOR_DEFINITION)

# set dependencies ------------------------------------------------------------------
target_link_libraries(exSSMExtendedPBRPhysicsLoader
    PRIVATE
        ipengine::injector
        ipengine::core
        glm
        OpenGL::GL
        glew::glew
        OpenVR::OpenVR)

# install ---------------------------------------------------------------------------

install(TARGETS exSSMExtendedPBRPhysicsLoader
        EXPORT exSSMExtendedPBRPhysicsLoaderConfig
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
)

# do this if there are public headers that must be installed (usually they should be located in the api_headers/api directory)
#install(DIRECTORY include/public/ DESTINATION include)

install(EXPORT exSSMExtendedPBRPhysicsLoaderConfig NAMESPACE ipengine:: DESTINATION ${CMAKE_INSTALL_PREFIX}/)

add_library(ipengine::exSSMExtendedPBRPhysicsLoader ALIAS exSSMExtendedPBRPhysicsLoader)