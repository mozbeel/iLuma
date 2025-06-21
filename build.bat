@echo off
title iLuma Build Script

set /p platform=Enter platform to build app for (e.g., windows, linux, android, all; defaults to windows): 
set /p debug_or_release=Enter Debug/Release mode to start app (release/debug; defaults to release): 

if not "%platform%"=="android" (
    set /p mode="Configure CMake/build application only/both (c/b/both; defaults to b): "
)

@REM Set default values if not provided
if "%platform%"=="" (
    set platform=windows
)
if "%debug_or_release%"=="" (
    set debug_or_release=release
)
if "%mode%"=="" (
    set mode=b
)

echo Building the application for platform %platform% in %debug_or_release% mode

if "%platform%"=="windows"  (
    echo Starting build for Windows in %debug_or_release% mode
    call scripts\build-windows.bat %debug_or_release% %mode%
    exit /b 0
) 
if "%platform%"=="linux" (
    echo Starting build for Linux in %debug_or_release% mode
    call scripts\build-linux.bat %debug_or_release% %mode%
    exit /b 0
) 
if "%platform%"=="macos" (
    echo Starting build for MacOS in %debug_or_release% mode
    call scripts\build-macos.bat %debug_or_release% %mode%
    exit /b 0
) 

if "%platform%"=="android" (
    echo Starting build for Android in %debug_or_release% mode
    call scripts\build-android.bat %debug_or_release%
    exit /b 0
)
if "%platform%"=="ios" (
    echo Starting build for iOS in %debug_or_release% mode
    call scripts\build-ios.bat %debug_or_release% %mode%
    exit /b 0
)
if "%platform%"=="web" (
    echo Starting build for web in %debug_or_release% mode
    emsdk activate
    call scripts\build-web.bat %debug_or_release% %mode%
    exit /b 0
)   
if "%platform%"=="all" (
    echo Starting build for all platforms in %debug_or_release% mode
    call scripts\build-all.bat %debug_or_release% %mode%
    exit /b 0
) 
echo Unsupported platform: %platform%
exit /b 1