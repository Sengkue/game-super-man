# Superman vs Ghost Game

A fast-paced action game built with C++ and SFML where you play as Superman fighting against hordes of ghosts in a dynamic cityscape environment.

## Features

- **Dynamic Gameplay**: Fly around as Superman with smooth movement controls
- **Combat System**: Laser vision attacks and super punch abilities
- **Enemy AI**: Three types of ghosts with different behaviors and difficulty levels
- **Visual Effects**: Particle systems for explosions, attacks, and environmental effects
- **Progressive Difficulty**: Multiple levels with increasing ghost counts and challenges
- **Audio**: Sound effects and background music support
- **Cross-Platform**: Runs on Windows, Linux, and macOS

## Game Controls

| Control | Action |
|---------|--------|
| `W/A/S/D` or `Arrow Keys` | Superman movement/flying |
| `Spacebar` | Laser vision attack (ranged) |
| `Left Mouse Click` | Super punch attack (close combat) |
| `ESC` | Pause/Resume game |
| `Enter` | Start game / Restart after game over |
| `Alt+F4` | Exit game |

## Game Mechanics

- **Health System**: Superman starts with 100 health and regenerates 1 HP per second
- **Ghost Types**:
  - **Regular Ghost**: 20 HP, slow movement, 10 damage
  - **Fast Ghost**: 15 HP, quick movement, 15 damage  
  - **Boss Ghost**: 50 HP, special attacks, 20 damage (appears every 3rd level)
- **Scoring**: Regular (10 pts), Fast (20 pts), Boss (50 pts)
- **Level Progression**: Complete levels by defeating all ghosts

## Prerequisites

### Windows
- **Visual Studio 2019 or later** (with C++ development tools)
- **CMake 3.16 or later**
- **SFML 2.5 or later**

### Linux (Ubuntu/Debian)
```bash
sudo apt update
sudo apt install build-essential cmake
sudo apt install libsfml-dev
```

### Linux (Fedora/CentOS)
```bash
sudo dnf install gcc-c++ cmake
sudo dnf install SFML-devel
```

### macOS
```bash
# Install Homebrew if not already installed
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install cmake sfml
```

## Building the Game

### Option 1: Using CMake (Recommended)

1. **Clone or download the project**
```bash
git clone <repository-url>
cd superman-vs-ghost
```

2. **Create build directory**
```bash
mkdir build
cd build
```

3. **Configure with CMake**
```bash
# Debug build
cmake -DCMAKE_BUILD_TYPE=Debug ..

# Release build (recommended for playing)
cmake -DCMAKE_BUILD_TYPE=Release ..
```

4. **Build the project**
```bash
# Windows (Visual Studio)
cmake --build . --config Release

# Linux/macOS
make -j$(nproc)
```

5. **Run the game**
```bash
# Windows
./Release/SupermanVsGhost.exe

# Linux/macOS
./SupermanVsGhost
```

### Option 2: Manual Compilation

#### Windows (MinGW)
```bash
g++ -std=c++17 -O2 -I"path/to/SFML/include" -L"path/to/SFML/lib" *.cpp -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -o SupermanVsGhost.exe
```

#### Linux
```bash
g++ -std=c++17 -O2 *.cpp -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -o SupermanVsGhost
```

#### macOS
```bash
g++ -std=c++17 -O2 *.cpp -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -o SupermanVsGhost
```

### Option 3: Visual Studio (Windows)

1. Open Visual Studio
2. Create a new C++ Console Application project
3. Add all `.cpp` and `.h` files to the project
4. Configure SFML:
   - Project Properties → C/C++ → Additional Include Directories: `path/to/SFML/include`
   - Project Properties → Linker → Additional Library Directories: `path/to/SFML/lib`
   - Project Properties → Linker → Additional Dependencies: Add:
     - `sfml-graphics.lib`
     - `sfml-window.lib` 
     - `sfml-system.lib`
     - `sfml-audio.lib`
