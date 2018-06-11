## this is a template for CMakeLists.txt files for building modules. Copy the content of this file into the desired
#  cmake script and search & replace SimpleContentModule with the desired module name

cmake_minimum_required(VERSION 3.0)
project(SimpleContentModule)

# collect headers and sources -------------------------------------------------------

set(SimpleContentModule_headers)
set(SimpleContentModule_sources)

# headers
list(APPEND SimpleContentModule_headers "include/OBJLoader.h")
list(APPEND SimpleContentModule_headers "include/SimpleContentModule.h")
# sources
list(APPEND SimpleContentModule_sources "src/OBJLoader.cpp")
list(APPEND SimpleContentModule_sources "src/SimpleContentModule.cpp")

# generate source groups
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SimpleContentModule_headers} ${SimpleContentModule_sources})

# create the library target ---------------------------------------------------------
add_library(SimpleContentModule SHARED ${SimpleContentModule_headers} ${SimpleContentModule_sources})

# set include path ------------------------------------------------------------------
target_include_directories(SimpleContentModule
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
#target_compile_definitions(SimpleContentModule
#    PRIVATE
#        SOME_PREPROCESSOR_DEFINITION)

# set dependencies ------------------------------------------------------------------
target_link_libraries(SimpleContentModule
    PRIVATE
        ipengine::injector
        ipengine::core
        glm
        glew::glew)

# install ---------------------------------------------------------------------------

install(TARGETS SimpleContentModule
        EXPORT SimpleContentModuleConfig
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
)

# do this if there are public headers that must be installed (usually they should be located in the api_headers/api directory)
#install(DIRECTORY include/public/ DESTINATION include)

install(EXPORT SimpleContentModuleConfig NAMESPACE ipengine:: DESTINATION ${CMAKE_INSTALL_PREFIX}/)

add_library(ipengine::SimpleContentModule ALIAS SimpleContentModule)