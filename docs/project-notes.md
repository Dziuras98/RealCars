# RealCars project notes

Last consolidated: 2026-07-16

This document records the observations, decisions, plans, uncertainties and validation gaps established during the initial project setup. It is intended to prevent important context from existing only in chat history.

## Project objective

RealCars is planned as a Windows racing game whose main differentiator is highly accurate, observable and independently validated vehicle dynamics.

The project should be developed as a simulation research system first and a game second. Tracks, user interface, career systems, AI opponents, visual effects and content production should not drive the design of the physics core.

## Core architecture decision

The vehicle simulation is a standalone C++ library independent from the rendering engine.

The intended boundaries are:

- the simulation core owns vehicle state, forces, moments and physical integration,
- headless applications exercise individual subsystems and complete manoeuvres,
- data files hold vehicle, tire, surface and validation parameters,
- test code verifies invariants, reference curves and complete manoeuvres,
- a future Unreal Engine adapter will exchange inputs, road-contact information, poses and telemetry without introducing Unreal types into the core API.

This separation is intended to support repeatable tests, calibration runs, regression analysis, fixed-step simulation and possible future engine changes.

## Confirmed project decisions

- Supported target platform: Windows x64 only.
- Language for the simulation core: C++20.
- Build system: CMake.
- Current CI: Visual Studio builds on `windows-latest`, Debug and Release.
- Internal physical units: SI.
- Vehicle coordinate system: right-handed, `+X` forward, `+Y` left, `+Z` upward.
- Physics must be testable without graphics.
- Parameters should live outside implementation code where practical.
- The rendering/game engine will be integrated later, after subsystem validation.
- Unreal Engine is the current intended game-engine direction, but its version and adapter design are not yet fixed.

## Current repository state

The repository currently contains:

- a standalone static simulation library,
- a common steady-state tire-model interface,
- a simple linear tire model retained as a diagnostic baseline,
- a nonlinear brush/Fiala-style tire model,
- a text parameter loader,
- a generic development tire parameter file,
- a headless tire rig,
- lateral, longitudinal and combined-slip CSV sweeps,
- unit/invariant tests,
- Windows Debug and Release CI,
- coordinate-system, tire-model and roadmap documentation,
- a tire-data source register covering measured datasets, model fixtures, tools, licensing constraints and known public-data gaps.

The tire rig currently generates 4,415 numerical samples per run, excluding CSV headers.

The current source register is [`docs/research/tire-data-source-register.md`](research/tire-data-source-register.md). It is a research inventory rather than a calibration decision. No source listed there is automatically approved for importing numerical coefficients.

## Tire-model observations

The current nonlinear tire model provides:

- pure longitudinal force as a nonlinear function of slip ratio,
- pure lateral force as a nonlinear function of `tan(slip angle)`,
- saturation using a Fiala-style polynomial,
- combined-slip limiting with a configurable friction ellipse,
- load-sensitive friction coefficient,
- load-sensitive longitudinal, cornering and camber stiffness,
- linear camber thrust before combined-force limiting,
- a pneumatic-trail approximation,
- aligning moment that falls toward zero near full lateral saturation.

The model is steady-state. `road_speed_mps` is carried by the API but is not yet used by the force calculation.

The current combined-slip implementation calculates pure-axis forces first and then projects them onto an ellipse. This is useful as a controlled baseline but is not a complete combined-slip brush derivation.

The current aligning moment is a qualitative approximation based on force utilization rather than an integrated contact-patch pressure and shear distribution.

## Critical validation status

The included `reference_sport_tire.cfg` is not a representation of a specific production tire. Its values are generic development values.

The present model equations and default coefficients were implemented as a provisional engineering baseline before the repository sourcing rule was established. They must not be described as validated until the project records authoritative sources and, where possible, compares the output against measured data.

Required remediation:

1. identify primary or academically authoritative sources for each implemented equation,
2. record those sources in the repository,
3. verify that the implementation matches the stated equations and sign conventions,
4. replace generic coefficients with sourced data or explicit user-approved provisional values,
5. add golden reference curves and numerical tolerances,
6. state the valid operating range of each parameter set.

