@echo off
set debug_or_release=%~1
set mode=%~2

if "%mode%"=="c" (
    echo "Configuring CMake (for MacOS)..."
    wsl -e cmake -S . -B builds/linux -DCMAKE_BUILD_TYPE=%debug_or_release%
) else if "%mode%"=="b" (
    echo "Building the application only (for MacOS)..."
    wsl -e cmake --build builds/linux --config %debug_or_release%
) else if "%mode%"=="both" (
    echo "Configuring CMake and building the application (for MacOS)..."
    wsl -e cmake -S . -B builds/linux -DCMAKE_BUILD_TYPE=%debug_or_release%
    wsl -e cmake --build builds/linux --config %debug_or_release%
)