@echo off
title iLuma - Start

set /p platform="Enter platform to start app (e.g., windows, android, linux): "
if /i "%platform%"=="windows" (
    set /p debug_or_release="Enter debug/release mode to start app (release, debug): "
    
)
echo Starting the application for platform %platform%

if /i "%platform%"=="windows" (
    start ./builds/windows/%debug_or_release%/iLuma.exe
) else if /i "%platform%"=="linux" (
    wsl -e ./builds/linux/iLuma
) else if /i "%platform%"=="android" (
    echo This is not yet supported.
) else if "%platform%"=="web"  (
    emrun --port 8080 ./builds/web/iLuma.html
) else (
    echo Unsupported platform: %platform%
    exit /b 1
)