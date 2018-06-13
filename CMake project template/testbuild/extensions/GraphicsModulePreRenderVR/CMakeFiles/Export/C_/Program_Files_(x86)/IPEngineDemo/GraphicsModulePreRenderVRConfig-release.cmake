#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "ipengine::GraphicsModulePreRenderVR" for configuration "Release"
set_property(TARGET ipengine::GraphicsModulePreRenderVR APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(ipengine::GraphicsModulePreRenderVR PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/GraphicsModulePreRenderVR.lib"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/GraphicsModulePreRenderVR.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS ipengine::GraphicsModulePreRenderVR )
list(APPEND _IMPORT_CHECK_FILES_FOR_ipengine::GraphicsModulePreRenderVR "${_IMPORT_PREFIX}/lib/GraphicsModulePreRenderVR.lib" "${_IMPORT_PREFIX}/bin/GraphicsModulePreRenderVR.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
