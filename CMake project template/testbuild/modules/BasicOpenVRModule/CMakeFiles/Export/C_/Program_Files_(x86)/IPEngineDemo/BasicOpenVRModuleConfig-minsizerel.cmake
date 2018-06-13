#----------------------------------------------------------------
# Generated CMake target import file for configuration "MinSizeRel".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "ipengine::BasicOpenVRModule" for configuration "MinSizeRel"
set_property(TARGET ipengine::BasicOpenVRModule APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)
set_target_properties(ipengine::BasicOpenVRModule PROPERTIES
  IMPORTED_IMPLIB_MINSIZEREL "${_IMPORT_PREFIX}/lib/BasicOpenVRModule.lib"
  IMPORTED_LOCATION_MINSIZEREL "${_IMPORT_PREFIX}/bin/BasicOpenVRModule.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS ipengine::BasicOpenVRModule )
list(APPEND _IMPORT_CHECK_FILES_FOR_ipengine::BasicOpenVRModule "${_IMPORT_PREFIX}/lib/BasicOpenVRModule.lib" "${_IMPORT_PREFIX}/bin/BasicOpenVRModule.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
