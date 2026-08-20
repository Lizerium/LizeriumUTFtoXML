@echo off
setlocal

set "ROOT=%~dp0"
set "PROJECT=%ROOT%UTFXML\UTFXML.vcxproj"

set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"

for /f "usebackq tokens=*" %%i in (`"%VSWHERE%" -latest -products * -requires Microsoft.Component.MSBuild -property installationPath`) do (
    set "VS_PATH=%%i"
)

call "%VS_PATH%\Common7\Tools\VsDevCmd.bat" -arch=x86 -host_arch=x64

msbuild "%PROJECT%" ^
    /m ^
    /p:Configuration=Debug ^
    /p:Platform=Win32

exit /b %ERRORLEVEL%