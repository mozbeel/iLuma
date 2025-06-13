@echo off

:: Move to project root (one level up from scripts/)
pushd "%~dp0.."

:: Remember scripts folder full path
set "scripts_dir=%~dp0"

:: Inputs passed from parent build.bat: %1 = Debug/Release, %2 = mode
set "debug_or_release=%~1"
set "mode=%~2"

echo.
echo Building all platforms in %debug_or_release% [%mode%]
echo.

:: Windows
echo -> Windows
call "%scripts_dir%build-windows.bat" "%debug_or_release%" "%mode%"

:: Linux
echo -> Linux
call "%scripts_dir%build-linux.bat" "%debug_or_release%" "%mode%"

:: Android
echo -> Android
call "%scripts_dir%build-android.bat" "%debug_or_release%" "%mode%"

echo -> MacOS
call "%scripts_dir%build-macOS.bat" "%debug_or_release%" "%mode%"

popd
exit /b 0
