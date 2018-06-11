cmake_minimum_required(VERSION 3.0)

add_subdirectory(platform_abstraction_layer)
add_subdirectory(core)
setTargetFolder(core core)
add_subdirectory(injector)
setTargetFolder(injector core)
add_subdirectory(application_base)
setTargetFolder(application_base core)