#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "ipengine::InputModule" for configuration "Release"
set_property(TARGET ipengine::InputModule APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(ipengine::InputModule PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/InputModule.lib"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/InputModule.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS ipengine::InputModule )
list(APPEND _IMPORT_CHECK_FILES_FOR_ipengine::InputModule "${_IMPORT_PREFIX}/lib/InputModule.lib" "${_IMPORT_PREFIX}/bin/InputModule.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
