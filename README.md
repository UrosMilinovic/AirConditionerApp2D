# Air Conditioner (2D Project, Computer Graphics)

A 2D graphics application that simulates an air conditioner. Written in C++ with OpenGL 3.3 (core profile, programmable pipeline), GLFW and GLEW.

## Controls
| Input | Effect |
|---|---|
| Left click on the lamp | Turns the air conditioner on / off |
| Up / Down arrow | Target temperature +1 / -1 (range -10 to 40 °C) |
| Space | Empties the basin |
| Escape | Exits the program |

## Code structure
- `Config.h` – all constants (speeds, number of water steps, drop parameters)
- `Game.h/.cpp` – state and logic (no OpenGL)
- `Render.h/.cpp` – shader setup, textures, scene drawing
- `Input.h/.cpp` – keyboard and mouse callbacks
- `Main.cpp` – window, main loop, frame limiter (75 FPS)
- `Util.h/.cpp` – helpers for loading shaders, textures and cursors
- `basic.vert`, `basic.frag` – shaders
- `res/` – images (icons, digit atlas, cursor, name tag)

## Running
Open the `.sln` file in Visual Studio (x64), restore the NuGet packages (GLEW and GLFW) and run. The program needs `basic.vert`, `basic.frag` and the `res/` folder in its working directory.
