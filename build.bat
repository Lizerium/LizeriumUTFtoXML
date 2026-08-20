@echo off
setlocal

set "ROOT=%~dp0"
set "PROJECT=%ROOT%UTFXML\UTFXML.vcxproj"

echo.
echo ========================================
echo   Building UTFXML Release Win32
echo ========================================
echo.

if not exist "%PROJECT%" (
    echo [ERROR] Project not found:
    echo %PROJECT%
    exit /b 1
)

set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"

if not exist "%VSWHERE%" (
    echo [ERROR] vswhere.exe not found.
    echo Install Visual Studio 2022 Build Tools or Visual Studio 2022.
    exit /b 1
)

for /f "usebackq tokens=*" %%i in (`"%VSWHERE%" -latest -products * -requires Microsoft.Component.MSBuild -property installationPath`) do (
    set "VS_PATH=%%i"
)

if not defined VS_PATH (
    echo [ERROR] Visual Studio / Build Tools installation not found.
    exit /b 1
)

call "%VS_PATH%\Common7\Tools\VsDevCmd.bat" -arch=x86 -host_arch=x64

if errorlevel 1 (
    echo [ERROR] Failed to initialize Visual Studio build environment.
    exit /b 1
)

msbuild "%PROJECT%" ^
    /m ^
    /p:Configuration=Release ^
    /p:Platform=Win32

if errorlevel 1 (
    echo.
    echo ========================================
    echo   BUILD FAILED
    echo ========================================
    exit /b 1
)

echo.
echo ========================================
echo   BUILD SUCCESS
echo ========================================
echo.
echo Output:
echo %ROOT%UTFXML\Release\UTFXML.exe
echo.

exit /b 0