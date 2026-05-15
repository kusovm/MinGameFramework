@echo off
setlocal

set BUILD_DIR=%~dp0\build_debug
set EXE_NAME=framework_x64_debug.exe

set SRC_DIR=%~dp0\src
set SRC=                            ^
    "%SRC_DIR%\entities.cpp"        ^
    "%SRC_DIR%\localization.cpp"    ^
    "%SRC_DIR%\renderer.cpp"        ^
    "%SRC_DIR%\scenes.cpp"          ^
    "%SRC_DIR%\state.cpp"           ^
    "%SRC_DIR%\time.cpp"            ^
    "%SRC_DIR%\utils.cpp"           ^
    "%SRC_DIR%\main.cpp"

set LIBS=winmm.lib User32.lib

set CFLAGS=/Z7 /Od /D _DEBUG /JMC /RTC1 /MTd /EHsc /std:c++17
set LDFLAGS=/INCREMENTAL:NO

:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

where /q cl || (
    echo [Debug] cl not found; run this from x64 Native Tools Command Prompt for VS
    exit /b 1
)

if "%Platform%" neq "x64" (
    echo [Debug] Platform is not x64; run this from x64 Native Tools Command Prompt for VS
    exit /b 1
)

if not exist "%BUILD_DIR%" (
    mkdir "%BUILD_DIR%"
)

pushd "%BUILD_DIR%"

if not exist "obj\" (
    mkdir "obj\"
)

pushd "obj\"

echo [Debug] Building "%CD%\%EXE_NAME%":
call cl /nologo /W3 /MP %CFLAGS% %SRC% %LIBS% /Fe:..\%EXE_NAME% /link %LDFLAGS%

popd

if errorlevel 1 (
    echo [Debug] Build failed
) else (
    echo [Debug] Finished building "%CD%\%EXE_NAME%"
)

