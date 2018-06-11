## this is a template for CMakeLists.txt files for building extensions. Copy the content of this file into the desired
#  cmake script and search & replace exSSMExtendedWriterPhysics with the desired extension name

cmake_minimum_required(VERSION 3.0)
project(exSSMExtendedWriterPhysics)

# collect headers and sources -------------------------------------------------------

set(exSSMExtendedWriterPhysics_headers)
set(exSSMExtendedWriterPhysics_sources)

# headers
list(APPEND exSSMExtendedWriterPhysics_headers "include/exSSMExtendedWriterPhysics.h")
# sources
list(APPEND exSSMExtendedWriterPhysics_sources "src/exSSMExtendedWriterPhysics.cpp")

# generate source groups
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${exSSMExtendedWriterPhysics_headers} ${exSSMExtendedWriterPhysics_sources})

# create the library target ---------------------------------------------------------
add_library(exSSMExtendedWriterPhysics SHARED ${exSSMExtendedWriterPhysics_headers} ${exSSMExtendedWriterPhysics_sources})

# set include path ------------------------------------------------------------------
target_include_directories(exSSMExtendedWriterPhysics
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
#target_compile_definitions(exSSMExtendedWriterPhysics
#    PRIVATE
#        SOME_PREPROCESSOR_DEFINITION)

# set dependencies ------------------------------------------------------------------
target_link_libraries(exSSMExtendedWriterPhysics
    PRIVATE
        ipengine::injector
        ipengine::core
        glm
        OpenGL::GL
        glew::glew
        OpenVR::OpenVR)

# install ---------------------------------------------------------------------------

install(TARGETS exSSMExtendedWriterPhysics
        EXPORT exSSMExtendedWriterPhysicsConfig
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
)

# do this if there are public headers that must be installed (usually they should be located in the api_headers/api directory)
#install(DIRECTORY include/public/ DESTINATION include)

install(EXPORT exSSMExtendedWriterPhysicsConfig NAMESPACE ipengine:: DESTINATION ${CMAKE_INSTALL_PREFIX}/)

add_library(ipengine::exSSMExtendedWriterPhysics ALIAS exSSMExtendedWriterPhysics)