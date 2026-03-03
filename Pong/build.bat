@echo off
echo Building Pong Game with vcpkg...

REM Create build directory
if not exist build mkdir build
cd build

REM Configure with CMake
echo Configuring project...
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/dev/vcpkg/scripts/buildsystems/vcpkg.cmake

REM Build the project
echo Building project...
cmake --build . --config Release

echo Build complete!
echo Executable is in: %CD%\Release\PongGame.exe
pause
