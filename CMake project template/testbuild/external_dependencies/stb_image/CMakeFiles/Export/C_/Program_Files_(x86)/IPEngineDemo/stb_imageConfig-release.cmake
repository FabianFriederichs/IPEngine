#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "stb::stb_image" for configuration "Release"
set_property(TARGET stb::stb_image APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(stb::stb_image PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/stb_image.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS stb::stb_image )
list(APPEND _IMPORT_CHECK_FILES_FOR_stb::stb_image "${_IMPORT_PREFIX}/lib/stb_image.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
