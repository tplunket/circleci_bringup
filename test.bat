@echo off
setlocal

set "COMPILER=Visual Studio"

::set "COMPILER_VERSION=11 2012"
set "COMPILER_VERSION=14 2015"
::set "COMPILER_VERSION=15 2017"

set COMPILER_ARCHITECTURE=Win64
::set COMPILER_ARCHITECTURE=ARM

set CONFIG=Debug
set CONFIG=Release

if "%COMPILER_ARCHITECTURE%"=="" (
    set GENERATOR="%COMPILER% %COMPILER_VERSION%"
) else (
    set GENERATOR="%COMPILER% %COMPILER_VERSION% %COMPILER_ARCHITECTURE%"
)

pushd %~dp0

set "PROJECT=%~1"
set PROJECT_DIR="_out\%~1\vs%COMPILER_VERSION:~0,2%.%COMPILER_ARCHITECTURE%"

if "%PROJECT%"=="" (
    echo Must specify a project name on the command line.
    exit /b 1
)

if NOT EXIST "%PROJECT%" (
    echo Project named %PROJECT% does not appear to exist.
    exit /b 2
)

if "%PROJECT%"=="Log" set EXTRA=-DUSE_wchar_t=off

if NOT EXIST %PROJECT_DIR% md %PROJECT_DIR%

echo on
cd %PROJECT_DIR%
cmake -G %GENERATOR% %EXTRA% "..\..\..\%PROJECT%\" && cmake --build . --config %CONFIG% -j && ".\%CONFIG%\%PROJECT%Tests"

popd
