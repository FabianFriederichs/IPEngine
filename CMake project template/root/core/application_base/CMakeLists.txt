cmake_minimum_required(VERSION 3.0)
project(ApplicationBase)


set(appbase_public_headers)
set(appbase_private_headers)
set(appbase_sources)

# injector ------------------------------------------------------------------------------------------------------
list(APPEND appbase_public_headers "include/public/application_base/Applicationh.h")

list(APPEND appbase_sources "src/Application.cpp")

# ---------------------------------------------------------------------------------------------------------------
# generate source groups
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${appbase_public_headers} ${appbase_private_headers} ${appbase_sources})

# create the injector library
add_library(application_base SHARED ${appbase_public_headers} ${appbase_private_headers} ${appbase_sources})

# define include dirs
target_include_directories(application_base
    PUBLIC 
        $<INSTALL_INTERFACE:include>    
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/public>
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

# injector dependencies
target_link_libraries(application_base
    PUBLIC
        ipengine::injector
        ipengine::core)

# for declspec(dllexport) stuff on windows
target_compile_definitions(application_base
PRIVATE
    EXPORT_APPLICATION)

# install rules
install(TARGETS application_base
        EXPORT application_baseConfig
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
)

install(DIRECTORY include/public/ DESTINATION include)

install(EXPORT application_baseConfig NAMESPACE ipengine:: DESTINATION ${CMAKE_INSTALL_PREFIX}/)

add_library(ipengine::application_base ALIAS application_base)