@echo off
set debug_or_release=%~1
set mode=%~2

if "%mode%"=="c" (
    echo "Configuring CMake (for Android)..."
    cmake -S . -B builds/android -G "Ninja" -DCMAKE_TOOLCHAIN_FILE="C:\Users\moz04\AppData\Local\Android\Sdk\ndk\29.0.13599879\build\cmake\android.toolchain.cmake" -DANDROID_ABI=armeabi-v7a -DANDROID_PLATFORM=android-21 -DANDROID_STL=c++_static -DCMAKE_BUILD_TYPE=%debug_or_release%
) else if "%mode%"=="b" (
    echo "Building the application only (for Android)..."
    cmake --build builds/android
) else if "%mode%"=="both" (
    echo "Configuring CMake and building the application (for Android)..."
    cmake -S . -B builds/android -G "Ninja" -DCMAKE_TOOLCHAIN_FILE="C:\Users\moz04\AppData\Local\Android\Sdk\ndk\29.0.13599879\build\cmake\android.toolchain.cmake" -DANDROID_ABI=armeabi-v7a -DANDROID_PLATFORM=android-21 -DANDROID_STL=c++_static -DCMAKE_BUILD_TYPE=%debug_or_release%
    cmake --build builds/android
)