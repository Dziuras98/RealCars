# Physics roadmap

## Objective

Build a vehicle-dynamics core that is independent from the rendering engine, deterministic at a fixed time step, observable through telemetry, and validated against measured or published data.

## Milestone 0 — repository bootstrap

- C++20 simulation library
- CMake build
- compiler warnings
- headless tire test rig
- deterministic CSV output
- unit and CI tests
- documented units and coordinate conventions

## Milestone 1 — tire laboratory

- replace the bootstrap linear model with a physically motivated brush/Fiala model
- pure longitudinal and lateral slip
- combined slip
- load sensitivity
- camber thrust
- pneumatic trail and aligning torque
- parameter files separate from code
- Python plotting and parameter-fitting tools
- reference curves and regression tolerances

## Milestone 2 — planar vehicle

- rigid body with longitudinal, lateral, and yaw dynamics
- four tire instances
- steering geometry
- wheel rotational inertia
- simple powertrain and brakes
- steady-state cornering, step-steer, acceleration and braking tests

## Milestone 3 — full 6DoF vehicle

- roll, pitch and heave
- suspension kinematics
- springs, dampers, anti-roll bars and bump stops
- unsprung masses
- road contact queries
- fixed-step integrator and convergence tests

## Milestone 4 — engine integration

- Unreal Engine adapter around the standalone core
- interpolated rendering state
- collision handoff
- telemetry, sound and force-feedback outputs
- no Unreal types in the simulation library public API

## Validation rule

A feature is not considered complete when it merely looks plausible. It must have:

1. documented assumptions and equations,
2. automated tests,
3. reference data or an explicitly stated validation gap,
4. telemetry sufficient to diagnose its behavior,
5. a fixed regression tolerance.
