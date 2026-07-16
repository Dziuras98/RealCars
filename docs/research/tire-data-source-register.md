# Tire data source register

Last reviewed: 2026-07-16

## Purpose

This register records candidate sources of measured tire data, fitted model parameters, road-surface data, test standards, reference implementations and publication-only curves that may support RealCars tire-model development and validation.

A source being listed here does **not** make its values validated or approved for use in a production parameter set. Before importing numerical values, the project must verify:

- provenance and the exact tested tire,
- test method, units, coordinate system and slip definitions,
- operating conditions and valid range,
- licensing and redistribution rights,
- whether values are measured, fitted, synthetic, anonymized or merely illustrative,
- whether the source is suitable for a road-car tire rather than only for qualitative comparison.

The current `reference_sport_tire.cfg` remains a generic, unvalidated development fixture. None of the sources below has yet been selected as the first RealCars calibration target.

## Evaluation labels

### Evidence class

- **A — raw measured data:** time histories or sweep points with documented test conditions.
- **B — fitted parameter set:** coefficients for a defined model, ideally tied to a documented test campaign.
- **C — tool or reference implementation:** useful for import, fitting, comparison or validation workflow, but not itself measurement evidence.
- **D — publication curves or reported results:** useful for qualitative validation or digitization when legally permitted; raw data are not known to be available.
- **E — catalog, regulation or coarse metadata:** useful for bounds, categories and operating guidance, not for calibrating a complete force-and-moment model.

### Reuse status

- **Open/reference-safe:** source is public and appears usable for research/reference; repository license or document terms still govern code and redistribution.
- **License review required:** terms are missing, unclear or may conflict with the intended project distribution.
- **Restricted:** access or use is contractually limited.
- **Do not import:** provenance or redistribution rights are incompatible or unresolved.
- **Unverified lead:** existence, contents or access could not be confirmed during this review.

## Search vocabulary used

The search was repeated with American and British spelling and with terms used by laboratories, model developers and road researchers:

- `tire data`, `tyre data`, `tire dataset`, `tyre dataset`,
- `tire characterization`, `tyre characterisation`,
- `force and moment tire testing`, `F&M tire data`,
- `Flat-Trac`, `flat belt tire test`, `drum tire test`, `tire test rig`,
- `cornering force`, `cornering stiffness`, `lateral force`, `camber thrust`,
- `slip angle sweep`, `slip ratio sweep`, `combined slip`,
- `aligning torque`, `self-aligning moment`, `pneumatic trail`,
- `relaxation length`, `transient tire response`,
- `contact patch`, `tire footprint`, `contact pressure distribution`,
- `rolling resistance`, `coastdown tire data`,
- `tire wear`, `tread temperature`, `thermal tire model`,
- `intelligent tire`, `smart tire`, `inner-liner accelerometer`,
- `wet grip`, `road friction`, `skid resistance`, `hydroplaning`, `aquaplaning`,
- `TYDEX`, `.tdx`, `.tir`, `MF-Tyre`, `Magic Formula`, `Pacejka`, `TMeasy`, `Fiala`,
- `road profile`, `road roughness`, `OpenCRG`.

## Priority shortlist

1. **NASA public reports** — the strongest immediately accessible experimental source found for wet/flooded traction, hydroplaning trends, wear and temperature. They concern aircraft tires, so they are validation references for mechanisms and trends, not direct passenger-car calibration data.
2. **MathWorks/GCAPS example tire data** — a well-described passenger-car force-and-moment test matrix in TYDEX form. Access and redistribution depend on the MathWorks product/license and must be checked before files are imported.
3. **TUM FTM parameter-estimation toolbox** — an open workflow for identifying a simplified Magic Formula model from real driving data, including preprocessing and uncertainty estimation.
4. **TYDEX 1.3 specification** — a public interchange format suitable as a future RealCars measured-data import target.
5. **ASAM OpenCRG** — an open road-surface format and APIs for elevation and other gridded properties, including possible friction fields.
6. **Modelica sample `.tir` and Project Chrono examples** — useful parser and regression fixtures only; they must not be represented as calibrated production-tire data.
7. **FSAE Tire Test Consortium** — excellent measured data but contractually restricted, non-redistributable and unsuitable for commercial use under the published membership terms unless separate permission is obtained.

