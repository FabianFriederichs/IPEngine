#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "ipengine::injector" for configuration "Debug"
set_property(TARGET ipengine::injector APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(ipengine::injector PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/injector.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS ipengine::injector )
list(APPEND _IMPORT_CHECK_FILES_FOR_ipengine::injector "${_IMPORT_PREFIX}/lib/injector.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
