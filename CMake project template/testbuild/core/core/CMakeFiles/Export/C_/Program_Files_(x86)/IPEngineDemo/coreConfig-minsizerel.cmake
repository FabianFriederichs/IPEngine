#----------------------------------------------------------------
# Generated CMake target import file for configuration "MinSizeRel".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "ipengine::core" for configuration "MinSizeRel"
set_property(TARGET ipengine::core APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)
set_target_properties(ipengine::core PROPERTIES
  IMPORTED_IMPLIB_MINSIZEREL "${_IMPORT_PREFIX}/lib/core.lib"
  IMPORTED_LOCATION_MINSIZEREL "${_IMPORT_PREFIX}/bin/core.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS ipengine::core )
list(APPEND _IMPORT_CHECK_FILES_FOR_ipengine::core "${_IMPORT_PREFIX}/lib/core.lib" "${_IMPORT_PREFIX}/bin/core.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
