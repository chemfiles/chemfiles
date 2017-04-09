if ("$env:CXX_PATH" -ne "") {
    $env:PATH += ";$env:CXX_PATH"
}

$env:CMAKE_ARGUMENTS = "-G `"$env:generator`""
$env:CMAKE_ARGUMENTS += " -DBUILD_SHARED_LIBS=$env:SHARED_LIBS"
$env:CMAKE_ARGUMENTS += " -DCMAKE_BUILD_TYPE=$env:configuration"
$env:CMAKE_ARGUMENTS += " -DCHFL_BUILD_TESTS=ON"

if ($env:generator -Match "Visual Studio") {
    $env:BUILD_ARGUMENTS="/verbosity:minimal"
} else {
    $env:BUILD_ARGUMENTS=""
}

if ($env:generator -eq "MinGW Makefiles") {
    # Remove sh.exe from git in the PATH for MinGW to work
    $env:PATH = ($env:PATH.Split(';') | Where-Object { $_ -ne 'C:\Program Files\Git\usr\bin' }) -join ';'
}