## Measured force-and-moment data

### MathWorks Extended Tire Features example data / GCAPS

- **Evidence class:** A, with model-fitting examples around it.
- **Source:** [MathWorks — Example Tire Data](https://www.mathworks.com/help/vdynblks/ug/example-tire-data.html)
- **Tire:** midsize 235/45R18 94V passenger-car tire.
- **Format:** TYDEX 1.3 `.tdx` files.
- **Coverage:** lateral, longitudinal and combined-slip sweeps.
- **Documented conditions:**
  - vertical loads: 2150, 4300, 5160 and 6450 N,
  - camber angles: -6, 0 and +6 degrees,
  - road speed: 27.78 m/s,
  - inflation pressure: 262 kPa,
  - combined-slip lateral sweep examples at slip angles -8, -5, -2 and +2 degrees.
- **Potential variables:** longitudinal force, lateral force, aligning moment and standard test channels available through TYDEX records.
- **RealCars use:** candidate passenger-tire reference matrix; strong example of how the tire rig and future fitter should organize tests.
- **Reuse status:** **license review required**. The example is associated with MathWorks Vehicle Dynamics Blockset / Extended Tire Features. Do not commit downloaded files until product terms and redistribution rights are checked.
- **Caveat:** availability through documentation does not imply permission to redistribute the underlying measurements.

### Formula SAE Tire Test Consortium (FSAE TTC)

- **Evidence class:** A.
- **Source:** [Milliken Research Associates — FSAE TTC](https://www.millikenresearch.com/fsaettc.html)
- **Test facility:** Calspan flat-belt force-and-moment machine; the published description identifies a 120-grit belt and nominal 25 mph testing.
- **Published channels include:**
  - elapsed time and road velocity,
  - slip angle, inclination/camber and slip ratio,
  - normal load,
  - lateral and longitudinal force,
  - aligning torque and overturning moment,
  - loaded/effective radius and wheel speed,
  - inflation pressure,
  - three tread temperatures,
  - ambient and road temperature.
- **Formats:** ASCII and MATLAB; SI and US customary units.
- **Sampling:** published as 10–100 Hz depending on test.
- **RealCars use:** technically excellent for fitting steady-state and potentially transient models, load sensitivity, camber effects, combined slip and thermal correlations.
- **Reuse status:** **restricted**.
- **Published restrictions:** membership data are licensed to participating schools; the site states a USD 500 membership fee, prohibits commercial use and prohibits redistribution or posting the data online.
- **Decision:** do not add any TTC data to this repository without a documented license that explicitly covers the intended use and distribution.

### Unofficial GitHub copy described as FSAE TTC Round 6

- **Evidence class:** claimed A; provenance is not acceptable for project use.
- **Source lead:** [shiliu520/Formula-SAE-Tire-Test-Data](https://github.com/shiliu520/Formula-SAE-Tire-Test-Data)
- **Claimed contents:** Calspan Round 6, May 2015, including cornering and drive/brake MATLAB archives and documentation.
- **Conflict:** the official FSAE TTC terms prohibit redistribution and online posting.
- **Reuse status:** **do not import**.
- **Decision:** the repository may be cited only as evidence that unauthorized copies circulate. Do not download, copy, fit or derive project parameters from these files unless the rights holder supplies explicit permission and provenance.

### Large experimental vehicle dataset with measured tire forces

- **Evidence class:** D / possible A lead.
- **Paper:** [Model-based vs Data-driven Estimation of Vehicle Sideslip Angle and Benefits of Tyre Force Measurements](https://arxiv.org/abs/2206.15119)
- **Reported scope:** 216 experimental maneuvers with measured tire forces used to compare sideslip estimators.
- **RealCars use:** possible source of full-vehicle validation trajectories and force-sensor methodology.
- **Reuse status:** **unverified lead**. No confirmed raw-data download was found in this search. Check supplementary material and contact the authors before treating it as a dataset.

## Open parameter-identification and fitting workflows

### TUM FTM Tire Parameter and Uncertainty Estimation

- **Evidence class:** C, with example input data.
- **Repository:** [TUMFTM/Tire_Parameter_and_Uncertainty_Estimation](https://github.com/TUMFTM/Tire_Parameter_and_Uncertainty_Estimation)
- **License:** MIT.
- **Purpose:** fits a simplified Magic Formula model from real-world driving measurements.
- **Workflow includes:**
  - preprocessing raw and filtered CSV data,
  - calculating tire states from vehicle velocity measurements such as Correvit data,
  - estimating tire forces from IMU and vehicle data,
  - deterministic optimization using Nelder–Mead,
  - stochastic variational inference and parameter uncertainty output,
  - parameter bounds and example configuration files.
- **Associated paper:** [Bayesian Optimization-based Tire Parameter and Uncertainty Estimation for Real-World Data](https://arxiv.org/abs/2504.20863)
- **RealCars use:** reference architecture for fitting, uncertainty reporting and separating raw measurements from derived tire states.
- **Caveat:** verify whether each bundled example file is measured, synthetic or altered before using it as a numerical validation target.

### Magic Formula Tyre Tool

- **Evidence class:** C, with obscured demonstration data.
- **Repository:** [teasit/magic-formula-tyre-tool](https://github.com/teasit/magic-formula-tyre-tool)
- **Status:** archived read-only on 2026-03-06.
- **License:** GPL-3.0.
- **Capabilities:**
  - MATLAB GUI for Magic Formula 6.1.2 fitting,
  - segmentation of time-series data into steady-state points,
  - comparison of measured and modeled outputs,
  - loading and saving `.tir` files,
  - parsers for FSAE TTC cornering and drive/brake MATLAB structures.
- **Data note:** demonstration data are described as de-identified or obscured rather than original unrestricted TTC measurements.
- **RealCars use:** study fitting workflow, channel mapping and diagnostics.
- **Reuse status:** **license review required** for code reuse because GPL-3.0 obligations may not match the future game distribution. Independent reimplementation of concepts must avoid copying protected code.

### MFeval

- **Evidence class:** C.
- **Source:** [MathWorks File Exchange — MFeval](https://www.mathworks.com/matlabcentral/fileexchange/63618-mfeval)
- **Capabilities:** evaluates Magic Formula 5.2, 6.1 and 6.2 parameter files and supports `.tir`-based workflows in MATLAB/Simulink.
- **RealCars use:** cross-checking the numerical output of a future independent MF implementation against an established evaluator.
- **Reuse status:** **license review required**. Inspect the exact File Exchange license before copying or redistributing code.
- **Caveat:** MATLAB/Simulink dependency makes it a validation-side tool, not a runtime dependency candidate.

## Example parameter files and reference implementations

### Modelica ExternData sample MF6.1 `.tir`

- **Evidence class:** B/C fixture.
- **File:** [modelica-3rdparty/ExternData — sample.tir](https://github.com/modelica-3rdparty/ExternData/blob/master/ExternData/Resources/Examples/sample.tir)
- **Contents:** a broad MF6.1 property file with geometry, pressure, mass/inertia, vertical stiffness and damping, valid ranges, scaling factors, pure and combined longitudinal/lateral coefficients, moments and relaxation terms.
- **Internal warning:** descriptive comments and numeric fields appear inconsistent about the nominal tire size. This reinforces that the file is an example, not a traceable production-tire measurement.
- **RealCars use:** parser fixture, schema coverage test and deterministic evaluator regression input.
- **Reuse status:** inspect the parent repository license before committing a copy.
- **Decision:** never label this file as a calibrated tire.

### OpenTire / OpenTirePython

- **Evidence class:** C, with example parameter sets.
- **Repository:** [OpenTire/OpenTirePython](https://github.com/OpenTire/OpenTirePython)
- **License:** MIT.
- **Purpose:** open mathematical tire-model library with a common interface and example notebooks.
- **Coverage:** includes a PAC2002 example and references the commonly reproduced literature 205/60R15 parameter set.
- **RealCars use:** compare interfaces, equations and test cases; locate original literature sources behind commonly copied coefficients.
- **Caveat:** inactivity and widespread reuse of literature example values do not establish provenance or accuracy for a particular physical tire.

### Project Chrono tire models

- **Evidence class:** C, with illustrative model parameter files.
- **Documentation:** [Project Chrono — Tire models](https://api.projectchrono.org/wheeled_tire.html)
- **Implemented families:** rigid tires, Pacejka 89, Pacejka 2002, TMeasy, Fiala and finite-element tire models.
- **Useful details:**
  - Pac02 accepts Adams/Car-compatible `.tir` files,
  - handling models include transient/contact-patch slip-state support to varying degrees,
  - example JSON files expose vertical load curves, stiffnesses, friction, rolling resistance and relaxation lengths,
  - documentation includes a handling-model validation study against measured vehicle test data.
- **Important limitation stated by Chrono:** its Pac02 implementation omits inflation-pressure dependence and large-camber terms because suitable reference datasets were unavailable.
- **RealCars use:** independent behavioral cross-check, test-case design and comparison of low-speed/transient handling strategies.
- **Decision:** example values are implementation fixtures, not automatically authoritative physical data.

## Data formats and interchange standards

### TYDEX 1.3

- **Evidence class:** C/standard.
- **Specification:** [TYDEX Format, Release 1.3](https://www.fast.kit.edu/download/DownloadsFahrzeugtechnik/TY100531_TYDEX_V1_3.pdf)
- **Publication:** 1997, Unrau and Zamow, produced by an international working group.
- **Terms stated by the manual:** the format may be used without special permission or a license fee.
- **Purpose:** exchange tire measurement and model data.
- **Structure:** `.tdx` files with sections such as `HEADER`, `COMMENTS`, `CONSTANTS`, `MEASURCHANNELS`, `MEASURDATA` and model-specific sections.
- **Coverage:** metadata, units, axes, channel definitions, measured samples and standard tire variables including longitudinal slip.
- **RealCars use:** leading candidate for a future measured-data import interface and archival format.
- **Caveat:** the format being open does not remove restrictions attached to measurements stored inside a `.tdx` file.

## Wet traction, hydroplaning and road friction

### NASA TM X-72805 — dry, wet and flooded runway traction

- **Evidence class:** A/D.
- **Record:** [NASA NTRS 19760011408](https://ntrs.nasa.gov/citations/19760011408)
- **Tire:** 30 x 11.5-14.5 Type VIII aircraft tire.
- **Test range:**
  - speeds from 5 to 100 knots,
  - yaw angles through 12 degrees,
  - dry, wet and flooded runway surfaces.
- **Reported outputs:** friction coefficient versus slip, maximum and skidding drag coefficients, cornering coefficients and effects of speed, yaw and water.
- **Access:** NASA public report; the NTRS record permits public use of the government work subject to its stated terms.
- **RealCars use:** validate qualitative wet-friction degradation, flooded-surface behavior and hydroplaning transitions.
- **Caveat:** aircraft construction, pressure, load and tread differ substantially from road-car tires. Do not directly transplant coefficients.

### Public pavement skid-resistance data

- **Evidence class:** A/E depending on collection.
- **Examples:** reports and datasets in the U.S. Department of Transportation ROSA P archive and FHWA Long-Term Pavement Performance program.
- **Typical methods:** locked-wheel measurements following ASTM E274, controlled surface wetting, smooth or ribbed standardized test tires and repeated road-speed measurements.
- **Potential variables:** skid number/friction index, speed, pavement type, texture, climate and maintenance history.
- **RealCars use:** construct defensible road-surface friction ranges and validate relative surface categories.
- **Caveat:** skid-number data are properties of a specific tire–surface–method combination. They do not directly define the peak and sliding coefficients for an arbitrary simulated tire.
- **Follow-up:** identify an official bulk-download endpoint and exact data dictionary before importing any records.

### Rubber friction and wet-road research

Publication leads with mechanistic curves rather than confirmed raw datasets:

- [Rubber friction and tire dynamics](https://arxiv.org/abs/1007.2713)
- [Sealing is at the Origin of Rubber Slipping on Wet Roads](https://arxiv.org/abs/cond-mat/0412045)
- [General contact mechanics theory for randomly rough surfaces with application to rubber friction](https://arxiv.org/abs/1506.06955)
- [Tire tread block dynamics](https://arxiv.org/abs/2602.22078)

These sources address rough-surface contact, viscoelastic friction, water sealing and flash/background temperature effects. They are candidates for later friction-model research, not immediate parameter sources for the current brush/Fiala baseline.

## Wear and thermal behavior

### NASA Technical Paper 1569 — wear, friction and temperature

- **Evidence class:** A/D.
- **Record:** [NASA NTRS 19800004758](https://ntrs.nasa.gov/citations/19800004758)
- **PDF:** [NASA TP-1569](https://ntrs.nasa.gov/api/citations/19800004758/downloads/19800004758.pdf)
- **Tires:** 22 x 5.5 Type VII aircraft tires with four tread elastomers.
- **Tests:**
  - fixed-slip-ratio braking on smooth and coarse asphalt and concrete,
  - yaw tests from 0 to 24 degrees on dry smooth asphalt,
  - stated test load 17.8 kN,
  - mostly 738 kPa inflation pressure,
  - stated test speed 32 km/h.
- **Outputs:** tread wear versus distance/slip/yaw, friction coefficient, tread temperature and carcass temperature.
- **RealCars use:** mechanism-level validation for heat generation, wear trends and coupling between slip and temperature.
- **Caveat:** aircraft-tire construction and operating pressure prevent direct road-car calibration.

### Pirelli Motorsport rally catalog

- **Evidence class:** E.
- **Source:** [Pirelli P Zero RA](https://www.pirelli.com/tyres/en-ww/motorsport/car/rally/catalog/p-zero-ra)
- **Published data:**
  - tire dimensions and rolling circumference,
  - compound/version codes,
  - intended wet/damp/dry usage,
  - working-temperature windows by compound,
  - recommended cold starting and hot ending pressures.
- **Example published temperature windows:**
  - RA9 super-soft: 20–60 °C,
  - RA7+ soft: 30–70 °C,
  - RA7 medium: 50–90 °C,
  - RA5 hard: 70–130 °C.
- **RealCars use:** plausible operating envelopes and compound-category metadata for a future motorsport thermal model.
- **Caveat:** catalog recommendations are not force-versus-slip measurements and cannot determine a complete friction curve.

### Hoosier FSAE tire specifications and spring-rate links

- **Evidence class:** E, with possible measured vertical-rate files.
- **Source:** [Hoosier — Tire Specs](https://www.hoosiertire.com/news/article/64377/Tire_Specs)
- **Published fields:** item number, size, outside diameter, tread and section width, recommended/measured rim, compound and approximate weight.
- **Additional resources:** the page links spring-rate data for several FSAE tire families and points to FSAE TTC force-and-moment data.
- **RealCars use:** geometry, mass and vertical-stiffness leads for small racing tires; useful for constructing test fixtures.
- **Reuse status:** **license review required** for linked files. Confirm test conditions, units and permission before copying numerical tables.
- **Caveat:** FSAE tires are not representative of ordinary passenger-car construction.

## Intelligent-tire and in-tire sensing research

The following papers use sensors on or inside the tire and a laboratory force reference, generally an MTS Flat-Trac. Raw files were not confirmed as public during this review, but the methods and plotted relationships are valuable:

- [Tire Slip Angle Estimation based on Intelligent Tire Technology](https://arxiv.org/abs/2010.06803)
- [Tire Lateral Force Prediction using Gaussian Process Regression](https://arxiv.org/abs/2009.12463)
- [Direct Slip Ratio Estimation using Intelligent Tire](https://arxiv.org/abs/2106.08961)
- [Abnormal Road Surface Detection Using Wheel Sensor Data](https://arxiv.org/abs/2108.09162)

Reported measurements include inner-liner accelerations or forces, vertical load, speed, slip angle or slip ratio, and laboratory reference forces. Potential RealCars uses are:

- estimating contact-patch entry and exit timing,
- studying transient delay and effective contact length,
- designing a future data-acquisition plan,
- comparing model states with quantities observable from a real instrumented tire.

**Reuse status:** publication reference only until supplementary-data and license checks are complete.

## Transient response and relaxation-length leads

No clean, public passenger-car raw dataset for relaxation length was found in this search. Relevant leads include:

- R. T. Uil, *Non-lagging effect of motorcycle tyres: An experimental study with the Flat Plank Tyre Tester*, Eindhoven University of Technology, 2006.
- P. W. A. Zegelaar, *The Dynamic Response of Tyres to Brake Torque Variations and Road Unevennesses*, Delft University of Technology, 1998.
- Project Chrono transient Pac02, TMeasy and Fiala implementations.

**Status:** D / unverified data leads. Search university repositories for full theses, appendices or accompanying measurements before extracting values from secondary summaries.

## Contact-patch and footprint data

A broad search for `tire footprint`, `contact pressure distribution`, `contact patch measurement` and related terms did not identify a clearly licensed, downloadable passenger-car dataset with dynamic pressure maps.

This remains a major validation gap for:

- vertical pressure distribution,
- lateral and longitudinal shear distribution,
- pneumatic trail derived from distributed stress,
- load and pressure effects on patch dimensions,
- transient patch deformation.

Publication figures may be digitizable for qualitative comparisons, but no such curve should become a calibration target without provenance, uncertainty and permission checks.

## Road-surface geometry and properties

### ASAM OpenCRG

- **Evidence class:** C plus example surface data.
- **Repository:** [asam-ev/OpenCRG](https://github.com/asam-ev/OpenCRG)
- **License:** Apache-2.0.
- **Purpose:** standard and software for high-precision road-surface descriptions on a curved regular grid.
- **Formats:** ASCII and binary CRG with clear-text headers.
- **Data:** normally elevation; the format can also carry other gridded road properties such as friction coefficient or gray values.
- **Software:** ANSI-C and MATLAB libraries for reading, modifying, evaluating, generating, analyzing and visualizing CRG files.
- **RealCars use:** road roughness, kerbs, scanned surfaces, vibration/ride inputs and spatial friction fields.
- **Caveat:** a CRG file describes the road input, not the tire response. Each included sample must be checked for its own provenance and license.

## Regulatory and catalog metadata

### EU EPREL tire database and Regulation (EU) 2020/740

- **Evidence class:** E.
- **Database:** [EPREL public tire search](https://eprel.ec.europa.eu/screen/product/tyres)
- **Regulation:** [Regulation (EU) 2020/740](https://eur-lex.europa.eu/eli/reg/2020/740/oj)
- **Typical registered/label fields:** supplier and product identity, tire class and size, fuel-efficiency/rolling-resistance class, wet-grip class, external rolling-noise value/class and, where applicable, snow and ice markings.
- **RealCars use:** classify real market tires and set coarse priors or validation bounds for rolling resistance, wet performance and noise.
- **Caveat:** label classes are intentionally low-resolution and test-procedure-specific. They cannot reconstruct lateral, longitudinal or combined-slip force curves.
- **Follow-up:** investigate whether EPREL exposes a documented bulk export or API appropriate for reproducible data ingestion.

### Rolling-resistance standards

Relevant procedures and regulations include:

- SAE J1269 — thermally equilibrated rolling-resistance measurement,
- SAE J2452 — stepwise coastdown-like rolling-resistance measurement,
- ISO 28580 — rolling-resistance measurement for tire classification,
- UN Regulation No. 117 and EU labeling rules.

These are test definitions, not open datasets. They are valuable for specifying a future RealCars rolling-resistance data schema and interpreting catalog/label results. Full standard texts may require purchase or licensed access.

## Additional publication-only leads

These papers may contain useful plots, experimental protocols or model structures. Raw data availability has not been confirmed:

- [Autonomous Drifting with 3 Minutes of Data via Learned Tire Models](https://arxiv.org/abs/2306.06330)
- [Experimental verification of online traction parameter identification](https://arxiv.org/abs/2303.09860)
- [Extended Pacejka Tire Model](https://arxiv.org/abs/2305.18422)
- [Rubber wear: experiment and theory](https://arxiv.org/abs/2411.07332)
- [Predicting Asphalt Pavement Friction Using Texture-Based Image Indicator](https://arxiv.org/abs/2507.03559)
- [High-Slip-Ratio Control for Peak Tire-Road Friction Estimation Using Automated Vehicles](https://arxiv.org/abs/2603.09073)

Each must be reviewed at the full-text and supplementary-material level before any curve is digitized or model equation is adopted.

## Unverified or rejected leads

### Zenodo DOI 10.5281/zenodo.7674873

A previous search lead associated this DOI with bicycle-tire characterization. Repeated searches on 2026-07-16 did not return a verifiable Zenodo record or metadata.

- **Reuse status:** **unverified lead**.
- **Decision:** do not cite it as an available dataset and do not rely on the claimed description until the DOI resolves through an authoritative metadata source.

### Search-result snippets and general-reference pages

Wikipedia, community posts and search-result snippets were used only to discover technical terminology and original references. They are not accepted as final evidence for project equations or parameters.

## Data gaps after this search

No unrestricted, clearly licensed dataset was found that simultaneously provides a modern passenger-car tire with all of the following:

- `Fx`, `Fy`, `Fz`, `Mx`, `My`, `Mz`,
- slip ratio and slip angle,
- camber,
- multiple loads and inflation pressures,
- multiple speeds,
- tread/carcass temperature,
- dry and wet surfaces,
- transient sweeps or relaxation behavior,
- documented uncertainty and calibration,
- rights suitable for committing data to a public repository and eventual game development.

Particularly weak public-data areas are:

- dynamic contact-patch pressure and shear maps,
- passenger-car relaxation-length measurements,
- combined-slip sweeps over temperature and pressure,
- road-car wear data tied to force history,
- complete wet-road force-and-moment matrices,
- raw modern production-tire data with commercial-use permission.

## Recommended acquisition and implementation sequence

1. Define a machine-readable source manifest with provenance, license, hash, tire identity, units, axes, test conditions and allowed uses.
2. Implement a TYDEX 1.3 reader only after representative legally usable fixtures are secured.
3. Add a generic `.tir` parser/evaluator fixture using explicitly illustrative data; never imply that it represents a real production tire.
4. Reproduce selected NASA curves as external validation plots without copying more source material than the document terms allow.
5. Evaluate the MathWorks/GCAPS data license and determine whether local, non-redistributed use is possible.
6. Use the TUM workflow as a design reference for a future independent fitter and uncertainty report.
7. Select one legally usable road-car calibration target before replacing generic coefficients.
8. Treat FSAE TTC as unavailable unless a license appropriate to the project is obtained.
9. Design a small RealCars measurement campaign if no suitable public dataset emerges: instrumented vehicle tests can identify low-slip stiffness and road-level behavior, although a force-and-moment rig is still required for complete tire isolation.

## Minimum metadata required before accepting a dataset

Any dataset proposed for import must record at least:

- source title, authors/organization, URL/DOI and retrieval date,
- license and redistribution/commercial-use status,
- tire manufacturer/model/size/construction and wear state,
- rim dimensions,
- inflation pressure and its measurement state,
- vertical load and load-control method,
- belt/drum/road surface and roughness,
- speed, slip-angle, slip-ratio and camber definitions,
- force/moment coordinate system and sign convention,
- units and sample rate,
- temperature channels and sensor locations,
- warm-up and test sequence,
- filtering, zeroing, corrections and uncertainty,
- whether points are raw, averaged, fitted, synthetic or altered,
- valid operating range and known anomalies,
- cryptographic hash of every imported file.

## Current decision

This register is a research inventory, not a calibration decision. The first real tire and dataset for RealCars remain unresolved. No numerical coefficient from this document may be copied into the simulation parameter files without a separate source review, explicit provenance and validation plan.