5. Build and run

## Project Structure

```
superman-vs-ghost/
├── main.cpp              # Entry point
├── Game.h/.cpp           # Main game class and loop
├── Superman.h/.cpp       # Player character implementation
├── Ghost.h/.cpp          # Enemy AI and behaviors
├── Projectile.h/.cpp     # Laser beams and projectiles
├── ParticleSystem.h/.cpp # Visual effects system
├── ResourceManager.h/.cpp # Asset loading and management
├── CMakeLists.txt        # Build configuration
├── README.md            # This file
└── assets/              # Game assets (optional)
    ├── textures/        # Image files (.png)
    ├── sounds/          # Audio files (.wav, .ogg)
    └── fonts/           # Font files (.ttf)
```

## Asset Requirements

The game can run without external assets by using procedurally generated graphics and silent audio. For enhanced experience, you can add:

### Textures (32x32 to 128x128 PNG files)
- `assets/textures/superman.png` - Superman sprite
- `assets/textures/ghost.png` - Ghost sprite  
- `assets/textures/background.png` - City background
- `assets/textures/effects.png` - Particle effects

### Audio (WAV or OGG files)
- `assets/sounds/laser.wav` - Laser attack sound
- `assets/sounds/punch.wav` - Punch attack sound
- `assets/sounds/ghost_death.wav` - Ghost defeat sound
- `assets/sounds/background_music.ogg` - Background music

### Fonts (TTF files)
- `assets/fonts/arial.ttf` - UI text font

## Performance Requirements

- **Minimum**: 2GB RAM, DirectX 9 compatible graphics
- **Recommended**: 4GB RAM, dedicated graphics card
- **Target Performance**: 60 FPS with 20+ entities on screen

## Troubleshooting

### Common Issues

**"SFML not found" during compilation**
- Install SFML development libraries
- Verify SFML path in CMake or compiler flags

**Game window doesn't open**
- Check graphics drivers are up to date
- Verify SFML runtime libraries are accessible

**No sound/audio**
- Install OpenAL runtime libraries
- Check audio device drivers

**Poor performance**
- Run in Release mode instead of Debug
- Close other applications
- Update graphics drivers

### Building Issues

**Windows: "Cannot find sfml-graphics-2.dll"**
```bash
# Copy SFML DLLs to executable directory
copy "SFML_PATH/bin/*.dll" "build/Release/"
```

**Linux: "error while loading shared libraries"**
```bash
# Install SFML runtime libraries
sudo apt install libsfml2.5

# Or add to LD_LIBRARY_PATH
export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
```

**macOS: Framework issues**
```bash
# Reinstall SFML via Homebrew
brew uninstall sfml
brew install sfml
```

## Development

### Code Style
- C++17 standard
- Object-oriented design with proper encapsulation
- RAII and smart pointers for memory management
- Frame-rate independent movement using delta time

### Adding Features
1. Implement new classes in separate `.h/.cpp` files
2. Add to CMakeLists.txt `SOURCES` and `HEADERS`
3. Follow existing patterns for game state management
4. Use ResourceManager for loading assets

### Debugging
- Build in Debug mode: `cmake -DCMAKE_BUILD_TYPE=Debug ..`
- Use debugger breakpoints in game loop
- Enable verbose logging in ResourceManager

## License

This project is open source. Feel free to modify and distribute according to your needs.

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Submit a pull request

## Credits

- **SFML Library**: Simple and Fast Multimedia Library
- **Game Engine**: Custom C++ implementation
- **Graphics**: Procedural generation and custom sprites

## Version History

- **v1.0.0**: Initial release with core gameplay features
  - Superman character with movement and abilities
  - Three ghost types with AI behaviors
  - Particle effects and visual polish
  - Cross-platform build system
  - Complete game loop and state management

---

**Enjoy playing Superman vs Ghost!** 🦸‍♂️👻

For support or questions, please create an issue in the project repository."# game-super-man" 
