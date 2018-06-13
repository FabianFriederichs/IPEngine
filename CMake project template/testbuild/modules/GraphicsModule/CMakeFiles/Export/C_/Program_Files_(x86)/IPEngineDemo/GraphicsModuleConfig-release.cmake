#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "ipengine::GraphicsModule" for configuration "Release"
set_property(TARGET ipengine::GraphicsModule APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(ipengine::GraphicsModule PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/GraphicsModule.lib"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/GraphicsModule.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS ipengine::GraphicsModule )
list(APPEND _IMPORT_CHECK_FILES_FOR_ipengine::GraphicsModule "${_IMPORT_PREFIX}/lib/GraphicsModule.lib" "${_IMPORT_PREFIX}/bin/GraphicsModule.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
