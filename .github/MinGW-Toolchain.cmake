# Cmake Toolchain file for native MinGW compilers on Github CI windows-2019 VM

set(CMAKE_SYSTEM_NAME Windows)

set(MINGW_PREFIX "C:/ProgramData/chocolatey/lib/mingw/tools/install/mingw64/bin")

set(CMAKE_CXX_COMPILER ${MINGW_PREFIX}/x86_64-w64-mingw32-g++.exe CACHE FILEPATH "")
set(CMAKE_C_COMPILER ${MINGW_PREFIX}/x86_64-w64-mingw32-gcc.exe CACHE FILEPATH "")

set(CMAKE_LINKER ${MINGW_PREFIX}/ld.exe CACHE FILEPATH "")
set(CMAKE_AR ${MINGW_PREFIX}/gcc-ar.exe CACHE FILEPATH "")
set(CMAKE_RANLIB ${MINGW_PREFIX}/gcc-ranlib.exe CACHE FILEPATH "")


set(CMAKE_EXE_LINKER_FLAGS_INIT    "-static-libstdc++ -fuse-ld=lld")
set(CMAKE_MODULE_LINKER_FLAGS_INIT "-static-libstdc++ -fuse-ld=lld")
set(CMAKE_SHARED_LINKER_FLAGS_INIT "-static-libstdc++ -fuse-ld=lld")
