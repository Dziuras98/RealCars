# RealCars

RealCars is an experimental racing-game project built around a testable and independently validated vehicle-dynamics simulation.

The repository begins with a standalone C++ simulation core and a headless tire laboratory. Rendering-engine integration will be added only after the physical subsystems can be tested and calibrated without graphics.

## Current status

The initial bootstrap contains:

- a C++20 static simulation library,
- a deliberately simple linear tire model with a friction-circle limit,
- a command-line tire test rig producing CSV sweeps,
- unit tests and GitHub Actions CI,
- documented SI units and coordinate conventions.

The current tire model is scaffolding, not the target high-fidelity model. Its purpose is to establish stable interfaces, test infrastructure and telemetry before implementing a brush/Fiala model.

## Build

Requirements:

- CMake 3.24 or newer,
- a C++20 compiler.

```bash
cmake -S . -B build -DREALCARS_BUILD_TESTS=ON
cmake --build build --config Release
ctest --test-dir build --output-on-failure -C Release
```

## Run the tire rig

```bash
./build/realcar_tire_rig telemetry
```

On multi-configuration generators such as Visual Studio, the executable may be under `build/Release/`.

The program creates:

- `lateral_sweep.csv`,
- `longitudinal_sweep.csv`.

## Project rules

- SI units in the simulation core.
- Fixed and documented sign conventions.
- No rendering-engine types in the public simulation API.
- Physics changes require automated tests and telemetry.
- Plausible appearance is not accepted as validation.

See [`docs/physics-roadmap.md`](docs/physics-roadmap.md) and [`docs/coordinate-system.md`](docs/coordinate-system.md).
