@echo off
set debug_or_release=%~1
set mode=%~2

if "%mode%"=="s" (
    echo "Rebuilding the Solution (for Android)..."
    cmake -S . -B builds/android -G "Ninja" -DCMAKE_TOOLCHAIN_FILE="C:\Users\moz04\AppData\Local\Android\Sdk\ndk\29.0.13599879\build\cmake\android.toolchain.cmake" -DANDROID_ABI=armeabi-v7a -DANDROID_PLATFORM=android-21 -DANDROID_STL=c++_static -DCMAKE_BUILD_TYPE=%debug_or_release%
) else if "%mode%"=="l" (
    echo "Building the library only (for Android)..."
    cmake --build builds/android
) else if "%mode%"=="both" (
    echo "Building both the Solution and library (for Android)..."
    cmake -S . -B builds/android -G "Ninja" -DCMAKE_TOOLCHAIN_FILE="C:\Users\moz04\AppData\Local\Android\Sdk\ndk\29.0.13599879\build\cmake\android.toolchain.cmake" -DANDROID_ABI=armeabi-v7a -DANDROID_PLATFORM=android-21 -DANDROID_STL=c++_static -DCMAKE_BUILD_TYPE=%debug_or_release%
    cmake --build builds/android
)