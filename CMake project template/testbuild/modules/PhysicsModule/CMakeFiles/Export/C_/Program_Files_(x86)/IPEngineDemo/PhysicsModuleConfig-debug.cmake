#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "ipengine::PhysicsModule" for configuration "Debug"
set_property(TARGET ipengine::PhysicsModule APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(ipengine::PhysicsModule PROPERTIES
  IMPORTED_IMPLIB_DEBUG "${_IMPORT_PREFIX}/lib/PhysicsModule.lib"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/bin/PhysicsModule.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS ipengine::PhysicsModule )
list(APPEND _IMPORT_CHECK_FILES_FOR_ipengine::PhysicsModule "${_IMPORT_PREFIX}/lib/PhysicsModule.lib" "${_IMPORT_PREFIX}/bin/PhysicsModule.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
