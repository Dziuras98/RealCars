# Tire data source register — expanded search addendum

Reviewed: 2026-07-16

This document extends [`tire-data-source-register.md`](tire-data-source-register.md) with sources found during a second search pass. The same evidence and reuse labels defined in the main register apply here.

The addendum does not approve any numerical value for the RealCars tire model. It records material that may support later equation review, calibration-target selection, test design or independent validation.

## Search areas added in this pass

The second pass emphasized terms and source families that were underrepresented in the first register:

- government tire handbooks and regulatory test definitions,
- large comparative rolling-resistance studies,
- hydroplaning flow visualization,
- tread-rubber dynamic mechanical properties,
- foundational empirical and physical tire models,
- uneven-road and rolling-tire modal dynamics,
- thermodynamic tire-friction models,
- learned force models derived from full-vehicle experiments,
- open vehicle-dynamics implementations containing tire-model fixtures.

## Government and consensus references

### National Academies Special Report 286 — passenger-tire energy-performance data

- **Evidence class:** D/E, with a large comparative data analysis.
- **Source:** [Tires and Passenger Vehicle Fuel Economy: Informing Consumers, Improving Performance](https://nap.nationalacademies.org/catalog/11620/tires-and-passenger-vehicle-fuel-economy-informing-consumers-improving-performance)
- **Citation:** Transportation Research Board, 2006, Special Report 286, DOI `10.17226/11620`.
- **Scope:** the report reviews technical literature and analyzes energy-performance data from nearly 200 passenger tires.
- **Relevant sections:**
  - background on passenger tires,
  - tire influence on vehicle fuel consumption,
  - rolling resistance, traction and wear,
  - comparison of SAE rolling-resistance procedures.
- **RealCars use:**
  - establish realistic ranges and tradeoffs for passenger-tire rolling resistance,
  - understand how test-procedure choice affects reported results,
  - identify underlying data and cited laboratories for follow-up.
- **Reuse status:** **open/reference-safe for reading and citation**; copying tables or figures requires compliance with National Academies terms.
- **Caveat:** it is a synthesis and comparative analysis, not a complete force-and-moment dataset.

### NHTSA — The Pneumatic Tire, DOT HS 810 561

- **Evidence class:** C/D reference compendium.
- **Source lead:** National Highway Traffic Safety Administration, *The Pneumatic Tire*, DOT HS 810 561, 2006.
- **Likely official document location:** [NHTSA PDF path](https://www.nhtsa.gov/sites/nhtsa.gov/files/810561.pdf)
- **Coverage:** tire construction, materials, mechanics, force generation, rolling resistance, traction, wear, vibration, noise and testing terminology.
- **RealCars use:** authoritative terminology and a map to older primary literature; useful when auditing equations and assumptions in the current model.
- **Reuse status:** **reference-safe, file verification required**.
- **Caveat:** verify the current official host, edition and chapter authors before treating any reproduced equation or curve as a primary source. The handbook contains material of mixed provenance and age.

### NHTSA — Dynamic Mechanical Properties of Passenger and Light Truck Tire Treads

- **Evidence class:** A/D.
- **Source lead:** National Highway Traffic Safety Administration, *Dynamic Mechanical Properties of Passenger and Light Truck Tire Treads*, DOT HS 811 270, 2010.
- **Likely official document location:** [NHTSA PDF path](https://www.nhtsa.gov/sites/nhtsa.gov/files/811270.pdf)
- **Subject:** measured dynamic mechanical behavior of tread compounds, relevant to frequency- and temperature-dependent rubber response.
- **RealCars use:**
  - later thermal/viscoelastic friction research,
  - understanding why grip and rolling losses vary with temperature and excitation frequency,
  - defining the metadata required for material-property inputs.
- **Reuse status:** **reference-safe, file verification required**.
- **Caveat:** material specimens or tread compounds are not a direct replacement for complete tire force-and-moment measurements.

### 49 CFR 575.104 — Uniform Tire Quality Grading Standards

- **Evidence class:** C/E regulatory test definition.
- **Source:** [Electronic Code of Federal Regulations — 49 CFR 575.104](https://www.ecfr.gov/current/title-49/subtitle-B/chapter-V/part-575/section-575.104)
- **Coverage:** U.S. passenger-tire grading procedures and reporting framework for treadwear, traction and temperature resistance.
- **RealCars use:**
  - interpret UTQG metadata without treating it as a direct friction coefficient,
  - document test-specific meaning and limitations of consumer tire grades,
  - locate standardized comparison tires and procedures.
- **Reuse status:** **open/reference-safe**.
- **Caveat:** UTQG grades are coarse, manufacturer-assigned regulatory descriptors and cannot define a full `Fx`, `Fy` or combined-slip surface.

## Hydroplaning and wet-contact experiments

### NASA TN D-2056 — Phenomena of Pneumatic Tire Hydroplaning

- **Evidence class:** A/D foundational experimental report.
- **Citation:** Walter B. Horne and Robert C. Dreher, *Phenomena of Pneumatic Tire Hydroplaning*, NASA Technical Note D-2056, 1963.
- **Public catalog copy:** [HathiTrust record](http://babel.hathitrust.org/cgi/pt?id=uiug.30112106773275)
- **RealCars use:**
  - foundational hydroplaning mechanisms and empirical trends,
  - historical test-method comparison,
  - source tracing for pressure-, speed- and water-depth relationships.
- **Reuse status:** **reference-safe; verify reproduction terms for scanned pages**.
- **Caveat:** aircraft tires and historical test conditions differ materially from modern road-car tires. Any empirical relationship must be traced to its stated range and assumptions before use.

### Water flow inside grooves of a rolling tire using r-PIV

- **Evidence class:** A/D.
- **Paper:** [Analysis of the water flow inside tire grooves of a rolling car using r-PIV](https://arxiv.org/abs/2012.03347)
- **Experimental scope:** a real car rolls through a water puddle while refraction particle-image velocimetry measures local water velocity fields inside longitudinal tire grooves.
- **Additional observations:** the work reports three-component velocity measurements in groove cross-sections and identifies two-phase-flow effects associated with bubbles.
- **RealCars use:**
  - validate qualitative tread-groove drainage behavior,
  - inform a later hydroplaning submodel beyond a single scalar wet-friction multiplier,
  - identify measurable flow variables for CFD or reduced-order model comparison.
- **Reuse status:** **publication reference**; raw PIV fields and supplementary-data licensing must be checked separately.
- **Caveat:** this is fluid-flow information, not a direct force-and-moment matrix.

## Smart-tire and structural-dynamics measurements

### Accelerometer-based tire load and slip-angle estimation

- **Evidence class:** D, with reported experimental validation.
- **Paper:** [Accelerometer Based Method for Tire Load and Slip Angle Estimation](https://arxiv.org/abs/1901.08049)
- **Scope:** tire-mounted accelerometer signals, signal-processing methods and experimental estimation of tire load and slip angle.
- **RealCars use:**
  - understand which internal deformation signatures correspond to load and lateral state,
  - design future instrumented-tire measurements,
  - compare modeled contact-patch timing and deformation with observable accelerometer features.
- **Reuse status:** **publication reference** until raw data and supplementary terms are confirmed.

### Virtual-array operational modal analysis of a rolling tire

- **Evidence class:** D, with experimental modal identification.
- **Paper:** [Virtual-Array Operational Modal Analysis of Rolling Tires Using a Single Tire Cavity Accelerometer](https://arxiv.org/abs/2606.10437)
- **Test concept:** a wireless tire-cavity accelerometer, optical timing and repeated drum revolutions are used to synthesize a circumferential virtual sensor array.
- **Reported result:** the method identifies multiple circumferential modes of a treaded rolling tire in the low-frequency structural range.
- **RealCars use:**
  - future validation of tire-envelope, rigid-ring or flexible-ring dynamics,
  - identify modal targets for cleat and uneven-road tests,
  - distinguish handling-frequency states from ride/noise-frequency structural behavior.
- **Reuse status:** **publication reference**; no raw time histories were confirmed during this pass.
- **Caveat:** published in 2026 and should be independently reviewed before adoption.

### Deformation of a loaded inflated bicycle tire

- **Evidence class:** A/D small-scale experimental reference.
- **Paper:** [Deformation of an inflated bicycle tire when loaded](https://arxiv.org/abs/1902.03661)
- **Scope:** analytical load–deflection model compared with experiments on two bicycle tires.
- **RealCars use:** limited vertical-compliance sanity checks and methodology comparison.
- **Reuse status:** **publication reference**.
- **Caveat:** bicycle-tire geometry and construction make the numerical results unsuitable for direct passenger-car calibration.

## Thermal and learned force-model studies

### Tire thermodynamics in automated drifting

- **Evidence class:** D, with full-vehicle experiments.
- **Paper:** [Trajectory Planning Using Tire Thermodynamics for Automated Drifting](https://arxiv.org/abs/2407.12989)
- **Scope:** a simple thermodynamic model captures variation of tire friction with temperature and is used in trajectory planning for automated drifting.
- **RealCars use:**
  - reference structure for a minimal tire-temperature state model,
  - validation ideas for heat-generation and grip-feedback coupling,
  - evidence that fixed friction parameters can be inadequate at the limit.
- **Reuse status:** **publication reference**; parameter provenance and raw experimental data must be checked.

### Learned tire-force models from a full-scale drifting vehicle

- **Evidence class:** D, possible data-acquisition methodology lead.
- **Paper:** [Autonomous Drifting with 3 Minutes of Data via Learned Tire Models](https://arxiv.org/abs/2306.06330)
- **Scope:** learned tire-force models are fitted from a small amount of full-vehicle driving data and used in nonlinear model-predictive control on a modified Toyota Supra.
- **RealCars use:**
  - compare analytical and learned residual models,
  - estimate what full-vehicle data can and cannot identify without a tire test rig,
  - design out-of-sample validation for highly nonlinear combined-slip operation.
- **Reuse status:** **publication reference**; no unrestricted raw dataset was confirmed.
- **Caveat:** successful control performance does not by itself prove that the learned internal tire forces are physically identifiable or transferable.

### Neural-network front-tire lateral-force model for drifting

- **Evidence class:** D.
- **Paper:** [Neural Network Tire Force Modeling for Automated Drifting](https://arxiv.org/abs/2407.13760)
- **Scope:** a neural network predicts front-tire lateral force in a full-scale automated drifting vehicle and is compared with a brush-model baseline.
- **RealCars use:** later residual-model experiments and identification of conditions where a simple brush model omits latent dynamics.
- **Reuse status:** **publication reference**.
- **Caveat:** this is not a general production-tire dataset and should not replace a physically interpretable baseline without separate validation.

## Foundational equation and model sources

These sources are primarily equation/model references rather than freely downloadable raw data. They are useful for auditing the current implementation and choosing future model families.

### Bakker, Nyborg and Pacejka — early Magic Formula publication

- **Evidence class:** C/D.
- **Citation:** E. Bakker, L. Nyborg and H. B. Pacejka, *Tyre Modelling for Use in Vehicle Dynamics Studies*, SAE Technical Paper 870421, 1987.
- **Publisher lead:** [SAE Mobilus search by paper number](https://saemobilus.sae.org/search/?qt=870421)
- **Importance:** early formal presentation of a compact empirical representation fitted to measured pure-cornering and pure-braking behavior.
- **RealCars use:** trace the origin, intended variable definitions and limitations of Magic Formula-style curves.
- **Reuse status:** **license review required**; SAE full text may require purchase or institutional access.

### Dugoff, Fancher and Segel — analytical tire-performance model

- **Evidence class:** C/D.
- **Citation:** H. Dugoff, P. S. Fancher and L. Segel, *An Analysis of Tire Traction Properties and Their Influence on Vehicle Dynamic Performance*, SAE Technical Paper 700377, 1970.
- **Publisher lead:** [SAE Mobilus search by paper number](https://saemobilus.sae.org/search/?qt=700377)
- **Importance:** a classic analytical combined-slip framework widely used in estimation and control literature.
- **RealCars use:** compare a sourced combined-slip derivation with the current provisional ellipse projection.
- **Reuse status:** **license review required**.
- **Caveat:** later reproductions often alter notation or assumptions; implementation must be checked against the original paper or a traceable authoritative derivation.

### Fiala stretched-string / brush tire model

- **Evidence class:** C/D.
- **Citation lead:** E. Fiala, *Seitenkräfte am rollenden Luftreifen*, VDI-Zeitschrift, 1954.
- **Importance:** foundational physical approximation behind many later Fiala/brush formulations.
- **RealCars use:** audit the current Fiala-style saturation polynomial, contact-length assumptions and aligning-moment derivation.
- **Reuse status:** **license and source-location review required**.
- **Caveat:** many modern implementations called “Fiala” are not equation-identical. The exact formulation and sign conventions must be recorded.

### TMeasy

- **Evidence class:** C/D, with public supporting material and implementations in other projects.
- **Primary citation:** W. Hirschberg, G. Rill and H. Weinfurter, *Tire Model TMeasy*, Vehicle System Dynamics, volume 45 supplement 1, 2007, pages 101–119.
- **Project site:** [TMeasy](https://www.tmeasy.de/)
- **Model focus:** compact handling model with intuitive characteristic parameters and transient extensions.
- **RealCars use:**
  - compare parameter identifiability with Magic Formula,
  - derive a second independent reference model for regression testing,
  - evaluate transient slip-state handling.
- **Reuse status:** **publication/license review required** before copying equations or code.

### Zegelaar and Pacejka — in-plane dynamics on uneven roads

- **Evidence class:** C/D.
- **Citation:** P. W. A. Zegelaar and H. B. Pacejka, *The In-Plane Dynamics of Tyres on Uneven Roads*, Vehicle System Dynamics, 1996.
- **Scope:** rolling-tire response to road unevenness and in-plane dynamic behavior.
- **RealCars use:** connect future wheel rotational dynamics, enveloping/road input and transient longitudinal response.
- **Reuse status:** **license review required**.
- **Caveat:** locate and inspect the original paper before adopting any reduced-order equation from secondary sources.

### SWIFT / rigid-ring intermediate-frequency tire model

- **Evidence class:** C/D.
- **Citation lead:** I. J. M. Besselink, H. B. Pacejka, A. J. C. Schmeitz and S. T. H. Jansen, SWIFT rigid-ring tire-model publications from the mid-2000s.
- **Scope:** extends handling-oriented tire behavior toward short-wavelength road inputs and intermediate-frequency dynamics.
- **RealCars use:** model-family comparison for kerbs, cleats and road roughness after steady-state handling is validated.
- **Reuse status:** **unverified bibliographic lead / license review required**.
- **Decision:** do not implement from secondary summaries; first identify the exact publication, version and parameter definitions.

## Additional open implementations and parameter fixtures

### OpenVD — Vehicle Dynamics Lateral

- **Evidence class:** C, with illustrative coefficients.
- **Repository:** [andresmendes/Vehicle-Dynamics-Lateral](https://github.com/andresmendes/Vehicle-Dynamics-Lateral)
- **License:** permissive three-clause BSD-style license.
- **Contents:** open MATLAB vehicle-dynamics package with vehicle and tire classes, examples and documentation.
- **Tire material:** includes a `TirePacejka` implementation with load-dependent lateral coefficients and tire-comparison examples.
- **RealCars use:**
  - independent equation and sign-convention comparison,
  - plotting and scenario ideas,
  - illustrative regression fixtures.
- **Caveat:** default coefficients in the source are examples, not traceable measurements of a named production tire.

### Fastest-lap

- **Evidence class:** C, with open vehicle/tire parameter fixtures.
- **Repository:** [juanmanzanero/fastest-lap](https://github.com/juanmanzanero/fastest-lap)
- **License:** MIT.
- **Contents:** C++ vehicle-dynamics and lap-time optimization library with Python access, transient vehicle models, tests, examples and XML parameter databases.
- **Published model fixtures:** Formula One and kart examples tied to cited vehicle-dynamics literature.
- **RealCars use:**
  - compare architecture for testable C++ dynamics,
  - inspect tire/vehicle parameter schemas,
  - cross-check G-G diagrams and optimal-lap validation scenarios,
  - study automatic differentiation and optimization integration without making it a runtime dependency.
- **Caveat:** database parameters must be traced to their cited publications before being treated as measurements.

## Sources deliberately not promoted to calibration targets

The expanded search also found many simulator parameter files, student projects, forum tables and copied Magic Formula coefficients. They are not listed individually because most lack one or more of:

- named tire identity,
- documented test conditions,
- original provenance,
- units and sign definitions,
- uncertainty,
- a license permitting redistribution or commercial use.

Such files may be used to discover original references, but not as calibration evidence.

## New follow-up actions

1. Obtain verified official copies of DOT HS 810 561 and DOT HS 811 270 and record file hashes.
2. Review the National Academies report bibliography to identify the underlying nearly-200-tire data sources and whether any tables are reusable.
3. Check supplementary material for the r-PIV, smart-tire, modal-analysis and drifting papers.
4. Acquire the original Bakker, Dugoff, Fiala, TMeasy and Zegelaar publications before changing equations.
5. Build a comparison matrix for the current RealCars model, Dugoff, Fiala, TMeasy and a limited Magic Formula evaluator.
6. Keep handling-force calibration separate from intermediate-frequency structural and hydroplaning models.

## Current decision

This addendum broadens the research inventory but does not resolve the first real-tire calibration target. No coefficient or empirical relationship listed here may be copied into production data without a dedicated source review, operating-range check and validation plan.
