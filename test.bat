@echo off
setlocal

set GENERATOR="Visual Studio 14 2015 Win64"
set CONFIG=Debug

pushd %~dp0

set "PROJECT=%~1"
set PROJECT_DIR="_out\%~1"

if "%PROJECT%"=="Log" set EXTRA=-DUSE_wchar_t=off

if NOT EXIST %PROJECT_DIR% md %PROJECT_DIR%

echo on
cd %PROJECT_DIR%
cmake -G %GENERATOR% %EXTRA% "..\..\%PROJECT%\" && cmake --build . --config %CONFIG% -j && ".\%CONFIG%\%PROJECT%Tests"

popd
