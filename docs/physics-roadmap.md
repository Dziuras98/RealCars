# Physics roadmap

## Objective

Build a vehicle-dynamics core that is independent from the rendering engine, deterministic at a fixed time step, observable through telemetry, and validated against measured or published data. Windows is the only supported target platform.

## Milestone 0 — repository bootstrap

- C++20 simulation library
- CMake build
- compiler warnings
- headless tire test rig
- deterministic CSV output
- unit and CI tests
- documented units and coordinate conventions

## Milestone 1 — tire laboratory

Completed baseline:

- nonlinear brush/Fiala pure longitudinal and lateral force
- combined-slip friction ellipse
- load sensitivity
- camber thrust
- load-sensitive stiffness
- pneumatic-trail approximation and aligning torque
- parameter file separate from implementation
- pure and combined CSV sweeps
- automated numerical invariants and Windows Debug/Release CI

Still required:

- measured reference curves and parameter fitting
- Python plotting and comparison reports
- golden regression data with fixed tolerances
- relaxation length and transient slip states
- temperature, pressure, wear and speed sensitivity
- more rigorous combined-slip and aligning-torque formulations

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
