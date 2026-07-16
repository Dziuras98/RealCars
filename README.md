# RealCars

RealCars is an experimental Windows racing-game project built around a testable and independently validated vehicle-dynamics simulation.

The repository starts with a standalone C++ simulation core and a headless tire laboratory. Rendering-engine integration will be added only after the physical subsystems can be tested and calibrated without graphics.

## Current status

The current baseline contains:

- a C++20 static simulation library,
- a nonlinear brush/Fiala tire model,
- pure longitudinal, pure lateral and combined slip,
- load-sensitive friction and stiffness,
- camber thrust and a pneumatic-trail approximation,
- a file-based tire parameter set,
- a command-line tire test rig producing CSV sweeps,
- automated tests and Windows-only GitHub Actions CI,
- documented SI units, coordinate conventions and model equations.

The included reference tire parameters are generic development values. They are not calibrated to a particular production tire.

## Supported platform

Windows x64 is the only supported target. CI builds and tests Visual Studio configurations on `windows-latest` in Debug and Release.

## Build on Windows

Requirements:

- Visual Studio 2022 with Desktop development with C++,
- CMake 3.24 or newer.

```powershell
cmake -S . -B build -A x64 -DREALCARS_BUILD_TESTS=ON
cmake --build build --config Release --parallel
ctest --test-dir build --output-on-failure -C Release
```

## Run the tire rig

```powershell
.\build\Release\realcar_tire_rig.exe telemetry data\tires\reference_sport_tire.cfg
```

The program creates:

- `lateral_sweep.csv`,
- `longitudinal_sweep.csv`,
- `combined_sweep.csv`.

## Project rules

- SI units in the simulation core.
- Fixed and documented sign conventions.
- No rendering-engine types in the public simulation API.
- Physics changes require automated tests and telemetry.
- Plausible appearance is not accepted as validation.
- Windows x64 is the supported runtime and CI target.

See [`docs/tire-model.md`](docs/tire-model.md), [`docs/physics-roadmap.md`](docs/physics-roadmap.md) and [`docs/coordinate-system.md`](docs/coordinate-system.md).
