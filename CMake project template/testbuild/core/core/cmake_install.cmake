# Install script for directory: C:/Users/Fabian/Documents/SourceTreeRepos/IPEngine/CMake project template/root/core/core

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/IPEngineDemo")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Users/Fabian/Documents/SourceTreeRepos/IPEngine/CMake project template/root/output/x86/Debug/core.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Users/Fabian/Documents/SourceTreeRepos/IPEngine/CMake project template/root/output/x86/Release/core.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Users/Fabian/Documents/SourceTreeRepos/IPEngine/CMake project template/root/output/x86/MinSizeRel/core.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Users/Fabian/Documents/SourceTreeRepos/IPEngine/CMake project template/root/output/x86/RelWithDebInfo/core.lib")
  endif()
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "C:/Users/Fabian/Documents/SourceTreeRepos/IPEngine/CMake project template/root/output/x86/Debug/core.dll")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "C:/Users/Fabian/Documents/SourceTreeRepos/IPEngine/CMake project template/root/output/x86/Release/core.dll")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "C:/Users/Fabian/Documents/SourceTreeRepos/IPEngine/CMake project template/root/output/x86/MinSizeRel/core.dll")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "C:/Users/Fabian/Documents/SourceTreeRepos/IPEngine/CMake project template/root/output/x86/RelWithDebInfo/core.dll")
  endif()
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE DIRECTORY FILES "C:/Users/Fabian/Documents/SourceTreeRepos/IPEngine/CMake project template/root/core/core/include/public/")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}C:/Program Files (x86)/IPEngineDemo/coreConfig.cmake")
    file(DIFFERENT EXPORT_FILE_CHANGED FILES
         "$ENV{DESTDIR}C:/Program Files (x86)/IPEngineDemo/coreConfig.cmake"
         "C:/Users/Fabian/Documents/SourceTreeRepos/IPEngine/CMake project template/testbuild/core/core/CMakeFiles/Export/C_/Program_Files_(x86)/IPEngineDemo/coreConfig.cmake")
    if(EXPORT_FILE_CHANGED)
      file(GLOB OLD_CONFIG_FILES "$ENV{DESTDIR}C:/Program Files (x86)/IPEngineDemo/coreConfig-*.cmake")
      if(OLD_CONFIG_FILES)
        message(STATUS "Old export file \"$ENV{DESTDIR}C:/Program Files (x86)/IPEngineDemo/coreConfig.cmake\" will be replaced.  Removing files [${OLD_CONFIG_FILES}].")
        file(REMOVE ${OLD_CONFIG_FILES})
      endif()
    endif()
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "C:/Program Files (x86)/IPEngineDemo/coreConfig.cmake")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "C:/Program Files (x86)/IPEngineDemo" TYPE FILE FILES "C:/Users/Fabian/Documents/SourceTreeRepos/IPEngine/CMake project template/testbuild/core/core/CMakeFiles/Export/C_/Program_Files_(x86)/IPEngineDemo/coreConfig.cmake")
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
     "C:/Program Files (x86)/IPEngineDemo/coreConfig-debug.cmake")
    if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
        message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
    endif()
    if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
        message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
    endif()
file(INSTALL DESTINATION "C:/Program Files (x86)/IPEngineDemo" TYPE FILE FILES "C:/Users/Fabian/Documents/SourceTreeRepos/IPEngine/CMake project template/testbuild/core/core/CMakeFiles/Export/C_/Program_Files_(x86)/IPEngineDemo/coreConfig-debug.cmake")
  endif()
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
     "C:/Program Files (x86)/IPEngineDemo/coreConfig-minsizerel.cmake")
    if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
        message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
    endif()
    if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
        message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
    endif()
file(INSTALL DESTINATION "C:/Program Files (x86)/IPEngineDemo" TYPE FILE FILES "C:/Users/Fabian/Documents/SourceTreeRepos/IPEngine/CMake project template/testbuild/core/core/CMakeFiles/Export/C_/Program_Files_(x86)/IPEngineDemo/coreConfig-minsizerel.cmake")
  endif()
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
     "C:/Program Files (x86)/IPEngineDemo/coreConfig-relwithdebinfo.cmake")
    if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
        message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
    endif()
    if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
        message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
    endif()
file(INSTALL DESTINATION "C:/Program Files (x86)/IPEngineDemo" TYPE FILE FILES "C:/Users/Fabian/Documents/SourceTreeRepos/IPEngine/CMake project template/testbuild/core/core/CMakeFiles/Export/C_/Program_Files_(x86)/IPEngineDemo/coreConfig-relwithdebinfo.cmake")
  endif()
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
     "C:/Program Files (x86)/IPEngineDemo/coreConfig-release.cmake")
    if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
        message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
    endif()
    if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
        message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
    endif()
file(INSTALL DESTINATION "C:/Program Files (x86)/IPEngineDemo" TYPE FILE FILES "C:/Users/Fabian/Documents/SourceTreeRepos/IPEngine/CMake project template/testbuild/core/core/CMakeFiles/Export/C_/Program_Files_(x86)/IPEngineDemo/coreConfig-release.cmake")
  endif()
endif()

