# Tire model-to-measurement comparison

## Purpose

This comparison evaluates the current uncalibrated RealCars tire models at measured operating points. It is intended to expose structural and parameter errors before model tuning or additional tire states are introduced.

The comparison does **not** claim that `reference_sport_tire.cfg` represents the measured tire. No coefficient is fitted in this workflow.

## Measurement source

The workflow uses the public KIT dataset:

- title: *Tire's force transmission characteristic on dry asphalt measured at the inner drum test bench of KIT*,
- institution: Karlsruhe Institute of Technology,
- DOI: `10.35097/p0rr2jc5wmf0drf8`,
- license: CC BY-NC-SA 4.0,
- source archive checksum: MD5 `8fe6695d591f944d0abcd97af3039c8b`,
- extracted payload checksum: SHA-256 `5ffe1c57663f94a85c42498dcd83d875a1263858a8bd6e67970039b06ffb1353`.

The downloader verifies both checksums before extraction. The measured files are downloaded during validation and are not committed to RealCars.

The dataset contains processed characteristic curves for a `205/55 R17` summer tire on dry asphalt. The included test report identifies nominal vertical loads of `1250 N`, `4000 N` and `6000 N`, inflation pressure `2.4 bar`, pure longitudinal and lateral tests, and combined-slip tests at nominal slip angles of `1 degree` and `5 degrees`.

These measured test conditions are dataset facts. They are not new RealCars tire parameters.

## Selected coordinate representation

The dataset provides TYDEX characteristic files in H and W coordinate systems. RealCars imports the **H-system** files because their wheel-centered axes map directly to the current tire-model interface.

The parser uses these TYDEX channels:

| TYDEX channel | RealCars quantity | Conversion |
|---|---|---|
| `FZH` | `normal_load_n` | direct, N |
| `LONGSLIP` | `slip_ratio` | percent divided by 100 |
| `SLIPANGL` | `slip_angle_rad` | degrees to radians |
| `INCLANGL` | `camber_angle_rad` | degrees to radians |
| `TRAJVELW` | `road_speed_mps` | direct dataset value |
| `FXH` | measured longitudinal force | direct, N |
| `FYH` | measured lateral force | direct, N |
| `MZH` | measured aligning moment | direct, N m |

Inspection of the source curves confirmed that the dataset and RealCars share the relevant force and slip signs:

- positive longitudinal slip produces positive `Fx`,
- positive slip angle produces negative `Fy`,
- positive slip angle produces positive restoring `Mz`.

No sign inversion, curve mirroring or offset removal is applied.

## Data coverage

The comparator imports 843 H-system samples:

- 365 pure-longitudinal points,
- 264 pure-lateral points,
- 214 combined-slip points.

The measured values are evaluated exactly at their recorded load, slip ratio, slip angle and camber. No interpolation or resampling is performed.

## Compared models

Two models are evaluated:

1. `BrushTireModel` loaded from `data/tires/reference_sport_tire.cfg`.
2. `LinearTireModel` with its existing built-in diagnostic parameters.

The linear model is retained as a control. A more complex model is not assumed to be better merely because it is nonlinear.

## Reported metrics

For each model, sweep, force or moment component, and nominal load, the comparator reports:

- sample count,
- root mean square error,
- mean absolute error,
- signed mean error or bias,
- maximum absolute error,
- measured absolute peak,
- RMSE normalized by the measured absolute peak,
- coefficient of determination `R squared`,
- Pearson correlation.

Aggregate rows combine all available loads within a sweep. Per-load rows remain available in `model_measurement_summary.csv`.

Normalized error and `R squared` require caution when the measured component is nearly zero. In particular, the combined-slip `Mz` curves in this dataset have a very small measured range, so percentage normalization and `R squared` can become numerically large without representing an equally large absolute moment error.

## Generated outputs

The executable produces:

- `model_measurement_samples.csv` — every measured state, force, prediction and residual,
- `model_measurement_summary.csv` — aggregate and per-load metrics,
- `model_measurement_report.md` — a concise generated report,
- parity plots created by `tools/plot_tire_measurement_comparison.py`.

The dedicated GitHub Actions workflow uploads these files as the `tire-model-measurement-comparison` artifact.

## Validation boundaries

This workflow establishes reproducible comparison infrastructure, not a calibrated tire model.

Known limitations:

- the generic RealCars baseline and measured KIT tire are different tire definitions,
- the current steady-state model ignores pressure, temperature, detailed speed dependence and transient relaxation,
- no uncertainty bands or repeatability estimates are available in the imported characteristic files,
- the comparator does not weight samples by measurement uncertainty,
- no parameter optimization is performed,
- no acceptance thresholds are declared yet,
- the dataset license restricts reuse to its stated terms and is not automatically suitable for redistribution inside a commercial game repository.

## Interpretation rule

The comparison should be used to identify where the model shape is inadequate and which parameters are observable. It must not be used to tune one coefficient until the model family, measurement conditions and intended target tire have been selected explicitly.

The next model-development decision should be based on the measured residual structure:

- pure-longitudinal errors inform friction and longitudinal-stiffness behavior,
- pure-lateral errors inform cornering stiffness, saturation and load sensitivity,
- aligning-moment errors test the current pneumatic-trail approximation,
- combined-slip errors test whether post-hoc friction-ellipse projection is structurally adequate.
