@echo off
title iLuma Build for iOS

set debug_or_release=%~1
set mode=%~2

git add .
git commit -m "Update CMakeLists.txt and source files for iOS build" --allow-empty
git push origin main
cd app/src/main/cpp

if "%mode%"=="c" (
    echo Configuring CMake (for iOS)...
    for /f "tokens=5 delims=: " %%a in ('gh workflow run "Build for iOS" -f build_type=%debug_or_release% -f mode=c') do set RUN_ID=%%a

    echo Triggered workflow run ID: %RUN_ID%

    rem Wait for the run to appear in the list
    :wait_run_c
    timeout /t 5 /nobreak >nul
    set FOUND_RUN_ID=
    for /f %%i in ('gh run list --workflow="Build for iOS" --limit 10 --json databaseId --jq ".[].databaseId"') do (
        if "%%i"=="%RUN_ID%" set FOUND_RUN_ID=%%i
    )
    if defined FOUND_RUN_ID (
        echo Run %RUN_ID% found.
    ) else (
        goto wait_run_c
    )

    gh run view %RUN_ID% --web

    :wait-for-completion_c
    for /f %%s in ('gh run view %RUN_ID% --json status --jq ".status"') do set status=%%s
    for /f %%s in ('gh run view %RUN_ID% --json conclusion --jq ".conclusion"') do set conclusion=%%s

    echo Status: %status%, Conclusion: %conclusion%

    if "%status%"=="completed" (
        if "%conclusion%"=="success" (
            echo CMake configuration completed successfully.
            goto done_c
        ) else (
            echo CMake configuration failed. Check logs.
            exit /b 1
        )
    ) else (
        timeout /t 10 /nobreak >nul
        goto wait-for-completion_c
    )

    :done_c
    if exist ../../../../builds/iOS (
        rmdir /s /q ../../../../builds/iOS
    )
    gh run download %RUN_ID% --name iOS --dir ../../../../builds/iOS

    exit /b 0

) else if "%mode%"=="b" (
    echo Building the application only (for iOS)...
    for /f "tokens=5 delims=: " %%a in ('gh workflow run "Build for iOS" -f build_type=%debug_or_release% -f mode=b') do set RUN_ID=%%a

    echo Triggered workflow run ID: %RUN_ID%

    rem Wait for the run to appear in the list
    :wait_run_b
    timeout /t 5 /nobreak >nul
    set FOUND_RUN_ID=
    for /f %%i in ('gh run list --workflow="Build for iOS" --limit 10 --json databaseId --jq ".[].databaseId"') do (
        if "%%i"=="%RUN_ID%" set FOUND_RUN_ID=%%i
    )
    if defined FOUND_RUN_ID (
        echo Run %RUN_ID% found.
    ) else (
        goto wait_run_b
    )

    gh run view %RUN_ID% --web

    :wait-for-completion_b
    for /f %%s in ('gh run view %RUN_ID% --json status --jq ".status"') do set status=%%s
    for /f %%s in ('gh run view %RUN_ID% --json conclusion --jq ".conclusion"') do set conclusion=%%s

    echo Status: %status%, Conclusion: %conclusion%

    if "%status%"=="completed" (
        if "%conclusion%"=="success" (
            echo Building application completed successfully.
            goto done_b
        ) else (
            echo Building application failed. Check logs.
            exit /b 1
        )
    ) else (
        timeout /t 10 /nobreak >nul
        goto wait-for-completion_b
    )

    :done_b
    if exist ../../../../builds/iOS (
        rmdir /s /q ../../../../builds/iOS
    )
    gh run download %RUN_ID% --name iOS --dir ../../../../builds/iOS

    exit /b 0

) else if "%mode%"=="both" (
    echo Configuring CMake and building the application (for iOS)...
    for /f "tokens=5 delims=: " %%a in ('gh workflow run "Build for iOS" -f build_type=%debug_or_release% -f mode=both') do set RUN_ID=%%a

    echo Triggered workflow run ID: %RUN_ID%

    rem Wait for the run to appear in the list
    :wait_run_both
    timeout /t 5 /nobreak >nul
    set FOUND_RUN_ID=
    for /f %%i in ('gh run list --workflow="Build for iOS" --limit 10 --json databaseId --jq ".[].databaseId"') do (
        if "%%i"=="%RUN_ID%" set FOUND_RUN_ID=%%i
    )
    if defined FOUND_RUN_ID (
        echo Run %RUN_ID% found.
    ) else (
        goto wait_run_both
    )

    gh run view %RUN_ID% --web

    :wait-for-completion_both
    for /f %%s in ('gh run view %RUN_ID% --json status --jq ".status"') do set status=%%s
    for /f %%s in ('gh run view %RUN_ID% --json conclusion --jq ".conclusion"') do set conclusion=%%s

    echo Status: %status%, Conclusion: %conclusion%

    if "%status%"=="completed" (
        if "%conclusion%"=="success" (
            echo Building application completed successfully.
            goto done_both
        ) else (
            echo Building application failed. Check logs.
            exit /b 1
        )
    ) else (
        timeout /t 10 /nobreak >nul
        goto wait-for-completion_both
    )

    :done_both
    if exist ../../../../builds/iOS (
        rmdir /s /q ../../../../builds/iOS
    )
    gh run download %RUN_ID% --name iOS --dir ../../../../builds/iOS

    exit /b 0

) else (
    echo Invalid mode: %mode%
    exit /b 1
)
