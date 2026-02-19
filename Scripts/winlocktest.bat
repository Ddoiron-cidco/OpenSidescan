@echo off
setlocal

if not exist test\win-fileLock-test\build mkdir test\win-fileLock-test\build
if not exist build\reports mkdir build\reports

pushd test\win-fileLock-test\build || exit /b 1

cmake ..
if errorlevel 1 (
    set "ERR=%ERRORLEVEL%"
    popd
    exit /b %ERR%
)

MSBuild.exe wintest.sln /property:configuration=Debug
set "ERR=%ERRORLEVEL%"
popd
exit /b %ERR%
