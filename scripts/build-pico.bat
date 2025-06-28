@echo off
title iLuma Build Script for Pico

set debug_or_release=%~1
set /p bundle_or_apk=Enter to build APK or App Bundle (apk/bundle; defaults to apk): 

if not exist gradlew.bat (
    echo gradlew.bat not found in %cd%
    exit /b 1
)

if "%debug_or_release%"=="release" (
    if "%bundle_or_apk%"=="bundle" (
        echo Building App Bundle in Release mode...
        call gradlew.bat bundleRelease -PabiList="arm64-v8a" -PbuildDir="../builds/pico" -DcmakeEArgs="-DILUMA_VR=ON"
    ) else (
        echo Building APK in Release mode...
        call gradlew.bat assembleRelease -PabiList="arm64-v8a" -PbuildDir="../builds(pico" -DcmakeEArgs="-DILUMA_VR=ON"
    )
) else if "%debug_or_release%"=="debug" (
    if "%bundle_or_apk%"=="bundle" (
        echo Building App Bundle in Debug mode...
        call gradlew.bat bundleDebug -PabiList="arm64-v8a" -PbuildDir="../builds/pico" -DcmakeEArgs="-DILUMA_VR=ON"
    ) else (
        echo Building APK in Debug mode...
        call gradlew.bat assembleDebug -PabiList="arm64-v8a" -PbuildDir="../builds/pico" -DcmakeEArgs="-DILUMA_VR=ON"
    )
) else (
    echo Invalid argument. Use 'release' or 'debug'.
    exit /b 1
)

exit /b %exitCode%
