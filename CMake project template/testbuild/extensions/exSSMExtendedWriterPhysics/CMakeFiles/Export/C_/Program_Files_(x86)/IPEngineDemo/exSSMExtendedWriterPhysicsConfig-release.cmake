#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "ipengine::exSSMExtendedWriterPhysics" for configuration "Release"
set_property(TARGET ipengine::exSSMExtendedWriterPhysics APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(ipengine::exSSMExtendedWriterPhysics PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/exSSMExtendedWriterPhysics.lib"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/exSSMExtendedWriterPhysics.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS ipengine::exSSMExtendedWriterPhysics )
list(APPEND _IMPORT_CHECK_FILES_FOR_ipengine::exSSMExtendedWriterPhysics "${_IMPORT_PREFIX}/lib/exSSMExtendedWriterPhysics.lib" "${_IMPORT_PREFIX}/bin/exSSMExtendedWriterPhysics.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
