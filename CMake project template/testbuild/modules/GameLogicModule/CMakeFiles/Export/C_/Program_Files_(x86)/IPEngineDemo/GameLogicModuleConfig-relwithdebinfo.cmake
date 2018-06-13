#----------------------------------------------------------------
# Generated CMake target import file for configuration "RelWithDebInfo".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "ipengine::GameLogicModule" for configuration "RelWithDebInfo"
set_property(TARGET ipengine::GameLogicModule APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(ipengine::GameLogicModule PROPERTIES
  IMPORTED_IMPLIB_RELWITHDEBINFO "${_IMPORT_PREFIX}/lib/GameLogicModule.lib"
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/bin/GameLogicModule.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS ipengine::GameLogicModule )
list(APPEND _IMPORT_CHECK_FILES_FOR_ipengine::GameLogicModule "${_IMPORT_PREFIX}/lib/GameLogicModule.lib" "${_IMPORT_PREFIX}/bin/GameLogicModule.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
