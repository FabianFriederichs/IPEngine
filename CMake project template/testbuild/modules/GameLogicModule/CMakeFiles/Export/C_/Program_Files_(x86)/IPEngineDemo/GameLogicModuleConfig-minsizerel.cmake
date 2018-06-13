#----------------------------------------------------------------
# Generated CMake target import file for configuration "MinSizeRel".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "ipengine::GameLogicModule" for configuration "MinSizeRel"
set_property(TARGET ipengine::GameLogicModule APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)
set_target_properties(ipengine::GameLogicModule PROPERTIES
  IMPORTED_IMPLIB_MINSIZEREL "${_IMPORT_PREFIX}/lib/GameLogicModule.lib"
  IMPORTED_LOCATION_MINSIZEREL "${_IMPORT_PREFIX}/bin/GameLogicModule.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS ipengine::GameLogicModule )
list(APPEND _IMPORT_CHECK_FILES_FOR_ipengine::GameLogicModule "${_IMPORT_PREFIX}/lib/GameLogicModule.lib" "${_IMPORT_PREFIX}/bin/GameLogicModule.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
