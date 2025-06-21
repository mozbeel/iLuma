@echo off
title iLuma Build for iOS via GitHub Actions

setlocal enabledelayedexpansion

set "debug_or_release=%~1"
set "mode=%~2"

git add .
git commit -m "Update for iOS build" --allow-empty
git push origin main
cd app/src/main/cpp

if "%mode%"=="c" goto configure
if "%mode%"=="b" goto build_only
if "%mode%"=="both" goto build_both

echo Invalid mode: %mode%
exit /b 1

:configure
echo === Configuring only (CMake) ===
gh workflow run "Build for iOS" -f build_type=%debug_or_release% -f mode=c
goto wait

:build_only
echo === Building only ===
gh workflow run "Build for iOS" -f build_type=%debug_or_release% -f mode=b
goto wait

:build_both
echo === Configuring and Building ===
gh workflow run "Build for iOS" -f build_type=%debug_or_release% -f mode=both
goto wait

:wait
timeout /t 10 /nobreak >nul
for /f %%i in ('gh run list --limit 1 --json databaseId --jq ".[0].databaseId"') do set RUN_ID=%%i

echo Waiting for workflow completion...
gh run view %RUN_ID% --web

:poll_status
for /f %%s in ('gh run view %RUN_ID% --json status --jq ".status"') do set status=%%s
for /f %%c in ('gh run view %RUN_ID% --json conclusion --jq ".conclusion"') do set conclusion=%%c

echo Status: %status%, Conclusion: %conclusion%

if /i "%status%"=="completed" (
    if /i "%conclusion%"=="success" (
        echo ✅ Workflow succeeded. Downloading artifacts...
        goto download
    ) else (
        echo ❌ Workflow failed. Check the logs.
        exit /b 1
    )
) else (
    timeout /t 10 /nobreak >nul
    goto poll_status
)

:download
if exist ../../../../builds/iOS (
    rmdir /s /q ../../../../builds/iOS
)
gh run download %RUN_ID% --name iOS --dir ../../../../builds/iOS

echo ✅ Download complete.
exit /b 0
