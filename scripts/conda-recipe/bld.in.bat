:: This file is only intended for use with CI systems, and should be
:: configurated by cmake before usage.

set CHEMHARP_BUILD_DIR=@CMAKE_BINARY_DIR@

cd %CHEMHARP_BUILD_DIR%
cmake --build . --target install -- DESTDIR=%PREFIX%

:: PowerShell is needed for glob expansion
powershell -Command "mv %PREFIX%/usr/local/lib/* %PREFIX%/lib"
powershell -Command "mv %PREFIX%/usr/local/include/* %PREFIX%/include"

del /F /S %PREFIX%/usr/local

mkdir conda-build
cd conda-build
cmake -DBUILD_FRONTEND=OFF -DPYTHON_BINDING=ON -DCMAKE_INSTALL_PREFIX=%PREFIX% ..
cmake --build . --target install --config release
