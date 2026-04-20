# ProjectDX - DirectX 3D Football Pitch

A **C++ / DirectX 11** real-time 3D scene built from scratch, rendering a football pitch with custom geometry, lighting, textures, per-object HLSL shaders, a skybox, and XAudio2 audio - no game engine, no abstractions.

> University/personal project exploring low-level graphics programming with the DirectX API.

---

## What's Implemented

- **Custom render pipeline** - device and swap chain management via `DeviceResources`
- **Per-object HLSL shaders** - separate vertex/pixel shader pairs for grass, ground, goal posts, pitch markings (boxes and centre circle), skybox, and lit geometry
- **Phong lighting** - directional light with diffuse/specular, managed via `Light` class
- **3D model loading** - custom `modelclass` for loading and rendering `.obj` geometry
- **Camera system** - free-look camera with keyboard/mouse input
- **Skybox** - cube-mapped environment surrounding the scene
- **Audio** - XAudio2/XACT wave bank (`.xwb`) for in-scene sound
- **HUD / text** - SpriteBatch/SpriteFont rendering (SegoeUI 18pt)

---

## Shaders

| Shader | Purpose |
|--------|---------|
| `grass_vs/ps` | Football pitch surface |
| `ground_vs/ps` | Ground plane |
| `goal_post_vs/ps` | Goal post geometry |
| `pitch_boxes_ps` | Penalty/goal box markings (procedural) |
| `pitch_circle_vs/ps` | Centre circle marking |
| `skybox_vs/ps` | Cube-mapped skybox |
| `light_vs/ps` | Phong-lit geometry |
| `basic_vs/ps` | Unlit geometry pass |

---

## Tech

- **Language:** C++
- **Graphics API:** DirectX 11
- **Shading language:** HLSL
- **Audio:** XAudio2 / XACT (`.xwb` wave bank)
- **Toolkit:** DirectX Tool Kit (DeviceResources, StepTimer, SpriteBatch)
- **Build:** Visual Studio (`Mazetrix_Solution.sln`)

---

## Building

1. Open `Mazetrix_Solution.sln` in **Visual Studio 2019+**
2. Set build configuration to `Debug x64` or `Release x64`
3. Build and run - no additional SDK setup required beyond the Windows SDK

> Requires Windows SDK with DirectX headers (included with Visual Studio by default).

---

## Project Structure
```
Camera.cpp/h                     # Free-look camera
Light.cpp/h                      # Directional light
Shader.cpp/h                     # Shader compilation and binding
modelclass.cpp/h                 # OBJ model loading and rendering
DeviceResources.cpp/h            # D3D device, swap chain, render targets
Game.cpp/h                       # Main game loop and scene setup
Input.cpp/h                      # Keyboard/mouse input
Main.cpp                         # Win32 entry point
*.hlsl                           # HLSL vertex and pixel shaders
Audio/                           # Audio assets
Models/                          # 3D model assets
Textures/                        # Texture assets
```

---

## License

MIT
