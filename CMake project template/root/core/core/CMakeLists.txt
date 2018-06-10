cmake_minimum_required(VERSION 3.0)
project(IPCore)

# collect public headers, private headers and sources -------------------------------------------------------

set(core_public_headers)
set(core_private_headers)
set(core_sources)

# do core components one by one
# TODO: refactor to hide implementation detail, so that some of the public headers can be made private

# core ------------------------------------------------------------------------------------------------------
list(APPEND core_public_headers "include/public/core/core_config.h")
list(APPEND core_public_headers "include/public/core/core_types.h")
list(APPEND core_public_headers "include/public/core/core.h")
# no private headers
list(APPEND core_sources        "src/core.cpp")

# config ----------------------------------------------------------------------------------------------------
list(APPEND core_public_headers "include/public/core/config/ConfigManager.h")
# no private headers
list(APPEND core_sources        "src/config/ConfigManager.cpp")

# debug_monitoring ------------------------------------------------------------------------------------------
list(APPEND core_public_headers "include/public/core/debug_monitoring/DebugMonitor.h")
list(APPEND core_public_headers "include/public/core/debug_monitoring/Error.h")
list(APPEND core_public_headers "include/public/core/debug_monitoring/ErrorHandler.h")
list(APPEND core_public_headers "include/public/core/debug_monitoring/ErrorManager.h")
# no private headers
list(APPEND core_sources        "src/debug_monitoring/DebugMonitor.cpp")
list(APPEND core_sources        "src/debug_monitoring/Error.cpp")
list(APPEND core_sources        "src/debug_monitoring/ErrorHandler.cpp")
list(APPEND core_sources        "src/debug_monitoring/ErrorManager.cpp")

# memory ----------------------------------------------------------------------------------------------------
list(APPEND core_public_headers "include/public/core/memory/allocators.h")
list(APPEND core_public_headers "include/public/core/memory/lowlevel_allocators.h")
list(APPEND core_public_headers "include/public/core/memory/memory_utils.h")
list(APPEND core_public_headers "include/public/core/memory/MemoryManager.h")
# no private headers
list(APPEND core_sources        "src/memory/MemoryManager.cpp")

# messaging -------------------------------------------------------------------------------------------------
list(APPEND core_public_headers "include/public/core/messaging/EndpointRegistry.h")
list(APPEND core_public_headers "include/public/core/messaging/Message.h")
list(APPEND core_public_headers "include/public/core/messaging/MessageEndpoint.h")
list(APPEND core_public_headers "include/public/core/messaging/MessageQueue.h")
# no private headers
list(APPEND core_sources        "src/messaging/EndpointRegistry.cpp")
list(APPEND core_sources        "src/messaging/MessageEndpoint.cpp")
list(APPEND core_sources        "src/messaging/MessageQueue.cpp")

# runtime ---------------------------------------------------------------------------------------------------
list(APPEND core_public_headers "include/public/core/runtime/Console.h")
list(APPEND core_public_headers "include/public/core/runtime/Scheduler.h")
list(APPEND core_public_headers "include/public/core/runtime/Time.h")
# no private headers
list(APPEND core_sources        "src/runtime/Console.cpp")
list(APPEND core_sources        "src/runtime/Scheduler.cpp")

# threading -------------------------------------------------------------------------------------------------
list(APPEND core_public_headers "include/public/core/threading/Common.h")
list(APPEND core_public_headers "include/public/core/threading/Task.h")
list(APPEND core_public_headers "include/public/core/threading/TaskContext.h")
list(APPEND core_public_headers "include/public/core/threading/TaskHandle.h")
list(APPEND core_public_headers "include/public/core/threading/TaskLib.h")
list(APPEND core_public_headers "include/public/core/threading/ThreadPool.h")
# no private headers
list(APPEND core_sources        "src/threading/TaskHandle.cpp")
list(APPEND core_sources        "src/threading/ThreadPool.cpp")

# util ------------------------------------------------------------------------------------------------------
list(APPEND core_public_headers "include/public/core/util/any.h")
list(APPEND core_public_headers "include/public/core/util/Deque.h")
list(APPEND core_public_headers "include/public/core/util/function.h")
list(APPEND core_public_headers "include/public/core/util/handle.h")
list(APPEND core_public_headers "include/public/core/util/hazard_pointer.h")
list(APPEND core_public_headers "include/public/core/util/idgen.h")
list(APPEND core_public_headers "include/public/core/util/LFDS.h")
list(APPEND core_public_headers "include/public/core/util/refctr.h")
list(APPEND core_public_headers "include/public/core/util/Scanner.h")
list(APPEND core_public_headers "include/public/core/util/spinlock.h")
list(APPEND core_public_headers "include/public/core/util/stack.h")
# no private headers
# no sources

# -----------------------------------------------------------------------------------------------------------

# generate source groups
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${core_public_headers} ${core_private_headers} ${core_sources})

# create the core library
add_library(core SHARED ${core_public_headers} ${core_private_headers} ${core_sources})

# define include dirs
target_include_directories(core
    PUBLIC 
        $<INSTALL_INTERFACE:include>    
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/public>
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

# core dependencies
target_link_libraries(core
    PUBLIC
        moodycamel_queue)

# install rules
install(TARGETS core
        EXPORT coreConfig
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
)

install(DIRECTORY include/public/ DESTINATION include)

install(EXPORT coreConfig NAMESPACE ipengine DESTINATION ${CMAKE_INSTALL_PREFIX}/)