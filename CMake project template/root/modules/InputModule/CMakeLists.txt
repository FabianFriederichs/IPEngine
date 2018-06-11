## this is a template for CMakeLists.txt files for building modules. Copy the content of this file into the desired
#  cmake script and search & replace InputModule with the desired module name

cmake_minimum_required(VERSION 3.0)
project(InputModule)

# collect headers and sources -------------------------------------------------------

set(InputModule_headers)
set(InputModule_sources)

# headers
list(APPEND InputModule_headers "include/InputModule.h")
# sources
list(APPEND InputModule_sources "src/InputModule.cpp")

# generate source groups
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${InputModule_headers} ${InputModule_sources})

# create the library target ---------------------------------------------------------
add_library(InputModule SHARED ${InputModule_headers} ${InputModule_sources})

# set include path ------------------------------------------------------------------
target_include_directories(InputModule
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
#target_compile_definitions(InputModule
#    PRIVATE
#        SOME_PREPROCESSOR_DEFINITION)

# set dependencies ------------------------------------------------------------------
target_link_libraries(InputModule
    PRIVATE
        ipengine::injector
        ipengine::core
        OpenVR::OpenVR
        SDL2::SDL2
        glm)

# install ---------------------------------------------------------------------------

install(TARGETS InputModule
        EXPORT InputModuleConfig
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
)

# do this if there are public headers that must be installed (usually they should be located in the api_headers/api directory)
#install(DIRECTORY include/public/ DESTINATION include)

install(EXPORT InputModuleConfig NAMESPACE ipengine:: DESTINATION ${CMAKE_INSTALL_PREFIX}/)

add_library(ipengine::InputModule ALIAS InputModule)