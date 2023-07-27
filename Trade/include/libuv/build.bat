@echo off

:: 清空
::if exist "%~dp0build" ( rd /s /q "%~dp0build" )
::if exist "%~dp0lib" ( rd /s /q "%~dp0lib" )

:: 确保目录存在
if not exist "%~dp0build" ( md "%~dp0build" )
if not exist "%~dp0lib" ( md "%~dp0lib" )

:: 生成解决方案
echo --------------------------------------------------------------------------
cd build
cmake ../ -G "Visual Studio 14 2015 Win64" -DBUILD_TESTING=OFF -DCMAKE_INSTALL_PREFIX=../lib

:: 编译
echo --------------------------------------------------------------------------
echo .
cmake --build . --config Release
echo .

:: 检查错误
echo.
echo --------------------------------------------------------------------------
echo -- Finished!   [%DATE% %Time%]
echo.
if not %errorlevel% == 0 (
echo %_solution_file_% failed! Error: !errorlevel!
) else ( 
echo %_solution_file_%
echo compiled successful!
)
echo.

cd ..

copy /y "%~dp0build\Release\uv_a.lib" "%~dp0lib\uv_a.lib"
copy /y "%~dp0build\Release\uv.lib" "%~dp0lib\uv.lib"
copy /y "%~dp0build\Release\uv.dll" "%~dp0lib\uv.dll"

:: pause
:end_of_build
echo --------------------------------------------------------------------------