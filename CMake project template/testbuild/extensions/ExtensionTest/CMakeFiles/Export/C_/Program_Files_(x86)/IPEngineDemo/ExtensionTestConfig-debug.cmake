#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "ipengine::ExtensionTest" for configuration "Debug"
set_property(TARGET ipengine::ExtensionTest APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(ipengine::ExtensionTest PROPERTIES
  IMPORTED_IMPLIB_DEBUG "${_IMPORT_PREFIX}/lib/ExtensionTest.lib"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/bin/ExtensionTest.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS ipengine::ExtensionTest )
list(APPEND _IMPORT_CHECK_FILES_FOR_ipengine::ExtensionTest "${_IMPORT_PREFIX}/lib/ExtensionTest.lib" "${_IMPORT_PREFIX}/bin/ExtensionTest.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
