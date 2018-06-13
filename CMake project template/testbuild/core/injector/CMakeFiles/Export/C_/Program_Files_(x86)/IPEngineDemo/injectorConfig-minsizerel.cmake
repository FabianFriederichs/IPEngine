#----------------------------------------------------------------
# Generated CMake target import file for configuration "MinSizeRel".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "ipengine::injector" for configuration "MinSizeRel"
set_property(TARGET ipengine::injector APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)
set_target_properties(ipengine::injector PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_MINSIZEREL "CXX"
  IMPORTED_LOCATION_MINSIZEREL "${_IMPORT_PREFIX}/lib/injector.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS ipengine::injector )
list(APPEND _IMPORT_CHECK_FILES_FOR_ipengine::injector "${_IMPORT_PREFIX}/lib/injector.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
