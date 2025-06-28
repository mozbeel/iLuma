@echo off
title iLuma Build for Windows

cd app/src/main/cpp

set debug_or_release=%~1
set mode=%~2

echo Building the application for platform windows in %debug_or_release%

echo "Starting build for Windows SteamVR in %debug_or_release% %mode%"

if "%mode%"=="c" (
    echo "Configuring CMake (for Windows SteamVR)..."
    cmake -S . -B ../../../../builds/steamvr/windows -DCMAKE_BUILD_TYPE=%debug_or_release% -DILUMA_VR=ON
) else if "%mode%"=="b" (
    echo "Building the application only (for Windows SteamVR)..."
    cmake --build ../../../../builds/steamvr/windows --config %debug_or_release%
) else if "%mode%"=="both" (
    echo "Configuring CMake and building the application (for Windows SteamVR)..."
    cmake -S . -B ../../../../builds/steamvr/windows -DCMAKE_BUILD_TYPE=%debug_or_release% -DILUMA_VR=ON
    cmake --build ../../../../builds/steamvr/windows --config %debug_or_release%
)