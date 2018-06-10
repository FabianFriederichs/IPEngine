cmake_minimum_required(VERSION 3.0)

add_executable(demo src/Demo.cpp)

target_link_libraries(demo ipengine::application_base ipengine::api_headers ipengine::core ipengine::injector)

install(TARGETS demo
        EXPORT demoConfig
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
)

install(EXPORT demoConfig NAMESPACE ipengine:: DESTINATION ${CMAKE_INSTALL_PREFIX}/)