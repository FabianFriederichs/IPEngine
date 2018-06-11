cmake_minimum_required(VERSION 3.0)
project(API_Headers)

add_library(api_headers INTERFACE)

target_include_directories(api_headers
    INTERFACE 
        $<INSTALL_INTERFACE:include/api>    
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/api>
)

target_link_libraries(api_headers
        INTERFACE
                glm) # hack to introduce glm types, they are used virtually everywhere currently. find a better solution later!

install(TARGETS api_headers
        EXPORT api_headersConfig
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
)

install(DIRECTORY api/ DESTINATION include/api/)

install(EXPORT api_headersConfig NAMESPACE ipengine:: DESTINATION ${CMAKE_INSTALL_PREFIX}/)

add_library(ipengine::api_headers ALIAS api_headers)