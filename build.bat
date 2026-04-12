@echo off
setlocal

set BUILD_TYPE=Debug
set BUILD_DIR=build\%BUILD_TYPE%

if "%CLEAN_BUILD%"=="true" (
    if exist %BUILD_DIR% rmdir /s /q %BUILD_DIR%
)

if not exist %BUILD_DIR% mkdir %BUILD_DIR%

cmake -S . -B "%BUILD_DIR%" ^
    -DCMAKE_BUILD_TYPE=%BUILD_TYPE% ^
    -G "MinGW Makefiles"

cmake --build "%BUILD_DIR%"

"%BUILD_DIR%\rendering-studies.exe"

endlocal
