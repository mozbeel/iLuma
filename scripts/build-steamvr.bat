@echo off

title iLuma Build for SteamVR

:: Move to project root (one level up from scripts/)
pushd "%~dp0.."

:: Remember scripts folder full path
set "scripts_dir=%~dp0"

:: Inputs passed from parent build.bat: %1 = Debug/Release, %2 = mode
set "debug_or_release=%~1"
set "mode=%~2"

echo.
echo Building SteamVR platforms in %debug_or_release% [%mode%]
echo.

call "%scripts_dir%build-steamvr-windows.bat" "%debug_or_release%" "%mode%"
call "%scripts_dir%build-steamvr-linux.bat" "%debug_or_release%" "%mode%"

exit /b 0
