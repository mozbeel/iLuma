@echo off
title iLuma Build for Linux

cd src/main/cpp

set debug_or_release=%~1
set mode=%~2

if "%mode%"=="c" (
    echo "Configuring CMake (for Linux)..."
    wsl -e cmake -S . -B ../../../builds/linux -DCMAKE_BUILD_TYPE=%debug_or_release%
) else if "%mode%"=="b" (
    echo "Building the application only (for Linux)..."
    wsl -e cmake --build ../../../builds/linux --config %debug_or_release%
) else if "%mode%"=="both" (
    echo "Configuring CMake and building the application (for Linux)..."
    wsl -e cmake -S . -B ../../../builds/linux -DCMAKE_BUILD_TYPE=%debug_or_release%
    wsl -e cmake --build ../../../builds/linux --config %debug_or_release%
)