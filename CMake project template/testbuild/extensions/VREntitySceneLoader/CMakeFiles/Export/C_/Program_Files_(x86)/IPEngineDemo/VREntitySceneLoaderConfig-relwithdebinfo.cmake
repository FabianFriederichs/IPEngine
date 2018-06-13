#----------------------------------------------------------------
# Generated CMake target import file for configuration "RelWithDebInfo".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "ipengine::VREntitySceneLoader" for configuration "RelWithDebInfo"
set_property(TARGET ipengine::VREntitySceneLoader APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(ipengine::VREntitySceneLoader PROPERTIES
  IMPORTED_IMPLIB_RELWITHDEBINFO "${_IMPORT_PREFIX}/lib/VREntitySceneLoader.lib"
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/bin/VREntitySceneLoader.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS ipengine::VREntitySceneLoader )
list(APPEND _IMPORT_CHECK_FILES_FOR_ipengine::VREntitySceneLoader "${_IMPORT_PREFIX}/lib/VREntitySceneLoader.lib" "${_IMPORT_PREFIX}/bin/VREntitySceneLoader.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
