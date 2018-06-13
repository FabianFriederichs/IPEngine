# Install script for directory: C:/Users/Fabian/Documents/SourceTreeRepos/IPEngine/CMake project template/root

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
  if(EXISTS "$ENV{DESTDIR}C:/Program Files (x86)/IPEngineDemo/glmConfig.cmake")
    file(DIFFERENT EXPORT_FILE_CHANGED FILES
         "$ENV{DESTDIR}C:/Program Files (x86)/IPEngineDemo/glmConfig.cmake"
         "C:/Users/Fabian/Documents/SourceTreeRepos/IPEngine/CMake project template/testbuild/CMakeFiles/Export/C_/Program_Files_(x86)/IPEngineDemo/glmConfig.cmake")
    if(EXPORT_FILE_CHANGED)
      file(GLOB OLD_CONFIG_FILES "$ENV{DESTDIR}C:/Program Files (x86)/IPEngineDemo/glmConfig-*.cmake")
      if(OLD_CONFIG_FILES)
        message(STATUS "Old export file \"$ENV{DESTDIR}C:/Program Files (x86)/IPEngineDemo/glmConfig.cmake\" will be replaced.  Removing files [${OLD_CONFIG_FILES}].")
        file(REMOVE ${OLD_CONFIG_FILES})
      endif()
    endif()
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "C:/Program Files (x86)/IPEngineDemo/glmConfig.cmake")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "C:/Program Files (x86)/IPEngineDemo" TYPE FILE FILES "C:/Users/Fabian/Documents/SourceTreeRepos/IPEngine/CMake project template/testbuild/CMakeFiles/Export/C_/Program_Files_(x86)/IPEngineDemo/glmConfig.cmake")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/glm" TYPE DIRECTORY FILES "C:/Users/Fabian/Documents/SourceTreeRepos/IPEngine/CMake project template/root/external_dependencies/glm/glm/")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("C:/Users/Fabian/Documents/SourceTreeRepos/IPEngine/CMake project template/testbuild/external_dependencies/moodycamel/cmake_install.cmake")
  include("C:/Users/Fabian/Documents/SourceTreeRepos/IPEngine/CMake project template/testbuild/external_dependencies/stb_image/cmake_install.cmake")
  include("C:/Users/Fabian/Documents/SourceTreeRepos/IPEngine/CMake project template/testbuild/external_dependencies/glew/cmake_install.cmake")
  include("C:/Users/Fabian/Documents/SourceTreeRepos/IPEngine/CMake project template/testbuild/core/cmake_install.cmake")
  include("C:/Users/Fabian/Documents/SourceTreeRepos/IPEngine/CMake project template/testbuild/api_headers/cmake_install.cmake")
  include("C:/Users/Fabian/Documents/SourceTreeRepos/IPEngine/CMake project template/testbuild/modules/cmake_install.cmake")
  include("C:/Users/Fabian/Documents/SourceTreeRepos/IPEngine/CMake project template/testbuild/extensions/cmake_install.cmake")
  include("C:/Users/Fabian/Documents/SourceTreeRepos/IPEngine/CMake project template/testbuild/application/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "C:/Users/Fabian/Documents/SourceTreeRepos/IPEngine/CMake project template/testbuild/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
