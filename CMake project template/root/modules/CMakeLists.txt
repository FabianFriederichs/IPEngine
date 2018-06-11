cmake_minimum_required(VERSION 3.0)

# add all module subdirectories
add_subdirectory(BasicOpenVRModule)
setTargetFolder(BasicOpenVRModule modules)

add_subdirectory(DataStoreModule)
setTargetFolder(DataStoreModule modules)

add_subdirectory(GameLogicModule)
setTargetFolder(GameLogicModule modules)

add_subdirectory(GraphicsModule)
setTargetFolder(GraphicsModule modules)

add_subdirectory(InputModule)
setTargetFolder(InputModule modules)

add_subdirectory(PhysicsModule)
setTargetFolder(PhysicsModule modules)

add_subdirectory(SimpleContentModule)
setTargetFolder(SimpleContentModule modules)

add_subdirectory(SimpleSceneModule)
setTargetFolder(SimpleSceneModule modules)