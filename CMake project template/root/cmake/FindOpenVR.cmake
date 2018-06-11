# tries to find openvr

set(OpenVR_ROOT ${OpenVR_ROOT} CACHE PATH "Root directory of OpenVR SDK installation.")

# weird per platform directories
# we need to find the right directory name out of [linux32, linux64, osx32, osx64, win32, win64]

# find "bitness" of the system
set(bitness)

if(CMAKE_SIZEOF_VOID_P MATCHES 8)
  set(bitness "64")
else(CMAKE_SIZEOF_VOID_P MATCHES 8)
  set(bitness "32")
endif(CMAKE_SIZEOF_VOID_P MATCHES 8)

#find platform
set(osprefix)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  SET(osprefix "win")
ELSEIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  SET(osprefix "osx")
ELSEIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  SET(osprefix "linux")
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")

# assemble platform directory name
set(plat_dir_name "${osprefix}${bitness}")

# find the library file
find_library(OpenVR_LIBRARY
  NAMES
  OpenVR
  openvr_api
  PATHS
  ${OpenVR_ROOT}
  PATH_SUFFIXES
  lib
  "lib/${plat_dir_name}")

# find the include path
find_path(OpenVR_INCLUDE_DIR
  NAMES
  "openvr.h"
  "openvr_driver.h"
  PATHS
  ${OpenVR_ROOT}
  ENV OpenVR_SDK
  PATH_SUFFIXES
  headers
  include)

# take care of find module default behaviour
include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(OpenVR
  DEFAULT_MSG
  OpenVR_LIBRARY
  OpenVR_INCLUDE_DIR)

# define an imported target

IF(OpenVR_FOUND)
  # create imported target
  add_library(OpenVR::OpenVR INTERFACE IMPORTED)
  # set "old-style" variables
  set_target_properties(OpenVR::OpenVR PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${OpenVR_INCLUDE_DIR}
    INTERFACE_LINK_LIBRARIES ${OpenVR_LIBRARY})

ENDIF()

set(OpenVR_LIBRARIES ${OpenVR_LIBRARY})
set(OpenVR_INCLUDE_DIRS ${OpenVR_INCLUDE_DIR})

mark_as_advanced(OpenVR_LIBRARY OpenVR_INCLUDE_DIR)