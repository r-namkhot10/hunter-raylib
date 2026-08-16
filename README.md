# HUNTER

A real-time chase game written in C++ with raylib. Two hunters track you
across a world larger than the screen — survive 30 seconds to win.

This is a real-time remake of my earlier turn-based console game
([hunter-game](https://github.com/r-namkhot10/hunter-game)). The game logic
was already solved, so this port was about learning what changes when a game
stops waiting for input and starts running on a clock.

## Screenshot

<img width="1555" height="1017" alt="Screenshot 2026-08-17 010414" src="https://github.com/user-attachments/assets/71fbcf23-1fc2-4506-8b30-c032b410e1a9" />
![Gameplay](screenshot.png)

## Features

- **Two independent hunters** — each owns its own position and speed, and
  pursues the player continuously rather than turn by turn
- **Difficulty scaling** — every 10 seconds both hunters gain speed, up to a cap
- **Scrolling camera** — the world is 2000×2000 while the window is 1500×900,
  so the camera follows the player and reveals the world as you explore it
- **Frame-rate independent movement** — the game plays identically at 30, 60,
  or 144 FPS
- **Four game states** — title screen, playing, game over, and win, each with
  its own screen

## How to Run

Requires a C++17 compiler. The project uses
[raylib-quickstart](https://github.com/raylib-extras/raylib-quickstart), which
generates the project files for you.

**Visual Studio 2026**
1. Run `build-VisualStudio2026.bat`
2. Open the generated `.slnx` file
3. Press `Ctrl+F5`

**MinGW-W64**
1. Run `build-MinGW-W64.bat`
2. Run `make` in the project root
3. The executable is in `bin/`

## How to Play

| Key | Action |
|-----|--------|
| `Enter` | start the game |
| `W` | move up |
| `A` | move left |
| `S` | move down |
| `D` | move right |
| `Q` | quit |

You start at the centre of the world. Hunters spawn at opposite ends and head
straight for you. You are faster than they are — at first.

**Tip:** the walls are your enemy. Getting cornered costs you the distance you
spent the whole round building up.

## Technical Highlights

- **Custom `Vec2` math struct** with operator overloading (`+`, `-`, `*`),
  `length()`, `normalize()`, and `distanceTo()` — carried over from the console
  version and reused unchanged

- **`Hunter` class with encapsulated state** — position and speed are private,
  so each hunter owns its own data. This is what makes per-hunter speeds
  possible; the earlier version stored one shared speed in `main` and every
  enemy had to move at the same rate.

- **Delta-time movement** — every moving thing is measured in pixels per
  *second*, not per frame:
  ```cpp
  position = position + direction * speed * dt;
  ```

- **`Camera2D` following the player** — world coordinates are separated from
  screen coordinates, so the world can be far larger than the window. UI is
  drawn outside `BeginMode2D` so it stays fixed to the screen while the world
  scrolls underneath.

- **State machine** — instead of breaking out of the loop to end the game, the
  loop keeps running and only what gets drawn changes:
  ```cpp
  enum GameState { WAIT_FOR_START, PLAYING, GAME_OVER, WIN };
  ```

- **Fixing the diagonal speed bug** — playtesting revealed I could beat the game
  reliably by moving diagonally. Holding two keys added full speed to *both*
  axes, so diagonal movement was √2 ≈ 41% faster than moving straight — fast
  enough to outrun hunters that were supposed to catch me. The fix was to build
  an input vector and normalise it before applying speed:
  ```cpp
  // Before — diagonal movement was 424 px/s instead of 300
  if (IsKeyDown(KEY_D)) playerPos.x += speed * dt;
  if (IsKeyDown(KEY_S)) playerPos.y += speed * dt;

  // After — same speed in every direction
  Vec2 dir(0.0f, 0.0f);
  if (IsKeyDown(KEY_D)) dir.x += 1.0f;
  if (IsKeyDown(KEY_S)) dir.y += 1.0f;
  dir = dir.normalize();
  playerPos = playerPos + dir * speed * dt;
  ```
  The same `normalize()` I had written for the hunter AI turned out to be the
  fix. After patching it the game became unwinnable, so I rebalanced the hunter
  speeds from 300/240 down to 240/200.

- **Temporary on-screen notifications** — a boolean flag plus a countdown timer
  shows the "hunters are getting faster" message for three seconds and then
  clears itself, without blocking the game loop.

## About This Project

I built this while learning C++ and raylib. I solve problems myself first and
ask for hints when I get stuck, and I test every change by playing the game
rather than only reading the code — which is exactly how the diagonal speed bug
turned up.

The biggest thing I learned porting a turn-based game to real time: in a
console game the world waits for you, and in a real-time game it doesn't. Every
number that used to mean "per turn" has to be rethought as "per second."

## Credits

- [raylib](https://www.raylib.com/) by Ramon Santamaria
- [raylib-quickstart](https://github.com/raylib-extras/raylib-quickstart)
  template by Jeffery Myers (CC0)
