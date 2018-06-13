#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "ipengine::injector" for configuration "Release"
set_property(TARGET ipengine::injector APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(ipengine::injector PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/injector.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS ipengine::injector )
list(APPEND _IMPORT_CHECK_FILES_FOR_ipengine::injector "${_IMPORT_PREFIX}/lib/injector.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
