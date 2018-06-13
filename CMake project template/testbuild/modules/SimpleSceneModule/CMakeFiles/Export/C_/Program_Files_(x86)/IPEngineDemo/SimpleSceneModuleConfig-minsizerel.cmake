#----------------------------------------------------------------
# Generated CMake target import file for configuration "MinSizeRel".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "ipengine::SimpleSceneModule" for configuration "MinSizeRel"
set_property(TARGET ipengine::SimpleSceneModule APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)
set_target_properties(ipengine::SimpleSceneModule PROPERTIES
  IMPORTED_IMPLIB_MINSIZEREL "${_IMPORT_PREFIX}/lib/SimpleSceneModule.lib"
  IMPORTED_LOCATION_MINSIZEREL "${_IMPORT_PREFIX}/bin/SimpleSceneModule.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS ipengine::SimpleSceneModule )
list(APPEND _IMPORT_CHECK_FILES_FOR_ipengine::SimpleSceneModule "${_IMPORT_PREFIX}/lib/SimpleSceneModule.lib" "${_IMPORT_PREFIX}/bin/SimpleSceneModule.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
