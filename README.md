# Pong Game - Refactored

A modern, refactored Pong game built with SDL2, featuring both local and network multiplayer modes.

## Features

- 🎮 **Local Multiplayer**: Two players on the same computer
- 🌐 **Network Multiplayer**: Play over LAN or internet
- 🎯 **Improved Physics**: Better ball collision and paddle reflection
- 🖥️ **Modern Architecture**: Clean, maintainable C++ code
- ⚡ **Performance**: Consistent 60 FPS with proper frame timing
- 🛡️ **Error Handling**: Comprehensive error handling and recovery

## Prerequisites

- Visual Studio 2019 or later (with C++ development tools)
- Git
- CMake 3.15 or later

## Setup with vcpkg (Recommended)

### 1. Install vcpkg

```powershell
# Open PowerShell as Administrator
cd C:\
mkdir dev
cd dev

# Clone vcpkg
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg

# Bootstrap vcpkg
.\bootstrap-vcpkg.bat

# Add to PATH (optional)
[Environment]::SetEnvironmentVariable("PATH", $env:PATH + ";C:\dev\vcpkg", [EnvironmentVariableTarget]::User)
```

### 2. Install SDL2 Dependencies

```powershell
# Install SDL2 packages
.\vcpkg install sdl2:x64-windows
.\vcpkg install sdl2-ttf:x64-windows

# Integrate with Visual Studio
.\vcpkg integrate install
```

## Building the Project

### Option A: Visual Studio (Recommended for Development)

1. Open `Pong.sln` in Visual Studio
2. Select x64 platform
3. Build Solution (Ctrl+Shift+B)
4. Run (F5)

### Option B: CMake (Recommended for CI/CD)

```bash
# Navigate to project directory
cd Pong

# Run build script
build.bat

# Or manually:
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/dev/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build . --config Release
```

## Game Controls

### Local Game Mode
- **Player 1 (Left)**: W (up), S (down)
- **Player 2 (Right)**: ↑ (up), ↓ (down)
- **Quit**: ESC

### Network Game Mode
1. Choose Server (1) or Client (2)
2. Server waits for connection
3. Client enters server IP address
4. Use same controls as local mode

## Project Structure

```
Pong/
├── HEADER/           # Header files
│   ├── game.h       # Main game class
│   ├── constants.h  # Game constants
│   ├── network_manager.h  # Network handling
│   └── texture_manager.h  # Text rendering
├── SRC/             # Source files
│   ├── Main.cpp     # Entry point
│   ├── game.cpp     # Game implementation
│   ├── network_manager.cpp  # Network implementation
│   └── texture_manager.cpp  # Texture management
├── FONT/            # Font files
├── CMakeLists.txt   # CMake configuration
├── build.bat        # Build script
└── Pong.vcxproj     # Visual Studio project
```

## Architecture Improvements

- ✅ **Eliminated global variables** - All state encapsulated in Game class
- ✅ **Proper error handling** - Comprehensive error checking and recovery
- ✅ **Memory leak prevention** - RAII and proper cleanup
- ✅ **Network robustness** - Timeouts, validation, and error recovery
- ✅ **Performance optimization** - Texture caching and consistent frame timing
- ✅ **Modern C++ practices** - Smart pointers, RAII, and exception safety

## Troubleshooting

### Build Issues
- Ensure vcpkg is properly installed and integrated
- Verify SDL2 packages are installed: `.\vcpkg list`
- Check Visual Studio platform (x64 vs Win32)

### Runtime Issues
- Font file must be in the same directory as executable
- For network mode, check Windows Firewall settings
- Use `127.0.0.1` for local testing

### vcpkg Issues
- Reinstall packages: `.\vcpkg remove sdl2:x64-windows && .\vcpkg install sdl2:x64-windows`
- Check integration: `.\vcpkg integrate install`

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Submit a pull request

## License

This project is open source. Feel free to use and modify as needed.

## Acknowledgments

- SDL2 team for the excellent multimedia library
- Microsoft for vcpkg package manager
- The original Pong game for inspiration
