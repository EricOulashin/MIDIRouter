@echo off
REM Run from Visual Studio x64 Developer Command Prompt.
REM Generates MIDIRouter.sln under %%CD%%\build-vs2022-x64 — open that folder in Visual Studio.
REM Set CMAKE_PREFIX_PATH to your Qt 6 MSVC kit before running (example):
REM   set CMAKE_PREFIX_PATH=C:\Qt\6.7.0\msvc2019_64

cd /d "%~dp0\.."
cmake -S . -B vs\build-vs2022-x64 -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release
echo.
echo Solution: %CD%\vs\build-vs2022-x64\MIDIRouter.sln
