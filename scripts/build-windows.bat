@echo off
title iLuma Build for Windows

cd src/main/cpp

set debug_or_release=%~1
set mode=%~2

echo Building the application for platform windows in %debug_or_release%

if "%mode%"=="c" (
    echo "Configuring CMake (for Windows)..."
    cmake -S . -B ../../builds/windows -DCMAKE_BUILD_TYPE=%debug_or_release%
) else if "%mode%"=="b" (
    echo "Building the application only (for Windows)...""
    cmake --build ../../builds/windows --config %debug_or_release%
) else if "%mode%"=="both" (
    echo "Configuring CMake and building the application (for Windows)..."
    cmake -S . -B ../../builds/windows -DCMAKE_BUILD_TYPE=%debug_or_release%
    cmake --build ../../builds/windows --config %debug_or_release%
)