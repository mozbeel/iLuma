@echo off
title iLuma Build for Web

set debug_or_release=%~1
set mode=%~2

echo Building the application for platform windows in %debug_or_release%

echo "Starting build for Web in %debug_or_release% %mode%"

emsdk activate

if "%mode%"=="c" (
    echo "Configuring CMake (for Web)..."
    emcmake cmake -S app/src/main/cpp -B builds/web
) else if "%mode%"=="b" (
    echo "Building the application only (for Web)..."
    cmake --build builds/web --config %debug_or_release%
) else if "%mode%"=="both" (
    echo "Configuring CMake and building the application (for Windows)..."
    emcmake cmake -S app/src/main/cpp -B builds/web -DCMAKE_BUILD_TYPE=%debug_or_release%
    cmake --build builds/web --config %debug_or_release%
)