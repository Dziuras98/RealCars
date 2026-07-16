# Decision log

This log records project-level decisions that should not depend on chat history. New decisions should be appended with their date, rationale, evidence and consequences.

## 2026-07-16 — Physics-first project structure

**Decision:** Develop a standalone vehicle-dynamics laboratory before building the broader racing game.

**Rationale:** Subsystems must be measurable, repeatable and testable without graphics. Game content should not conceal or dictate physical behaviour.

**Consequences:** Initial work focuses on headless tools, telemetry, parameter files, tests and validation documentation.

## 2026-07-16 — Rendering-engine-independent simulation core

**Decision:** Keep the simulation core independent from Unreal Engine and other rendering-engine types.

**Rationale:** This supports headless tests, calibration, fixed-step execution and future engine integration without coupling physical code to presentation code.

**Consequences:** A future Unreal adapter will translate inputs, road contacts, poses and telemetry at the boundary.

## 2026-07-16 — Windows x64 only

**Decision:** Windows x64 is the only supported target platform.

**Rationale:** Explicit user requirement.

**Consequences:** CI builds Visual Studio Debug and Release configurations on Windows. Cross-platform support is not a project requirement.

## 2026-07-16 — C++20 and CMake

**Decision:** Use C++20 for the simulation core and CMake for standalone builds and tests.

**Rationale:** Required control over numerical execution and direct compatibility with the intended Unreal integration.

**Consequences:** Compiler warnings are enabled and can be elevated to errors. Public simulation interfaces remain ordinary C++ types.

## 2026-07-16 — SI units and right-handed coordinates

**Decision:** Use SI units and the documented right-handed vehicle coordinate system: `+X` forward, `+Y` left, `+Z` upward.

**Rationale:** Consistent units and signs are prerequisites for validation and subsystem integration.

**Consequences:** Any migration requires explicit user approval, documentation and regression-test updates.

## 2026-07-16 — Preserve a linear tire baseline

**Decision:** Retain the simple linear tire model after adding more advanced tire models.

**Rationale:** A simple model is useful for diagnosing sign errors, integration problems and unexpected nonlinear behaviour.

**Consequences:** The linear model is a diagnostic reference, not the target fidelity model.

## 2026-07-16 — Provisional brush/Fiala-style tire model

**Decision:** Use the current nonlinear brush/Fiala-style implementation as a development baseline only.

**Rationale:** It establishes nonlinear force curves, combined-slip constraints, load sensitivity, camber thrust and aligning-moment telemetry.

**Evidence status:** The initial implementation and default parameter values have not yet been tied to authoritative equation sources or measured tire data.

**Consequences:** It must not be called validated. Source registration, equation verification, calibration and golden-reference tests are required before relying on it as a physical representation of a real tire.

## 2026-07-16 — Evidence and uncertainty policy

**Decision:** Never guess material physical values. Search for sources first; ask the user whenever uncertainty remains or no adequate source exists.

**Rationale:** Explicit user instruction and the validation goals of the project.

**Consequences:** Provisional values require explicit user approval and must be visibly marked as unvalidated.

## 2026-07-16 — Pull-request and merge policy

**Decision:** Every logically separate change uses a new PR. After required tests pass, the PR is merged to the integration branch.

**Rationale:** Explicit user instruction.

**Branch note:** The repository integration branch is currently named `main`; the user's phrase `master` is treated as referring to this current integration branch unless the user explicitly requests a rename.

**Consequences:** Do not append unrelated work to an existing PR. Prefer squash merges unless commit preservation is technically justified.
