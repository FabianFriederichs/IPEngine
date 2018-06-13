#----------------------------------------------------------------
# Generated CMake target import file for configuration "RelWithDebInfo".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "ipengine::DataStoreModule" for configuration "RelWithDebInfo"
set_property(TARGET ipengine::DataStoreModule APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(ipengine::DataStoreModule PROPERTIES
  IMPORTED_IMPLIB_RELWITHDEBINFO "${_IMPORT_PREFIX}/lib/DataStoreModule.lib"
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/bin/DataStoreModule.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS ipengine::DataStoreModule )
list(APPEND _IMPORT_CHECK_FILES_FOR_ipengine::DataStoreModule "${_IMPORT_PREFIX}/lib/DataStoreModule.lib" "${_IMPORT_PREFIX}/bin/DataStoreModule.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
