# Tire characterization and validation harness

## Purpose

`realcar_tire_rig` is the repeatable, headless characterization entry point for the tire subsystem. It generates diagnostic curves for both the nonlinear brush/Fiala-style model and the retained linear reference model without changing either model's force equations.

The harness is infrastructure for later calibration and validation. Its output is **not** measured reference data and does not validate the generic development tire.

## Run

```powershell
.\build\Release\realcar_tire_rig.exe telemetry data\tires\reference_sport_tire.cfg
```

The second argument is optional. When omitted, the brush model uses its built-in defaults and records that fact in `run_metadata.csv`.

## Outputs

The original brush-model outputs remain available for compatibility:

- `lateral_sweep.csv`
- `longitudinal_sweep.csv`
- `combined_sweep.csv`

The characterization harness additionally creates:

- `characterization_samples.csv` — normalized long-form samples for both models,
- `characterization_summary.csv` — initial gradients, sampled peaks, zero-force residuals and maximum combined utilization,
- `run_metadata.csv` — schema version, sweep policy and the exact brush and linear parameters used.

`characterization_samples.csv` contains:

- model and sweep identifiers,
- normal load, slip ratio, slip angle, camber angle and road speed,
- `Fx`, `Fy` and `Mz`,
- the force limits used to normalize each model,
- combined-force utilization.

## Diagnostic sweep policy

PR 1 intentionally introduces no new tire coefficients or claimed operating limits.

- Normal loads and pure-slip ranges retain the original tire-rig settings.
- The combined-slip grid retains the original tire-rig settings.
- The camber sweep uses the existing `±3 degree` automated-test extent.
- Road speed remains `20 m/s`; both current models are steady-state and do not presently use it in force calculation.
- The central-difference step for reported initial gradients matches the existing small-slip brush-model test.

These settings define a reproducible diagnostic grid only. They must be replaced or supplemented by test-condition grids from selected measured datasets before calibration claims are made.

## Plotting

The plotting helper requires Python 3 and Matplotlib:

```powershell
py -m pip install matplotlib
py tools\plot_tire_characterization.py telemetry\characterization_samples.csv
```

It writes separate figures for longitudinal force, lateral force, aligning moment, camber thrust and combined-force utilization.

## Automated contracts

`realcar_tire_characterization_contract_tests` applies common invariants to both tire models:

- zero force at zero slip and zero camber,
- zero force for non-positive normal load,
- documented signs and odd symmetry for pure-slip response,
- finite output over the diagnostic grid,
- combined force inside each model's configured friction limit.

`realcar_tire_rig_output_validation` executes the rig and verifies that all expected files, schemas, models, sweeps, parameter metadata and sample count are present.

## Known limitations

- No measured curves are imported yet.
- No error metric against external reference data is reported yet.
- No golden numerical baseline with approved regression tolerances exists yet.
- Peak coordinates are grid-sampled values, not continuously optimized extrema.
- The brush and linear force limits are model-specific normalization references; equal utilization does not imply equal physical fidelity.

## Next validation step

Select a legally usable measured force-and-moment dataset with documented load, pressure, speed, camber and surface conditions. Add an importer and a comparison report without tuning the model until the dataset's coordinate conventions and test metadata are reconciled.
