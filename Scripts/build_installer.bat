@echo off
setlocal

set "version=%~1"
if not exist build mkdir build

pushd build || exit /b 1
cmake .. -DPROJECT_VERSION=%version%
if errorlevel 1 (
    set "ERR=%ERRORLEVEL%"
    popd
    exit /b %ERR%
)

cpack -G NSIS
set "ERR=%ERRORLEVEL%"
popd
exit /b %ERR%
