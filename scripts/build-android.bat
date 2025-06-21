@echo off
title iLuma Build Script for Android

set debug_or_release=%~1

if not exist gradlew.bat (
    echo gradlew.bat not found in %cd%
    exit /b 1
)

if "%debug_or_release%"=="release" (
    call gradlew.bat assembleRelease
    set exitCode=%ERRORLEVEL%
) else if "%debug_or_release%"=="debug" (
    call gradlew.bat assembleDebug
    set exitCode=%ERRORLEVEL%
) else (
    echo Invalid argument. Use 'release' or 'debug'.
    exit /b 1
)

exit /b %exitCode%
