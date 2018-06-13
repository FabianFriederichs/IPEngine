#----------------------------------------------------------------
# Generated CMake target import file for configuration "RelWithDebInfo".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "ipengine::SimpleContentModule" for configuration "RelWithDebInfo"
set_property(TARGET ipengine::SimpleContentModule APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(ipengine::SimpleContentModule PROPERTIES
  IMPORTED_IMPLIB_RELWITHDEBINFO "${_IMPORT_PREFIX}/lib/SimpleContentModule.lib"
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/bin/SimpleContentModule.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS ipengine::SimpleContentModule )
list(APPEND _IMPORT_CHECK_FILES_FOR_ipengine::SimpleContentModule "${_IMPORT_PREFIX}/lib/SimpleContentModule.lib" "${_IMPORT_PREFIX}/bin/SimpleContentModule.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
