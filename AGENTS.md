# Instructions for AI-assisted changes

This file is a repository-level instruction for ChatGPT and other AI coding agents working on RealCars. Read it before planning or modifying the repository.

## User directive — verbatim

> Zawsze pytaj się użytkownika o niepewności. Nigdy nie zgaduj wartości - zawsze szukaj źródeł. W razie braku źródeł zapytaj się użytkownika o potwierdzenie. Każdą zmianę twórz w nowym PR. Utworzony PR zawsze merguj do mastera po przejściu testów.

## Operational interpretation

1. Ask the user about every material uncertainty before implementing a choice that could affect physics, architecture, data, scope or validation.
2. Never invent physical constants, vehicle parameters, tire coefficients, dimensions, masses, inertias, aerodynamic values, performance figures, tolerances or target behaviour.
3. Search for and cite reliable sources before entering factual or numerical values. Prefer primary sources: manufacturer documentation, standards, peer-reviewed papers, textbooks with traceable equations, homologation data and measured datasets.
4. When no adequate source exists, state the gap and ask the user to explicitly approve a provisional assumption. Mark every approved provisional value in code and documentation as unvalidated.
5. Create every logically separate change on a new branch and in a new pull request. Do not append unrelated work to an existing PR.
6. Run all applicable tests before merging. A PR must not be merged when tests fail or when required validation has not been performed.
7. Merge each completed PR after its required checks pass. Prefer squash merge unless preserving individual commits has a clear technical purpose.
8. Record important observations, decisions, assumptions, source links, validation gaps and future plans in the repository rather than relying on chat history.

## Branch-name note

The user used the word `master` in the directive. The repository's current default and integration branch is named `main`. Until the user explicitly requests a branch rename, treat `main` as the integration branch referred to by that directive. Do not silently create or switch to a different integration branch.

## Physics-specific requirements

- Plausible-looking behaviour is not validation.
- Every model must document its equations, sign conventions, units, assumptions and valid operating range.
- Every numerical parameter must have a source or an explicit user-approved provisional status.
- Preserve SI units inside the standalone simulation core.
- Preserve the documented right-handed coordinate system unless the user approves a migration.
- Add automated invariants, regression tests and diagnostic telemetry for physics changes.
- Keep rendering-engine types out of the public simulation-core API.
- Do not tune one subsystem to hide an error in another subsystem.

## Required PR description

Each PR description must state:

- what changed,
- why it changed,
- sources used for equations and values,
- assumptions and unresolved uncertainties,
- validation performed,
- test results,
- known limitations,
- next recommended step.
