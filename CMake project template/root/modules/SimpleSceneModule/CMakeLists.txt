## this is a template for CMakeLists.txt files for building modules. Copy the content of this file into the desired
#  cmake script and search & replace SimpleSceneModule with the desired module name

cmake_minimum_required(VERSION 3.0)
project(SimpleSceneModule)

# collect headers and sources -------------------------------------------------------

set(SimpleSceneModule_headers)
set(SimpleSceneModule_sources)

# headers
list(APPEND SimpleSceneModule_headers "include/Scene.h")
list(APPEND SimpleSceneModule_headers "include/SimpleSceneModule.h")
# sources
list(APPEND SimpleSceneModule_sources "src/Scene.cpp")
list(APPEND SimpleSceneModule_sources "src/SimpleSceneModule.cpp")

# generate source groups
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SimpleSceneModule_headers} ${SimpleSceneModule_sources})

# create the library target ---------------------------------------------------------
add_library(SimpleSceneModule SHARED ${SimpleSceneModule_headers} ${SimpleSceneModule_sources})

# set include path ------------------------------------------------------------------
target_include_directories(SimpleSceneModule
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
#target_compile_definitions(SimpleSceneModule
#    PRIVATE
#        SOME_PREPROCESSOR_DEFINITION)

# set dependencies ------------------------------------------------------------------
target_link_libraries(SimpleSceneModule
    PRIVATE
        ipengine::injector
        ipengine::core)

# install ---------------------------------------------------------------------------

install(TARGETS SimpleSceneModule
        EXPORT SimpleSceneModuleConfig
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
)

# do this if there are public headers that must be installed (usually they should be located in the api_headers/api directory)
#install(DIRECTORY include/public/ DESTINATION include)

install(EXPORT SimpleSceneModuleConfig NAMESPACE ipengine:: DESTINATION ${CMAKE_INSTALL_PREFIX}/)

add_library(ipengine::SimpleSceneModule ALIAS SimpleSceneModule)