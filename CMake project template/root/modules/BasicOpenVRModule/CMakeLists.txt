## this is a template for CMakeLists.txt files for building modules. Copy the content of this file into the desired
#  cmake script and search & replace BasicOpenVRModule with the desired module name

cmake_minimum_required(VERSION 3.0)
project(BasicOpenVRModule)

# collect headers and sources -------------------------------------------------------

set(BasicOpenVRModule_headers)
set(BasicOpenVRModule_sources)

# headers
list(APPEND BasicOpenVRModule_headers "include/BasicOpenVRModule.h")
# sources
list(APPEND BasicOpenVRModule_sources "src/BasicOpenVRModule.cpp")

# generate source groups
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${BasicOpenVRModule_headers} ${BasicOpenVRModule_sources})

# create the library target ---------------------------------------------------------
add_library(BasicOpenVRModule SHARED ${BasicOpenVRModule_headers} ${BasicOpenVRModule_sources})

# set include path ------------------------------------------------------------------
target_include_directories(BasicOpenVRModule
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
#target_compile_definitions(BasicOpenVRModule
#    PRIVATE
#        SOME_PREPROCESSOR_DEFINITION)

# set dependencies ------------------------------------------------------------------
target_link_libraries(BasicOpenVRModule
    PRIVATE
        ipengine::injector
        ipengine::core
        OpenVR::OpenVR)

# install ---------------------------------------------------------------------------

install(TARGETS BasicOpenVRModule
        EXPORT BasicOpenVRModuleConfig
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
)

# do this if there are public headers that must be installed (usually they should be located in the api_headers/api directory)
#install(DIRECTORY include/public/ DESTINATION include)

install(EXPORT BasicOpenVRModuleConfig NAMESPACE ipengine:: DESTINATION ${CMAKE_INSTALL_PREFIX}/)

add_library(ipengine::BasicOpenVRModule ALIAS BasicOpenVRModule)