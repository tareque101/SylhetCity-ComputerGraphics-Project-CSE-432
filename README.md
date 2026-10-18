# Sylhet City Simulator (C++17, OpenGL 3.3, GLFW, GLAD, GLM)

## Build and run (Windows, MSYS2 MinGW + CMake, PowerShell)
```
cd D:\SylhetCity
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build build
.\build\SylhetCity.exe
```
- First configure downloads GLFW and GLM with git, so internet and `git` are needed once.
- GLAD (OpenGL 3.3 core) is already included in `external/glad`.
- Shaders are copied next to the exe automatically; `.\build\SylhetCity.exe` works from the project folder.

## Controls
| Key | Action |
|---|---|
| W A S D | move (top-down mode: pan map) |
| Mouse | look around |
| Q / E | down / up |
| Shift | 3x faster |
| Scroll | change speed (top-down mode: zoom) |
| M | toggle top-down map with place labels |
| N | day / night |
| T (hold) | collect toll when you stand within 45 m of a toll booth (Shah Jalal / East Surma Bridge) |
| TAB | release / capture mouse |
| ESC | quit |

The window title shows the nearest place name.

## Folder structure
```
SylhetCity/
  CMakeLists.txt
  external/glad/        GLAD loader (GL 3.3 core)
  shaders/              scene.vert, scene.frag (lighting, procedural windows, fog, night)
  src/
    main.cpp            window, input, render loop, day/night, top-down labels
    Camera.h            free-roam + top-down camera
    Shader.h            shader loader
    Batch.h/.cpp        box/cylinder/cone/sphere/text builder (one draw call per batch)
    Props.h/.cpp        trees, street lamps, signs, walls, flags
    RoadNet.h/.cpp      road grid, junction nodes, two Surma bridges (EDIT grid:: here)
    World.h/.cpp        ground, river, roads, zebra crossings, shop-house blocks
    Landmarks.h/.cpp    landmark table (EDIT positions/names) + points, bridges, market,
                        Court, DC office, City Corporation, BRTA, schools, hospital
    Campus.cpp          SIU (detailed), Leading, Metropolitan, Northeast universities
    Vehicles.h/.cpp     CNG, baby taxi, car, bike, rickshaw, bus + traffic simulation
    Kit.h               helper used by landmark builders
```

## Customise
- Move or rename a place: edit `makeLandmarks()` in `src/Landmarks.cpp`.
- Number of vehicles: `VehicleSystem::init` in `src/Vehicles.cpp` (the `specs` list).
- Road layout: `grid::X` and `grid::Z` in `src/RoadNet.h`.
- Add a new landmark: add a row in `makeLandmarks()` and a builder function; look at `buildCourt` as a template.

The layout is stylised (a grid of blocks), not a GPS-accurate map of Sylhet.


## v2 additions
- **Keane Bridge**: red steel arch bridge, pedestrians only (46 walkers, bollards at both ends, no road link).
- **Toll bridges**: Shah Jalal Bridge and East Surma Bridge. Every vehicle stops at the barrier. Walk (fly) to a booth and HOLD T:
  you collect the toll (Bike 10, Rickshaw 5, CNG/Baby taxi 20, Car 50, Bus 100 - simulation values, not real tariffs).
  If you are not at that booth, the booth staff collect automatically after ~3.5 s. Your total is shown in the window title,
  every booth has an income board.
- **New places**: Ali Amjad's Clock Tower, Sylhet Circuit House (both on the Surma bank near Keane Bridge), Karimullah Market
  (Bandar Bazar), Hasan Market (between Bandar Bazar and Zindabazar).
- **Surma river**: deep channel with embankments, ghat steps, promenade, boats, engine boats, steamers and launches in lanes.
  Edit lanes in `src/Boats.cpp` (LANES table), fares in `tollFare()` in `src/Vehicles.cpp`.
