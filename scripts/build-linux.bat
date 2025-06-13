@echo off
set debug_or_release=%~1
set mode=%~2

if "%mode%"=="s" (
    echo "Rebuilding the Solution (for Linux)..."
    wsl -e cmake -S . -B builds/linux -DCMAKE_BUILD_TYPE=%debug_or_release%
) else if "%mode%"=="l" (
    echo "Building the library only (for Linux)..."
    wsl -e cmake --build builds/linux --config %debug_or_release%
) else if "%mode%"=="both" (
    echo "Building both the Solution and library (for Linux)..."
    wsl -e cmake -S . -B builds/linux -DCMAKE_BUILD_TYPE=%debug_or_release%
    wsl -e cmake --build builds/linux --config %debug_or_release%
)