@echo off

title iLuma Build for All

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


call "%scripts_dir%build-windows.bat" "%debug_or_release%" "%mode%"
call "%scripts_dir%build-macos.bat" "%debug_or_release%" "%mode%"
call "%scripts_dir%build-linux.bat" "%debug_or_release%" "%mode%"

@REM call "%scripts_dir%build-android.bat" "%debug_or_release%" NOT_SUPPORTED
call "%scripts_dir%build-ios.bat" "%debug_or_release%" "%mode%"

call "%scripts_dir%build-web.bat" "%debug_or_release%" "%mode%"

popd
exit /b 0
