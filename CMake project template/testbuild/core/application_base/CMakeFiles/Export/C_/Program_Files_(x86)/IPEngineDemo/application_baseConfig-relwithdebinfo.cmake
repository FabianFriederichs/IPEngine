#----------------------------------------------------------------
# Generated CMake target import file for configuration "RelWithDebInfo".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "ipengine::application_base" for configuration "RelWithDebInfo"
set_property(TARGET ipengine::application_base APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(ipengine::application_base PROPERTIES
  IMPORTED_IMPLIB_RELWITHDEBINFO "${_IMPORT_PREFIX}/lib/application_base.lib"
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/bin/application_base.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS ipengine::application_base )
list(APPEND _IMPORT_CHECK_FILES_FOR_ipengine::application_base "${_IMPORT_PREFIX}/lib/application_base.lib" "${_IMPORT_PREFIX}/bin/application_base.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
