#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "ipengine::SimpleSceneModule" for configuration "Release"
set_property(TARGET ipengine::SimpleSceneModule APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(ipengine::SimpleSceneModule PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/SimpleSceneModule.lib"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/SimpleSceneModule.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS ipengine::SimpleSceneModule )
list(APPEND _IMPORT_CHECK_FILES_FOR_ipengine::SimpleSceneModule "${_IMPORT_PREFIX}/lib/SimpleSceneModule.lib" "${_IMPORT_PREFIX}/bin/SimpleSceneModule.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
