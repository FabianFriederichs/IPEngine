## this is a template for CMakeLists.txt files for building extensions. Copy the content of this file into the desired
#  cmake script and search & replace ExtensionTest with the desired extension name

cmake_minimum_required(VERSION 3.0)
project(ExtensionTest)

# collect headers and sources -------------------------------------------------------

set(ExtensionTest_headers)
set(ExtensionTest_sources)

# headers
list(APPEND ExtensionTest_headers "include/ExtensionTest.h")
# sources
list(APPEND ExtensionTest_sources "src/ExtensionTest.cpp")

# generate source groups
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${ExtensionTest_headers} ${ExtensionTest_sources})

# create the library target ---------------------------------------------------------
add_library(ExtensionTest SHARED ${ExtensionTest_headers} ${ExtensionTest_sources})

# set include path ------------------------------------------------------------------
target_include_directories(ExtensionTest
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
#target_compile_definitions(ExtensionTest
#    PRIVATE
#        SOME_PREPROCESSOR_DEFINITION)

# set dependencies ------------------------------------------------------------------
target_link_libraries(ExtensionTest
    PRIVATE
        ipengine::injector
        ipengine::core
        OpenVR::OpenVR)

# install ---------------------------------------------------------------------------

install(TARGETS ExtensionTest
        EXPORT ExtensionTestConfig
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
)

# do this if there are public headers that must be installed (usually they should be located in the api_headers/api directory)
#install(DIRECTORY include/public/ DESTINATION include)

install(EXPORT ExtensionTestConfig NAMESPACE ipengine:: DESTINATION ${CMAKE_INSTALL_PREFIX}/)

add_library(ipengine::ExtensionTest ALIAS ExtensionTest)