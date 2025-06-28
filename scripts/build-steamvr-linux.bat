@echo off
title iLuma Build for Linux

cd app/src/main/cpp

set debug_or_release=%~1
set mode=%~2

if "%mode%"=="c" (
    echo "Configuring CMake (for Linux SteamVR)..."
    wsl -e cmake -S . -B ../../../../builds/steamvr/linux -DCMAKE_BUILD_TYPE=%debug_or_release% -DILUMA_VR=ON
) else if "%mode%"=="b" (
    echo "Building the application only (for Linux SteamVR)..."
    wsl -e cmake --build ../../../../builds/steamvr/linux --config %debug_or_release%
) else if "%mode%"=="both" (
    echo "Configuring CMake and building the application (for Linux SteamVR)..."
    wsl -e cmake -S . -B ../../../../builds/steamvr/linux -DCMAKE_BUILD_TYPE=%debug_or_release% -DILUMA_VR=ON
    wsl -e cmake --build ../../../../builds/steamvr/linux --config %debug_or_release%
)