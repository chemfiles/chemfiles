if ("$env:CXX_PATH" -ne "") {
    $env:PATH += ";$env:CXX_PATH"
}

$env:CMAKE_ARGUMENTS = "-G `"$env:generator`""
$env:CMAKE_ARGUMENTS += " -DCMAKE_BUILD_TYPE=$env:configuration"
$env:CMAKE_ARGUMENTS += " -DCHFL_BUILD_TESTS=ON"

if ($env:ARCH -eq "x86") {
    C:\Miniconda35\Scripts\conda install --quiet --yes libnetcdf
    $env:CMAKE_ARGUMENTS += " -DCHFL_ENABLE_NETCDF=ON"
    $env:CMAKE_ARGUMENTS += " -DNETCDF_INCLUDES=C:/Miniconda35/Library/include"
    $env:CMAKE_ARGUMENTS += " -DNETCDF_LIBRARIES=C:/Miniconda35/Library/lib/netcdf.lib"
    $env:PATH += ";C:\Miniconda35\Library\bin"
} elseif ($env:ARCH -eq "x64") {
    C:\Miniconda35-x64\Scripts\conda install --quiet --yes libnetcdf
    $env:CMAKE_ARGUMENTS += " -DCHFL_ENABLE_NETCDF=ON"
    $env:CMAKE_ARGUMENTS += " -DNETCDF_INCLUDES=C:/Miniconda35-x64/Library/include"
    $env:CMAKE_ARGUMENTS += " -DNETCDF_LIBRARIES=C:/Miniconda35-x64/Library/lib/netcdf.lib"
    $env:PATH += ";C:\Miniconda35-x64\Library\bin"
}

if ($env:generator -Match "Visual Studio") {
    $env:BUILD_ARGUMENTS="/verbosity:minimal"
} else {
    $env:BUILD_ARGUMENTS=""
}

if ($env:generator -eq "MinGW Makefiles") {
    # Remove sh.exe from git in the PATH for MinGW to work
    $env:PATH = ($env:PATH.Split(';') | Where-Object { $_ -ne 'C:\Program Files\Git\usr\bin' }) -join ';'
}
