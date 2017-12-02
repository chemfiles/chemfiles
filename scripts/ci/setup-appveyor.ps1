powershell "c:\chemfiles\scripts\ci\install-cmake-3.8.ps1"

if ("$env:CXX_PATH" -ne "") {
    $env:PATH += ";$env:CXX_PATH"
}

$env:CMAKE_ARGUMENTS = "-G `"$env:generator`""
$env:CMAKE_ARGUMENTS += " -DBUILD_SHARED_LIBS=ON"
$env:CMAKE_ARGUMENTS += " -DCMAKE_BUILD_TYPE=debug"
$env:CMAKE_ARGUMENTS += " -DCHFL_BUILD_TESTS=ON"
$env:CMAKE_ARGUMENTS += " -DCHFL_BUILD_DOCTESTS=OFF"

if ($env:generator -Match "Visual Studio") {
    $env:BUILD_ARGUMENTS="/verbosity:minimal /m:2"
} else {
    $env:BUILD_ARGUMENTS="-j2"
}

if ($env:generator -eq "MinGW Makefiles") {
    # Remove sh.exe from git in the PATH for MinGW to work
    $env:PATH = ($env:PATH.Split(';') | Where-Object { $_ -ne 'C:\Program Files\Git\usr\bin' }) -join ';'
}
