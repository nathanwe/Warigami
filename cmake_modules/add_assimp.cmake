include(FetchContent)

set (ASSIMP_BUILD_TESTS OFF CACHE INTERNAL "")
set (ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE INTERNAL "")
set (ASSIMP_BUILD_ZLIB ON CACHE INTERNAL "")
set (ASSIMP_NO_EXPORT ON CACHE INTERNAL "")
set (ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE INTERNAL "")
set (ASSIMP_BUILD_GLTF_IMPORTER TRUE CACHE INTERNAL "")
set (ASSIMP_BUILD_OBJ_IMPORTER TRUE CACHE INTERNAL "")
set (ASSIMP_BUILD_OGRE_IMPORTER TRUE CACHE INTERNAL "") # Contains common Animation namespace

FetchContent_Declare(assimp
        GIT_REPOSITORY https://github.com/assimp/assimp.git
        GIT_TAG 8f0c6b04b2257a520aaab38421b2e090204b69df)

FetchContent_MakeAvailable(assimp)
