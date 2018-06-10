cmake_minimum_required(VERSION 3.0)
project(API_Headers)

# api header files
set(API_HEADER_FILES)

# add all the api headers here:
list(APPEND API_HEADER_FILES "IBasicGraphicsContentModule_API.h")
list(APPEND API_HEADER_FILES "IBasicOpenVRModule_API.h")
list(APPEND API_HEADER_FILES "IDataStoreModuleh_API.h")
list(APPEND API_HEADER_FILES "IGraphics_API.h")
list(APPEND API_HEADER_FILES "IInput_API.h")
list(APPEND API_HEADER_FILES "ILogicModule_API.h")
list(APPEND API_HEADER_FILES "IModule_API.h")
list(APPEND API_HEADER_FILES "IPhysicsModule_API.h")
list(APPEND API_HEADER_FILES "ISimpleContentModule_API.h")
list(APPEND API_HEADER_FILES "ISimpleSceneModule_API.h")
list(APPEND API_HEADER_FILES "Transform.h")

add_library(api_headers INTERFACE)

target_include_directories(api_headers
    INTERFACE 
        $<INSTALL_INTERFACE:include/api>    
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>
)

install(TARGETS api_headers
        EXPORT api_headersConfig
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
)

install(DIRECTORY DESTINATION include/api/)
install(FILES ${API_HEADER_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/api/)

install(EXPORT api_headersConfig NAMESPACE ipengine:: DESTINATION ${CMAKE_INSTALL_PREFIX}/)

add_library(ipengine::api_headers ALIAS api_headers)