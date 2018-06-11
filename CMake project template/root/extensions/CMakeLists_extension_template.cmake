## this is a template for CMakeLists.txt files for building extensions. Copy the content of this file into the desired
#  cmake script and search & replace EXTENSIONNAME with the desired extension name

cmake_minimum_required(VERSION 3.0)
project(EXTENSIONNAME)

# collect headers and sources -------------------------------------------------------

set(EXTENSIONNAME_headers)
set(EXTENSIONNAME_sources)

# headers

# sources

# generate source groups
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${EXTENSIONNAME_headers} ${EXTENSIONNAME_sources})

# create the library target ---------------------------------------------------------
add_library(EXTENSIONNAME SHARED ${EXTENSIONNAME_headers} ${EXTENSIONNAME_sources})

# set include path ------------------------------------------------------------------
target_include_directories(EXTENSIONNAME
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
#target_compile_definitions(EXTENSIONNAME
#    PRIVATE
#        SOME_PREPROCESSOR_DEFINITION)

# set dependencies ------------------------------------------------------------------
target_link_libraries(EXTENSIONNAME
    PRIVATE
        ipengine::injector
        ipengine::core
        # add other dependencies)

# install ---------------------------------------------------------------------------

install(TARGETS EXTENSIONNAME
        EXPORT EXTENSIONNAMEConfig
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
)

# do this if there are public headers that must be installed (usually they should be located in the api_headers/api directory)
#install(DIRECTORY include/public/ DESTINATION include)

install(EXPORT EXTENSIONNAMEConfig NAMESPACE ipengine:: DESTINATION ${CMAKE_INSTALL_PREFIX}/)

add_library(ipengine::EXTENSIONNAME ALIAS EXTENSIONNAME)