@echo off
title iLuma - Start

set /p platform="Enter platform to start app (e.g., windows, android, linux): "

if "%platform%"=="windows" (
    goto set_debug_or_release_mode
) else if "%platform%"=="android" (
    goto set_debug_or_release_mode
) else (
    goto start_platform
)

:set_debug_or_release_mode
set /p debug_or_release="Enter debug/release mode to start app (release, debug): "

:start_platform
echo Starting the application for platform %platform%


if /i "%platform%"=="windows" (
    start ./builds/windows/%debug_or_release%/iLuma.exe
) else if /i "%platform%"=="linux" (
    wsl -e ./builds/linux/iLuma
) else if /i "%platform%"=="android" (
    for /f "delims=" %%i in ('emulator -list-avds') do (
        start "" emulator -avd %%i
    )
    adb wait-for-device
    adb install builds/android/outputs/apk/%debug_or_release%/app-%debug_or_release%.apk

    :wait_for_shutdown
    adb devices | findstr /i emulator >nul
    if %ERRORLEVEL% == 0 (
        timeout /t 1 >nul
        goto wait_for_shutdown
    )
    echo Emulator has shut down.

) else if "%platform%"=="web"  (
    emsdk activate
    emrun --port 8080 ./builds/web/iLuma.html
) else (
    echo Unsupported platform: %platform%
    exit /b 1
)
