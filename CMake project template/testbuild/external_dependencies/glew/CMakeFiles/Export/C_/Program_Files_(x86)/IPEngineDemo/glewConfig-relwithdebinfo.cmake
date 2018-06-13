#----------------------------------------------------------------
# Generated CMake target import file for configuration "RelWithDebInfo".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "glew::glew" for configuration "RelWithDebInfo"
set_property(TARGET glew::glew APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(glew::glew PROPERTIES
  IMPORTED_IMPLIB_RELWITHDEBINFO "${_IMPORT_PREFIX}/lib/glew.lib"
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/bin/glew.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS glew::glew )
list(APPEND _IMPORT_CHECK_FILES_FOR_glew::glew "${_IMPORT_PREFIX}/lib/glew.lib" "${_IMPORT_PREFIX}/bin/glew.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
