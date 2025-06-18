@echo off

title iLuma Build Script for Android

set debug_or_release=%~1

if "%debug_or_release%"=="release" (
    gradle assembleRelease
) else if "%debug_or_release%"=="debug" (
    gradle assembleDebug
) else (
    echo Invalid argument. Use 'release' or 'debug'.
    exit /b 1
)