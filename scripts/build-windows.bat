@echo off
set debug_or_release=%~1
set mode=%~2

echo Building the application for platform windows in %debug_or_release%

if "%mode%"=="s" (
    echo "Rebuilding the Solution (for Windows)..."
    cmake -S . -B builds/windows -DCMAKE_BUILD_TYPE=%debug_or_release%
) else if "%mode%"=="l" (
    echo "Building the library only (for Windows)...""
    cmake --build builds/windows --config %debug_or_release%
) else if "%mode%"=="both" (
    echo "Building both the Solution and library (for Windows)..."
    cmake -S . -B builds/windows -DCMAKE_BUILD_TYPE=%debug_or_release%
    cmake --build builds/windows --config %debug_or_release%
)