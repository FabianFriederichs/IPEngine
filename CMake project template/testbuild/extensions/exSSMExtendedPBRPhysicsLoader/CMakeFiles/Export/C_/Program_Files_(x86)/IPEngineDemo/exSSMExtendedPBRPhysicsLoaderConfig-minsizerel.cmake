#----------------------------------------------------------------
# Generated CMake target import file for configuration "MinSizeRel".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "ipengine::exSSMExtendedPBRPhysicsLoader" for configuration "MinSizeRel"
set_property(TARGET ipengine::exSSMExtendedPBRPhysicsLoader APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)
set_target_properties(ipengine::exSSMExtendedPBRPhysicsLoader PROPERTIES
  IMPORTED_IMPLIB_MINSIZEREL "${_IMPORT_PREFIX}/lib/exSSMExtendedPBRPhysicsLoader.lib"
  IMPORTED_LOCATION_MINSIZEREL "${_IMPORT_PREFIX}/bin/exSSMExtendedPBRPhysicsLoader.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS ipengine::exSSMExtendedPBRPhysicsLoader )
list(APPEND _IMPORT_CHECK_FILES_FOR_ipengine::exSSMExtendedPBRPhysicsLoader "${_IMPORT_PREFIX}/lib/exSSMExtendedPBRPhysicsLoader.lib" "${_IMPORT_PREFIX}/bin/exSSMExtendedPBRPhysicsLoader.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
