## this is a template for CMakeLists.txt files for building modules. Copy the content of this file into the desired
#  cmake script and search & replace GameLogicModule with the desired module name

cmake_minimum_required(VERSION 3.0)
project(GameLogicModule)

# collect headers and sources -------------------------------------------------------

set(GameLogicModule_headers)
set(GameLogicModule_sources)

# headers
list(APPEND GameLogicModule_headers "include/GameLogicModule.h")
# sources
list(APPEND GameLogicModule_sources "src/GameLogicModule.cpp")

# generate source groups
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${GameLogicModule_headers} ${GameLogicModule_sources})

# create the library target ---------------------------------------------------------
add_library(GameLogicModule SHARED ${GameLogicModule_headers} ${GameLogicModule_sources})

# set include path ------------------------------------------------------------------
target_include_directories(GameLogicModule
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
#target_compile_definitions(GameLogicModule
#    PRIVATE
#        SOME_PREPROCESSOR_DEFINITION)

# set dependencies ------------------------------------------------------------------
target_link_libraries(GameLogicModule
    PRIVATE
        ipengine::injector
        ipengine::core
        OpenVR::OpenVR)

# install ---------------------------------------------------------------------------

install(TARGETS GameLogicModule
        EXPORT GameLogicModuleConfig
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
)

# do this if there are public headers that must be installed (usually they should be located in the api_headers/api directory)
#install(DIRECTORY include/public/ DESTINATION include)

install(EXPORT GameLogicModuleConfig NAMESPACE ipengine:: DESTINATION ${CMAKE_INSTALL_PREFIX}/)

add_library(ipengine::GameLogicModule ALIAS GameLogicModule)