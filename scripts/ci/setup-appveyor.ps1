if ("$env:CXX_PATH" -ne "") {
    $env:PATH = "$env:CXX_PATH;$env:PATH"
}

$env:CMAKE_ARGUMENTS = "-G `"$env:generator`""
$env:CMAKE_ARGUMENTS += " -DBUILD_SHARED_LIBS=$env:BUILD_SHARED_LIBS"
$env:CMAKE_ARGUMENTS += " -DCMAKE_BUILD_TYPE=debug"
$env:CMAKE_ARGUMENTS += " -DCHFL_BUILD_TESTS=ON"
$env:CMAKE_ARGUMENTS += " -DCHFL_BUILD_DOCTESTS=OFF"

if ($env:generator -Match "Visual Studio") {
    $env:BUILD_ARGUMENTS="/verbosity:minimal /m:2"
} elseif ($env:generator -eq "MinGW Makefiles") {
    $env:BUILD_ARGUMENTS="-j2"

    # Disable debug information for faster link time
    $env:CMAKE_ARGUMENTS += " -DCMAKE_CXX_FLAGS_DEBUG=''"
    $env:CMAKE_ARGUMENTS += " -DCMAKE_C_FLAGS_DEBUG=''"

    # Remove sh.exe from git in the PATH for MinGW to work
    $env:PATH = ($env:PATH.Split(';') | Where-Object { $_ -ne 'C:\Program Files\Git\usr\bin' }) -join ';'
}
