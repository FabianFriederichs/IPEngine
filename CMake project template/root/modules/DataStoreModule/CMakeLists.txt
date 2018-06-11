## this is a template for CMakeLists.txt files for building modules. Copy the content of this file into the desired
#  cmake script and search & replace DataStoreModule with the desired module name

cmake_minimum_required(VERSION 3.0)
project(DataStoreModule)

# collect headers and sources -------------------------------------------------------

set(DataStoreModule_headers)
set(DataStoreModule_sources)

# headers
list(APPEND DataStoreModule_headers "include/DataStoreModule.h")
# sources
list(APPEND DataStoreModule_sources "src/DataStoreModule.cpp")

# generate source groups
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${DataStoreModule_headers} ${DataStoreModule_sources})

# create the library target ---------------------------------------------------------
add_library(DataStoreModule SHARED ${DataStoreModule_headers} ${DataStoreModule_sources})

# set include path ------------------------------------------------------------------
target_include_directories(DataStoreModule
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
#target_compile_definitions(DataStoreModule
#    PRIVATE
#        SOME_PREPROCESSOR_DEFINITION)

# set dependencies ------------------------------------------------------------------
target_link_libraries(DataStoreModule
    PRIVATE
        ipengine::injector
        ipengine::core
        OpenVR::OpenVR)

# install ---------------------------------------------------------------------------

install(TARGETS DataStoreModule
        EXPORT DataStoreModuleConfig
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
)

# do this if there are public headers that must be installed (usually they should be located in the api_headers/api directory)
#install(DIRECTORY include/public/ DESTINATION include)

install(EXPORT DataStoreModuleConfig NAMESPACE ipengine:: DESTINATION ${CMAKE_INSTALL_PREFIX}/)

add_library(ipengine::DataStoreModule ALIAS DataStoreModule)