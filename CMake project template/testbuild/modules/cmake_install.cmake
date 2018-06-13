# Install script for directory: C:/Users/Fabian/Documents/SourceTreeRepos/IPEngine/CMake project template/root/modules

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

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("C:/Users/Fabian/Documents/SourceTreeRepos/IPEngine/CMake project template/testbuild/modules/BasicOpenVRModule/cmake_install.cmake")
  include("C:/Users/Fabian/Documents/SourceTreeRepos/IPEngine/CMake project template/testbuild/modules/DataStoreModule/cmake_install.cmake")
  include("C:/Users/Fabian/Documents/SourceTreeRepos/IPEngine/CMake project template/testbuild/modules/GameLogicModule/cmake_install.cmake")
  include("C:/Users/Fabian/Documents/SourceTreeRepos/IPEngine/CMake project template/testbuild/modules/GraphicsModule/cmake_install.cmake")
  include("C:/Users/Fabian/Documents/SourceTreeRepos/IPEngine/CMake project template/testbuild/modules/InputModule/cmake_install.cmake")
  include("C:/Users/Fabian/Documents/SourceTreeRepos/IPEngine/CMake project template/testbuild/modules/PhysicsModule/cmake_install.cmake")
  include("C:/Users/Fabian/Documents/SourceTreeRepos/IPEngine/CMake project template/testbuild/modules/SimpleContentModule/cmake_install.cmake")
  include("C:/Users/Fabian/Documents/SourceTreeRepos/IPEngine/CMake project template/testbuild/modules/SimpleSceneModule/cmake_install.cmake")

endif()

