cmake_minimum_required(VERSION 3.0)
project(MoodyCamelConcurrentQueue VERSION 1.0 LANGUAGES CXX)

add_library(moodycamel_queue INTERFACE)

target_include_directories(moodycamel_queue
    INTERFACE 
        $<INSTALL_INTERFACE:include>    
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>
)

install(TARGETS moodycamel_queue
        EXPORT moodycamel_queueConfig
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
)

install(DIRECTORY DESTINATION include)
install(FILE "concurrentqueue.h" DESTINATION ${CMAKE_INSTALL_PREFIX}/include/)

install(EXPORT moodycamel_queueConfig DESTINATION ${CMAKE_INSTALL_PREFIX}/)