## Development principles

- Do not equate visual plausibility with correctness.
- Do not tune camera, force feedback or assists to conceal a physics error.
- Keep model complexity proportional to available validation data.
- Prefer a simpler model with traceable parameters over a complex model with invented coefficients.
- Preserve the linear model as a diagnostic reference even after more advanced models are added.
- Make every force, moment, state and intermediate utilization observable through telemetry when needed for diagnosis.
- Test convergence when selecting an integration rate; a higher frequency is not automatically more accurate.
- Keep rendering interpolation separate from fixed-step physical state updates.
- Avoid allocations and nondeterministic iteration in the real-time simulation loop.

## Planned tire-laboratory work

Recommended sequence:

1. Maintain and extend the tire equation and parameter-data source register.
2. Add Python plotting tools for all generated CSV sweeps.
3. Add golden-reference data and comparison reports with fixed tolerances.
4. Decide which real tire or published dataset will be the first calibration target.
5. Fit or derive stiffness, friction and load-sensitivity parameters from the selected source.
6. Replace or refine the provisional combined-slip formulation using sourced equations.
7. Replace or refine the aligning-moment approximation using sourced equations or data.
8. Add transient slip states and relaxation lengths.
9. Add low-speed handling to avoid singular slip calculations in the future wheel model.
10. Add rolling resistance and speed sensitivity when sourced data is available.
11. Add thermal, pressure and wear models only after suitable data and validation targets are identified.

## Planned vehicle-development sequence

After the tire laboratory has a sourced and regression-tested baseline:

1. Implement a planar rigid-body vehicle with longitudinal, lateral and yaw dynamics.
2. Add four tire instances, steering geometry and wheel rotational inertia.
3. Add simple brakes, driveline and differential behaviour.
4. Validate acceleration, braking, skidpad, steady-state cornering and step-steer manoeuvres.
5. Extend the chassis to heave, roll and pitch.
6. Add suspension kinematics, springs, dampers, anti-roll bars, bump stops and unsprung masses.
7. Add road-contact queries and fixed-step convergence tests.
8. Build the Unreal Engine adapter only after the standalone manoeuvre tests are stable.

## Planned standard manoeuvres

The full vehicle should eventually support automated tests for:

- static equilibrium,
- free rolling and coastdown,
- straight-line acceleration,
- straight-line braking,
- split-friction braking,
- skidpad,
- constant-radius increasing-speed cornering,
- step steer,
- slalom,
- lift-off oversteer,
- combined braking and cornering,
- slope holding and launch,
- single-bump response,
- kerb traversal.

Each manoeuvre should record vehicle pose and velocity, body accelerations, yaw rate, roll and pitch, wheel loads, slip ratios, slip angles, tire forces and moments, wheel speeds and relevant powertrain states.

## Open questions requiring user input or sourced evidence

Do not silently resolve these questions:

- Which real vehicle will be the first complete validation target?
- Which tire or public tire dataset will be the first calibration target?
- What level of agreement with measured data is considered acceptable?
- What steering wheel, pedals and force-feedback hardware should define the initial input/output target?
- Is deterministic replay required only on one Windows machine, across different Windows CPUs, or across network clients?
- Will multiplayer be required, and if so, what synchronization model is intended?
- Which Unreal Engine version should be the supported integration target?
- What is the intended maximum number of simultaneously simulated vehicles?
- Are damage, tire wear, wet surfaces and temperature part of the first playable scope or later research milestones?
- Which real-world data can the user legally obtain or measure for calibration?

## Candidate values that are not decisions

A fixed physics rate around 400 Hz was discussed as an initial test candidate. It is not a confirmed project constant. The final rate must be selected through stability and convergence tests for the implemented models and target hardware.

No tire coefficient, vehicle mass property, suspension geometry, aerodynamic coefficient or drivetrain parameter should be copied from this document into production data without a source or explicit user approval.
