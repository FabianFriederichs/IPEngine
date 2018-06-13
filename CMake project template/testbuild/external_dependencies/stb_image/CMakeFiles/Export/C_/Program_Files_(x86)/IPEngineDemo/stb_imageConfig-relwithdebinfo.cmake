#----------------------------------------------------------------
# Generated CMake target import file for configuration "RelWithDebInfo".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "stb::stb_image" for configuration "RelWithDebInfo"
set_property(TARGET stb::stb_image APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(stb::stb_image PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELWITHDEBINFO "C"
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/lib/stb_image.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS stb::stb_image )
list(APPEND _IMPORT_CHECK_FILES_FOR_stb::stb_image "${_IMPORT_PREFIX}/lib/stb_image.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
