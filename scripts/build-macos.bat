@echo off
title iLuma Build for MacOS

set debug_or_release=%~1
set mode=%~2

git add .
git commit -m "Updating Repository for macOS build" --allow-empty
git push origin main

cd app/src/main/cpp

if "%mode%"=="c" (
    echo "Configuring CMake (for MacOS)..."
    gh workflow run "Build for macOS" -f build_type=%debug_or_release% -f mode=c

    timeout /t 10 /nobreak >nul
    for /f %%i in ('gh run list --limit 1 --json databaseId --jq ".[0].databaseId"') do set RUN_ID=%%i

    gh run view %RUN_ID% --web

:wait-for-completion_c
    for /f %%s in ('gh run view %RUN_ID% --json status --jq ".status"') do set status=%%s
    for /f %%s in ('gh run view %RUN_ID% --json status --jq ".conclusion"') do set conclusion=%%s

    if "%status%"=="completed" (
        echo "CMake configuration completed successfully."
        goto done_c
    ) else if "%conclusion%"=="failure" (
        echo "CMake configuration failed. Please check the logs."
        exit /b 1
    ) else (
        goto wait-for-completion_c
    )

:done_c
    if exist ../../../../builds/macOS (
        rmdir /s /q ../../../../builds/macOS
    )
    gh run download--name macOS --dir ../../../../builds/macOS

    exit /b 0

) else if "%mode%"=="b" (
    echo "Building the application only (for MacOS)..."
    gh workflow run "Build for macOS" -f build_type=%debug_or_release% -f mode=b

    timeout /t 10 /nobreak >nul
    for /f %%i in ('gh run list --limit 1 --json databaseId --jq ".[0].databaseId"') do set RUN_ID=%%i

    gh run view %RUN_ID% --web

:wait-for-completion_b
    for /f %%s in ('gh run view %RUN_ID% --json status --jq ".status"') do set status=%%s
    for /f %%s in ('gh run view %RUN_ID% --json status --jq ".conclusion"') do set conclusion=%%s

    if "%status%"=="completed" (
        echo "Building application completed successfully. Downloading..."
        goto done_b
    ) else if "%conclusion%"=="failure" (
        echo "Building application failed. Please check the logs."
        exit /b 1
    ) else (
        goto wait-for-completion_b
    )

:done_b
    if exist ../../../../builds/macOS (
        rmdir /s /q ../../../../builds/macOS
    )
    gh run download --name macOS --dir ../../../../builds/macOS

    exit /b 0

) else if "%mode%"=="both" (
    echo "Configuring CMake and building the application (for MacOS)..."
    gh workflow run "Build for macOS" -f build_type=%debug_or_release% -f mode=both

    timeout /t 10 /nobreak >nul
    for /f %%i in ('gh run list --limit 1 --json databaseId --jq ".[0].databaseId"') do set RUN_ID=%%i

    gh run view %RUN_ID% --web

:wait-for-completion_both
    for /f %%s in ('gh run view %RUN_ID% --json status --jq ".status"') do set status=%%s
    for /f %%s in ('gh run view %RUN_ID% --json status --jq ".conclusion"') do set conclusion=%%s

    if "%status%"=="completed" (
        echo "Building application completed successfully. Downloading..."
        goto done_both
    ) else if "%conclusion%"=="failure" (
        echo "Building application failed. Please check the logs."
        exit /b 1
    ) else (
        goto wait-for-completion_both
    )

:done_both
    if exist ../../../../builds/macOS (
        rmdir /s /q ../../../../builds/macOS
    )
    gh run download --name macOS --dir ../../../../builds/macOS

    exit /b 0
)