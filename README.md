# Space Shooter Extreme (TUI)

A fast, arcade-style terminal shooter written in C++. Defend the bottom row, fire upward, and survive endless waves as the game ramps up in speed.

## Features
- Smooth, non-blocking input for responsive movement
- Endless enemy waves with increasing difficulty
- Score tracking and limited lives
- Minimal dependencies (standard C++ + POSIX terminal control)

## How To Play
You control a ship at the bottom of the screen. Enemies fall from the top. Shoot them before they reach you.

## Controls
- `A` / `D`: Move left / right
- `SPACE`: Shoot
- `Q`: Quit

## Build
This is a single-file C++ game.

```bash
g++ -std=c++17 -O2 -Wall -Wextra -o game game.cpp
```

## Run
```bash
./game
```

## Gameplay Notes
- Enemies spawn faster over time.
- You start with 3 lives.
- Each enemy destroyed grants 10 points.

## Requirements
- A POSIX-compatible terminal (macOS, Linux)
- `g++` or another C++17 compiler

## Project Structure
- `game.cpp`: Game source
- `README.md`: Project documentation

## License
MIT (or update if you prefer a different license)
