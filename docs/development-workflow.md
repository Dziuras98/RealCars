# Development and validation workflow

## Change isolation

Every logically separate change must use a new branch and a new pull request based on the current integration branch.

Current integration branch: `main`.

Do not continue unrelated work in an existing pull request. If a new request arrives while a PR is open, finish or close the existing PR first, then create a fresh branch from the updated integration branch.

Recommended branch pattern:

```text
agent/<short-change-description>
```

## Uncertainty handling

Before implementing a material choice:

1. identify uncertain requirements, values, equations and constraints,
2. search for reliable sources where factual evidence can resolve them,
3. present unresolved alternatives to the user,
4. ask the user for a decision or explicit approval of a provisional assumption,
5. record the decision, source or provisional status in the repository.

Never silently select a physical value because it appears plausible.

## Source hierarchy

Prefer sources in this order:

1. applicable standards and regulations,
2. manufacturer engineering documentation and homologation data,
3. peer-reviewed papers and traceable experimental datasets,
4. established technical textbooks and reference works,
5. direct measurements with documented equipment and procedure,
6. secondary technical sources only when primary sources are unavailable.

Record enough bibliographic information or links to locate the source again. State which equation, parameter or validation target each source supports.

Community posts, simulator settings and unsourced vehicle databases may be used to locate leads, but not as final evidence without explicit user approval.

## Provisional values

A provisional value is allowed only when:

- no adequate source has been found,
- the user explicitly approves its temporary use,
- the code or data labels it as provisional,
- documentation states why it was selected,
- tests do not imply that the value has been validated,
- a follow-up task records what evidence is still required.

## Required implementation checks

Depending on the change, perform all applicable checks:

- build Windows x64 Debug,
- build Windows x64 Release,
- run unit and invariant tests,
- run smoke tests for command-line tools,
- generate diagnostic telemetry,
- compare against golden data,
- inspect numerical stability and convergence,
- verify units and sign conventions,
- verify parameter-file validation,
- document equations and valid operating range.

## Pull-request lifecycle

1. Create a new branch from `main`.
2. Implement only the requested logical change.
3. Add or update tests and documentation.
4. Open a pull request targeting `main`.
5. Include sources, assumptions, uncertainties, validation and known limitations in the PR description.
6. Wait for all required checks to finish.
7. Fix failures in the same PR when they belong to its scope.
8. Merge only after required tests pass.
9. Prefer squash merge for a concise integration history.
10. Record any remaining work in repository documentation or a follow-up issue.

## Merge rule

A passing build alone is insufficient for physics work. A physics PR is ready to merge only when its relevant equations, units, signs, assumptions, parameters and validation status are documented.

If a test cannot be performed, do not represent it as passed. Record the missing validation and ask the user whether the incomplete evidence is acceptable before merging.
