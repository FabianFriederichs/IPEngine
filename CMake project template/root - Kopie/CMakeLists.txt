cmake_minimum_required(VERSION 3.0)
project(IPEngineDemo VERSION 1.0 LANGUAGES CXX)

## project-global configuration
# 32- or 64-bit?
if(CMAKE_SIZEOF_VOID_P MATCHES 8)
    set(IP_BUILD_X64 TRUE)
    set(IP_BUILD_X86 FALSE)
else(CMAKE_SIZEOF_VOID_P MATCHES 8)
    set(IP_BUILD_X64 FALSE)
    set(IP_BUILD_X86 TRUE)
endif(CMAKE_SIZEOF_VOID_P MATCHES 8)

# output directory
set(IP_BINARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/output/$<$<BOOL:IP_BUILD_X64>:x64>$<$<BOOL:IP_BUILD_X86>:x86>/$<CONFIG>/")

## external dependencies
include(external_dependencies/IPExternalDependencies.cmake)

add_subdirectory(test)


## install stuff?