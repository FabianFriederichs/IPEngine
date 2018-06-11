cmake_minimum_required(VERSION 3.0)
project(IPEngineDemo VERSION 1.0 LANGUAGES CXX)

## project-global configuration
# 32- or 64-bit?
if(CMAKE_SIZEOF_VOID_P MATCHES 8)
    set(IP_BUILD_X64 TRUE)
else()
    set(IP_BUILD_X86 TRUE)
endif()

# output directory
set(IP_BINARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/output/$<$<BOOL:${IP_BUILD_X64}>:x64>$<$<BOOL:${IP_BUILD_X86}>:x86>/$<CONFIG>/")

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${IP_BINARY_OUTPUT_DIRECTORY}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${IP_BINARY_OUTPUT_DIRECTORY}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${IP_BINARY_OUTPUT_DIRECTORY}")

# some cmake helpers
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

# visual studio project folder stuff
if(MSVC)
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endif()

# some custom cmake tools
include(IPCMakeUtils)

## external dependencies
include(IPExternalDependencies)

## core
add_subdirectory(core)

## api headers
add_subdirectory(api_headers)

## modules
add_subdirectory(modules)

## extensions
add_subdirectory(extensions)

## application
add_subdirectory(application)
setTargetFolder(demo IPEngineDemoApplication)


## do something smart to copy neccessary dlls, sos and assets into the output directory

## install stuff?