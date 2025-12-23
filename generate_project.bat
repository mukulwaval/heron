@echo off
echo Generating Heron VS Solution...

vendor\premake\premake5.exe clean
vendor\premake\premake5.exe vs2022

if %errorlevel% neq 0 (
    echo.
    echo Premake failed. Check errors above.
    pause
    exit /b %errorlevel%
)

echo.
echo Done. Open Heron.sln in Visual Studio.
pause
