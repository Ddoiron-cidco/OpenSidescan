@echo off
setlocal

if not exist test\build mkdir test\build
if not exist build\reports mkdir build\reports

pushd build || exit /b 1
cmake ..
if errorlevel 1 (
    set "ERR=%ERRORLEVEL%"
    popd
    exit /b %ERR%
)
popd

pushd test\build || exit /b 1
cmake ..
if errorlevel 1 (
    set "ERR=%ERRORLEVEL%"
    popd
    exit /b %ERR%
)

MSBuild.exe tests.sln /property:configuration=Release
if errorlevel 1 (
    set "ERR=%ERRORLEVEL%"
    popd
    exit /b %ERR%
)

if exist Release\* move /Y Release\* . >nul
set "ERR=%ERRORLEVEL%"
popd
exit /b %ERR%
