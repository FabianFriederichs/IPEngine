## this is a template for CMakeLists.txt files for building modules. Copy the content of this file into the desired
#  cmake script and search & replace PhysicsModule with the desired module name

cmake_minimum_required(VERSION 3.0)
project(PhysicsModule)

# collect headers and sources -------------------------------------------------------

set(PhysicsModule_headers)
set(PhysicsModule_sources)

# headers
list(APPEND PhysicsModule_headers "include/PhysicsModule.h")
# sources
list(APPEND PhysicsModule_sources "src/PhysicsModule.cpp")

# generate source groups
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${PhysicsModule_headers} ${PhysicsModule_sources})

# create the library target ---------------------------------------------------------
add_library(PhysicsModule SHARED ${PhysicsModule_headers} ${PhysicsModule_sources})

# set include path ------------------------------------------------------------------
target_include_directories(PhysicsModule
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
#target_compile_definitions(PhysicsModule
#    PRIVATE
#        SOME_PREPROCESSOR_DEFINITION)

# set dependencies ------------------------------------------------------------------
target_link_libraries(PhysicsModule
    PRIVATE
        ipengine::injector
        ipengine::core
        glm)

# install ---------------------------------------------------------------------------

install(TARGETS PhysicsModule
        EXPORT PhysicsModuleConfig
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
)

# do this if there are public headers that must be installed (usually they should be located in the api_headers/api directory)
#install(DIRECTORY include/public/ DESTINATION include)

install(EXPORT PhysicsModuleConfig NAMESPACE ipengine:: DESTINATION ${CMAKE_INSTALL_PREFIX}/)

add_library(ipengine::PhysicsModule ALIAS PhysicsModule)