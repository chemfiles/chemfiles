# Workaround for the issue with mingw
# see https://github.com/appveyor/ci/issues/1804
$cmake_uninstall = "${env:ProgramFiles(x86)}\CMake\Uninstall.exe"
if([IO.File]::Exists($cmake_uninstall)) {
    Write-Host "Uninstalling previous CMake ..." -ForegroundColor Cyan
    # uninstall existent
    "`"$cmake_uninstall`" /S" | out-file ".\uninstall-cmake.cmd" -Encoding ASCII
    & .\uninstall-cmake.cmd
    del .\uninstall-cmake.cmd
    Start-Sleep -s 10
}

Write-Host "Installing CMake 3.8.2 ..." -ForegroundColor Cyan
$cmake_install = "$($env:TEMP)\cmake-3.8.2-win32-x86.msi"

(New-Object Net.WebClient).DownloadFile('https://cmake.org/files/v3.8/cmake-3.8.2-win32-x86.msi', $cmake_install)

cmd /c start /wait msiexec /i $cmake_install /quiet
del $cmake_install

add-path 'C:\Program Files (x86)\CMake\bin'
remove-path 'C:\ProgramData\chocolatey\bin'
add-path 'C:\ProgramData\chocolatey\bin'

Write-Host "CMake 3.8.2 installed" -ForegroundColor Green
# end of the workaround

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
