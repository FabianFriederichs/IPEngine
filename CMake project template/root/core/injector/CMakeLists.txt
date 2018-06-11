cmake_minimum_required(VERSION 3.0)
project(Injector)


set(injector_public_headers)
set(injector_private_headers)
set(injector_sources)

# injector ------------------------------------------------------------------------------------------------------
list(APPEND injector_public_headers "include/public/injector/DependencyGraph.h")
list(APPEND injector_public_headers "include/public/injector/DependencyParser.h")
list(APPEND injector_public_headers "include/public/injector/Injector.h")
list(APPEND injector_public_headers "include/public/injector/XMLParser.h")

list(APPEND injector_sources "src/Injector.cpp")
list(APPEND injector_sources "src/XMLParser.cpp")
# ---------------------------------------------------------------------------------------------------------------
# generate source groups
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${injector_public_headers} ${injector_private_headers} ${injector_sources})

# create the injector library
add_library(injector STATIC ${injector_public_headers} ${injector_private_headers} ${injector_sources})

# define include dirs
target_include_directories(injector
    PUBLIC 
        $<INSTALL_INTERFACE:include>    
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/public>
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

# injector dependencies
target_link_libraries(injector
    PUBLIC
        Boost::boost
        Boost::filesystem)

# install rules
install(TARGETS injector
        EXPORT injectorConfig
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
)

install(DIRECTORY include/public/ DESTINATION include)

install(EXPORT injectorConfig NAMESPACE ipengine:: DESTINATION ${CMAKE_INSTALL_PREFIX}/)

add_library(ipengine::injector ALIAS injector)