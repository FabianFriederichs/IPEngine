#----------------------------------------------------------------
# Generated CMake target import file for configuration "RelWithDebInfo".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "ipengine::demo" for configuration "RelWithDebInfo"
set_property(TARGET ipengine::demo APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(ipengine::demo PROPERTIES
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/bin/demo.exe"
  )

list(APPEND _IMPORT_CHECK_TARGETS ipengine::demo )
list(APPEND _IMPORT_CHECK_FILES_FOR_ipengine::demo "${_IMPORT_PREFIX}/bin/demo.exe" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
