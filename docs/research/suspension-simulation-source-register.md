# Suspension simulation source register

Last reviewed: 2026-07-16

## Purpose

This register records candidate sources for developing and validating suspension simulation in RealCars. It covers analytical models, multibody formulations, kinematics and compliance, component characterization, road inputs, open datasets, standards, test equipment, reference implementations and commercial-tool documentation.

A source being listed here does **not** approve any equation, topology or numerical parameter for implementation. Before a value or model is introduced into the simulation core, the project must verify its provenance, units, coordinate system, operating range, test conditions, licensing and applicability to the intended vehicle.

The current project has no implemented suspension subsystem and no selected real suspension calibration target.

## Evidence classes

- **A — raw measured data:** time histories, force–displacement curves, force–velocity loops, K&C tables or road profiles with documented conditions.
- **B — experimentally validated model:** equations or fitted parameters compared against physical measurements.
- **C — authoritative theory or standard:** books, standards and foundational papers suitable for equation and terminology review.
- **D — reference implementation or model fixture:** useful for architecture, regression or independent comparison, but not itself proof of physical accuracy.
- **E — commercial workflow documentation:** useful for identifying accepted industrial procedures, required inputs and validation methods.
- **F — research lead:** relevant publication or source family whose full text, supplemental data or licensing still requires verification.

## Search vocabulary used

The search used combinations of:

- `vehicle suspension simulation`, `suspension dynamics`, `ride dynamics`,
- `quarter car`, `half car`, `full car`, `7 DOF`, `14 DOF`,
- `double wishbone kinematics`, `MacPherson strut model`, `multilink suspension`,
- `suspension multibody dynamics`, `MBD suspension`, `flexible multibody suspension`,
- `kinematics and compliance`, `K&C test`, `elastokinematics`, `compliance steer`,
- `hardpoint optimization`, `wheel travel`, `camber gain`, `toe curve`, `roll center`,
- `spring force displacement`, `bump stop`, `jounce bumper`, `anti-roll bar`,
- `damper force velocity`, `shock dyno`, `damper hysteresis`, `cavitation`,
- `suspension bushing stiffness`, `hydraulic bushing`, `nonlinear bushing`,
- `air spring model`, `hydropneumatic suspension`, `inerter`,
- `semi-active suspension`, `skyhook`, `MR damper`, `active suspension`,
- `four post rig`, `K&C rig`, `N-post rig`, `road load data acquisition`,
- `ISO 8608 road profile`, `LTPP profile`, `ERD road data`, `OpenCRG`,
- `ride comfort`, `whole-body vibration`, `wheel hop`, `suspension travel`,
- `suspension dataset`, `active suspension dataset`, `K&C dataset`,
- `Project Chrono suspension`, `Simscape suspension`, `Modelica vehicle suspension`.

## Principal findings

1. Public suspension data are substantially easier to obtain than complete production-tire force-and-moment data, but most open datasets concern reduced-order quarter-car systems or road profiles rather than production suspension geometry.
2. A credible suspension implementation needs separate validation layers: component curves, pure kinematics, quasi-static K&C, vertical ride response, full-vehicle handling and transient/impact behavior.
3. Rigid multibody kinematics are insufficient when bushing, subframe, control-arm or body flexibility materially affects toe, camber and compliance steer.
4. A mapped K&C representation can be computationally efficient and suitable for real time, but it must be generated from measured or independently validated multibody data and must preserve its valid input domain.
5. Damper force cannot generally be represented by a single memoryless force–velocity curve when hysteresis, gas pressure, temperature, cavitation or frequency effects are relevant.
6. Road-profile provenance is part of suspension validation. A synthetic ISO 8608 spectrum and a measured two-track road profile are not interchangeable test inputs.

## Recommended fidelity ladder for research

This is a source-backed research sequence, not an implementation decision.

1. **Single-DOF body isolation model** — establish sign conventions, natural frequency, damping ratio and integration tests.
2. **Two-DOF quarter-car model** — sprung mass, unsprung mass, suspension spring/damper and tire vertical stiffness; validate transfer functions and step/sine/random response.
3. **Half-car pitch or roll model** — introduce axle coupling, pitch/roll inertia, anti-roll behavior and left/right or front/rear road phase.
4. **Full 7-DOF ride model** — heave, pitch, roll and four unsprung masses; validate four-post and wheel-hop behavior.
5. **Kinematic suspension corner** — explicit hardpoints, joints, wheel carrier and motion ratios; validate camber, toe, caster, track and wheel-center paths.
6. **Compliant suspension corner** — nonlinear six-axis bushings, steering compliance, subframe mounts and component flexibility; validate against K&C tests.
7. **Full multibody vehicle** — couple suspension, steering, tire, chassis and driveline states; validate standard handling and ride maneuvers.
8. **Optional high-frequency/flexible extension** — flexible links, structural modes, hydraulic bushings, detailed dampers and suspension NVH only after lower-frequency handling and ride models are validated.

## Foundational books and reference works

### Gillespie — Fundamentals of Vehicle Dynamics, Revised Edition

- **Evidence class:** C.
- **Source:** [SAE R-506](https://saemobilus.sae.org/books/fundamentals-vehicle-dynamics-revised-edition-r-506)
- **Citation:** Thomas D. Gillespie, *Fundamentals of Vehicle Dynamics, Revised Edition*, SAE International, 2021, DOI `10.4271/R-506`.
- **Coverage:** ride, handling, roll, suspension and steering fundamentals with equations and worked applications.
- **RealCars use:** baseline terminology, reduced-order ride models, load transfer and validation metrics.
- **Reuse status:** licensed reference; do not reproduce substantial text or figures.

### Blundell and Harty — The Multibody Systems Approach to Vehicle Dynamics

- **Evidence class:** C.
- **Source:** [Elsevier, second edition](https://shop.elsevier.com/books/the-multibody-systems-approach-to-vehicle-dynamics/blundell/978-0-08-099425-3)
- **Coverage relevant to suspension:**
  - suspension-system types,
  - quarter-vehicle modeling,
  - suspension characteristic determination,
  - compliance-matrix methods,
  - kinematics and vector-analysis case studies,
  - ride and durability studies,
  - full-vehicle multibody assembly,
  - model data sets and proving-ground standards.
- **RealCars use:** primary architectural reference for progressing from simple analytical models to validated multibody models.
- **Reuse status:** licensed reference.

### Jazar — Vehicle Dynamics: Theory and Application

- **Evidence class:** C.
- **Source:** [Springer, fourth edition](https://link.springer.com/book/10.1007/978-3-031-74458-7)
- **Citation:** Reza N. Jazar, fourth edition, 2025, DOI `10.1007/978-3-031-74458-7`.
- **Relevant sections:** applied kinematics, suspension mechanisms, vehicle roll dynamics, vehicle vibrations and suspension optimization.
- **RealCars use:** analytical derivations and comparison of kinematic, vibration and optimization formulations.
- **Reuse status:** licensed reference.

### Dixon — The Shock Absorber Handbook

- **Evidence class:** C.
- **Source:** [Wiley Online Books](https://onlinelibrary.wiley.com/doi/book/10.1002/9780470516430)
- **Citation:** John C. Dixon, second edition, 2007, DOI `10.1002/9780470516430`.
- **Coverage:** vibration theory, ride and handling, installation and motion ratios, fluid mechanics, valves, force characteristics, adjustable dampers, ER/MR dampers, specification and laboratory testing.
- **RealCars use:** principal reference for damper state variables, test interpretation and deciding when a lookup curve is inadequate.
- **Reuse status:** licensed reference.

### Milliken and Milliken — Race Car Vehicle Dynamics

- **Evidence class:** C.
- **Source:** [Milliken Research Associates description](https://www.millikenresearch.com/rcvd.html)
- **Coverage:** wheel loads, suspension geometry, springs, dampers, compliances, steering, transient and steady-state vehicle behavior.
- **Companion:** *Chassis Design: Principles and Analysis* focuses more deeply on suspension and steering, springing, unsprung-mass oscillation and sprung-mass roll.
- **RealCars use:** independent motorsport-oriented reference for ride/roll rates, wheel-load analysis and suspension design terminology.
- **Reuse status:** licensed reference.

## Standards and terminology

### ISO 8855:2011 — vehicle-dynamics vocabulary

- **Evidence class:** C.
- **Source:** [ISO 8855:2011](https://www.iso.org/standard/51180.html)
- **Status:** current; reviewed and confirmed in 2024.
- **Use:** authoritative definitions and coordinate terminology for road-vehicle dynamics.

### SAE J670_202206 — Vehicle Dynamics Terminology

- **Evidence class:** C.
- **Source:** [SAE J670_202206](https://saemobilus.sae.org/standards/j670_202206-vehicle-dynamics-terminology)
- **Coverage:** axis systems, bodies, suspension and steering, tires, controls, disturbances and response descriptors.
- **Use:** reconcile SAE and ISO terminology before implementing import/export adapters.

### ISO 8608:2016 — measured road profiles

- **Evidence class:** C.
- **Source:** [ISO 8608:2016](https://www.iso.org/standard/71202.html)
- **Scope:** uniform reporting of measured one-track and multiple-track vertical road profiles for roads and off-road terrain.
- **Status note:** published and under systematic review as of 2026.
- **Use:** road-input metadata, spectral classification and reproducible random-road generation.
- **Limitation:** the standard defines reporting, not the complete acquisition method or a universal synthetic-road generator.

### ISO 2631-1:1997 — whole-body vibration

- **Evidence class:** C.
- **Source:** [ISO 2631-1:1997](https://www.iso.org/standard/7612.html)
- **Status note:** current in July 2026 but expected to be replaced; ISO/DIS 2631-1 edition 3 is under development.
- **Use:** human-exposure and ride-comfort assessment rather than direct suspension calibration.

### ISO 10326-1:2016 — vehicle-seat vibration laboratory method

- **Evidence class:** C.
- **Source:** [ISO 10326-1:2016](https://www.iso.org/standard/67659.html)
- **Scope:** laboratory testing of vibration transmission through a vehicle seat, including instrumentation, assessment and reporting.
- **Use:** later seat/occupant validation; not a substitute for wheel-to-body suspension validation.

### ISO 4138:2021 and ISO 7401:2011

- **Evidence class:** C.
- **Sources:**
  - [ISO 4138:2021 — steady-state circular driving](https://www.iso.org/standard/81710.html)
  - [ISO 7401:2011 — lateral transient response](https://www.iso.org/standard/54144.html)
- **Use:** full-vehicle validation after the suspension is coupled to steering and tires.

## Reduced-order analytical models

### Quarter-car model

- **Evidence class:** C/B depending on parameter source.
- **Core states:** sprung and unsprung displacement/velocity.
- **Required physical inputs:** sprung mass, unsprung mass, suspension force law, tire vertical force law and road displacement.
- **Outputs:** body acceleration, suspension travel, dynamic tire load, wheel-hop response and transfer functions.
- **Reference lead:** Gillespie, Jazar and Blundell/Harty.
- **Additional paper:** [SAE 2018-01-0554](https://saemobilus.sae.org/papers/suspension-systems-new-analytical-formulas-describing-dynamic-behavior-2018-01-0554) derives analytical statistics for passive/active linear quarter-car variants, including an additional series spring representing a rubber mounting.
- **RealCars use:** mandatory diagnostic baseline even if the production implementation later uses explicit suspension geometry.

### Half-car and full 7-DOF ride models

- **Evidence class:** C/B.
- **Typical additional states:** body pitch and/or roll plus independent unsprung masses.
- **Reference implementation lead:** the shock-model paper SAE 2007-01-0845 evaluates the same nonlinear damper in 3-DOF handling and 7-DOF ride models.
- **RealCars use:** isolate vertical integration, roll/pitch coupling, anti-roll distribution and road-input phasing before introducing multibody constraints.

## Suspension kinematics and geometry

A kinematic suspension model should expose at least:

- wheel-center trajectory,
- camber, toe and caster versus wheel travel,
- track and wheelbase change,
- kingpin/steering-axis geometry,
- motion ratios for spring, damper and anti-roll bar,
- roll-center or equivalent force-line behavior,
- steering-rack and tie-rod coupling,
- joint limits and singularities.

### Project Chrono suspension templates

- **Evidence class:** D.
- **Documentation:** [Project Chrono suspension models](https://api.projectchrono.org/wheeled_suspension.html)
- **Repository:** [projectchrono/chrono](https://github.com/projectchrono/chrono)
- **License:** BSD-3-Clause.
- **Current public templates include:** double wishbone, reduced double wishbone, MacPherson strut, multi-link, semi-trailing arm, solid axle, three-link solid axles, bellcrank variants, leaf-spring axles, three-link IRS, rigid and generic suspension templates.
- **Architecture:** suspension types define bodies, joints, force elements, topology and hardpoints relative to an ISO-aligned suspension frame.
- **RealCars use:** strongest open C++ reference found for explicit suspension topology, hardpoint schemas and full-vehicle coupling.
- **Caveat:** example vehicle parameters are fixtures. Trace each numerical value before reuse.

### MathWorks suspension templates

- **Evidence class:** D/E.
- **Source:** [Vehicle Suspension System Templates](https://www.mathworks.com/help/sm/ug/independent-suspension-system-templates.html)
- **Included examples:** double wishbone, MacPherson and pushrod systems with independently driven road platforms.
- **RealCars use:** visual and numerical comparison of topology, road actuation, chassis roll and bounce.
- **Reuse status:** MathWorks licensing applies; use as an independent reference, not a runtime dependency.

### Over-constrained suspension methodology

- **Evidence class:** C/F.
- **Source:** [SAE 2006-01-0561](https://saemobilus.sae.org/papers/kinematic-simulation-modeling-constrained-suspensions-2006-01-0561)
- **Subject:** procedures for simulating over-constrained solid-axle layouts when bushing compliance is necessary for articulation but not yet known in early design.
- **Use:** warning against blindly imposing ideal joints on mechanisms whose physical motion depends on elastic deformation.

## Kinematics and compliance validation

### AB Dynamics SPMM Plus

- **Evidence class:** E, with direct measurement-method documentation.
- **Source:** [AB Dynamics K&C test equipment](https://www.abdynamics.com/lab-testing/kinematics-compliance-test-equipment/)
- **Method:**
  - quasi-static body motions for kinematics,
  - wheel-center position measurement while controlling contact-patch loads,
  - longitudinal, lateral and steer-axis inputs at wheel stations for compliance,
  - integrated load-cell measurement of forces and displacements.
- **Outputs:** geometry and stiffness characteristics used to build and correlate virtual suspension models.
- **RealCars use:** basis for a future K&C data schema and validation rig emulator.

### Foundational K&C facility paper

- **Evidence class:** C/E.
- **Source:** [SAE 970096](https://saemobilus.sae.org/papers/design-operation-a-new-vehicle-suspension-kinematics-compliance-facility-970096)
- **Scope:** four-wheel automated quasi-static facility and representative results.

### K&C measurement-to-model validation

- **Evidence class:** B/C.
- **Source:** [SAE 980897](https://saemobilus.sae.org/papers/suspension-kinematics-compliance-measuring-simulation-980897)
- **Scope:** test-bench procedure and validation of an ADAMS vehicle model using measured K&C results.
- **Additional correlation source:** [SAE 2007-26-046](https://saemobilus.sae.org/papers/correlation-issues-testing-simulation-kinematics-compliance-automotive-suspensions-2007-26-046).
- **RealCars use:** define separate kinematic and compliance error reports rather than judging only final vehicle trajectories.

### Flexible-body K&C

- **Evidence class:** B/C.
- **Source:** [SAE 2010-01-0951](https://saemobilus.sae.org/papers/kinematics-compliance-k-c-simulation-using-a-nonlinear-finite-element-model-2010-01-0951)
- **Finding:** nonlinear finite-element models with flexible suspension components and body structures can differ significantly from rigid multibody K&C results.
- **RealCars use:** criterion for deciding when rigid links plus bushings cease to be adequate.

### Mapped and data-driven K&C

- **Evidence class:** B/D.
- **Sources:**
  - [MathWorks Kinematics and Compliance Virtual Test Laboratory](https://www.mathworks.com/help/vdynblks/ug/kinematics-and-compliance-virtual-test-laboratory.html)
  - [Mapped suspension generation and comparison](https://www.mathworks.com/help/vdynblks/ug/kinematics-and-compliance-reference-application.html)
  - [SAE 2022-01-0287](https://saemobilus.sae.org/papers/neural-network-based-suspension-kinematics-compliance-characteristics-implementation-full-vehicle-dynamics-model-2022-01-0287)
  - [SAE 2025-01-8282](https://saemobilus.sae.org/papers/a-suspension-model-based-semi-recursive-method-data-driven-compliance-characteristics-2025-01-8282)
  - [SAE 2026-01-0584](https://saemobilus.sae.org/papers/target-cascading-kinematics-compliance-tables-suspension-design-2026-01-0584)
- **Use:** investigate real-time mappings after a validated high-fidelity source model exists.
- **Caveat:** preserve independent variables, interpolation method, extrapolation behavior and training-domain coverage.

## Springs, anti-roll bars and travel stops

Required data forms should support:

- force versus displacement with preload and installed length,
- progressive or piecewise stiffness,
- hysteresis where present,
- motion ratio as a function of suspension position,
- bump-stop and rebound-stop engagement,
- anti-roll-bar geometry and torsional stiffness,
- cross-axle coupling and blade-adjuster state,
- temperature or rate dependence when supported by evidence.

No public production-passenger-car dataset with complete spring, anti-roll-bar and stop curves was identified in this pass. Manufacturer catalog rates can be leads, but installed force at the wheel must account for geometry, preload and compliance.

## Damper modeling and measurement

### Shock-dynamometer hysteresis model

- **Evidence class:** B.
- **Source:** [SAE 2007-01-0845](https://saemobilus.sae.org/papers/evaluation-a-shock-model-vehicle-simulation-2007-01-0845)
- **Method:** damper characteristics measured on a shock dynamometer; a hysteresis-loop model is compared against a single force–velocity spline.
- **Reported use:** 3-DOF handling and 7-DOF ride vehicle models, including dampers at different mileages.
- **RealCars implication:** preserve loop direction and state history if measured data show material hysteresis.

### Nonlinear physical damper model

- **Evidence class:** B/F.
- **Reference lead:** *A universal nonlinear model for dynamic behaviour of shock absorbers* (2022), covering monotube/piggyback pressure, shim-stack and hysteretic behavior with bench validation.
- **Use:** later detailed component model; acquire and verify the primary full text before implementation.

### Minimum damper test metadata

- piston velocity and displacement,
- command waveform, amplitude and frequency,
- force sign and sensor location,
- gas pressure and reservoir configuration,
- fluid and body temperature,
- mounting orientation,
- bleed and adjuster settings,
- warm-up procedure,
- sample rate and filtering,
- cavitation/aeration observations,
- unit age, mileage and wear state.

## Bushings and compliant joints

A general bushing representation may require six coupled force/moment components as functions of relative translation, rotation, rate, amplitude, frequency, preload and temperature.

### Hydraulic-bushing nonlinear compliance

- **Evidence class:** B.
- **Source:** [SAE 2015-01-2355](https://saemobilus.sae.org/articles/dynamic-analysis-hydraulic-bushings-measured-nonlinear-compliance-parameters-2015-01-2355)
- **Scope:** step-sine and dedicated bench experiments, amplitude-sensitive and frequency-dependent hydraulic-bushing behavior, and reduced-order nonlinear compliance modeling.

### Bushing stiffness and full-vehicle correlation

- **Evidence class:** B/C.
- **Sources:**
  - [SAE 2010-01-0721](https://saemobilus.sae.org/articles/optimization-suspension-elastomeric-bushing-compliance-constraints-handling-ride-durability-2010-01-0721)
  - [SAE 2026-26-0094](https://saemobilus.sae.org/papers/influence-bushing-stiffness-measurement-methodology-axle-elasto-kinematics-2026-26-0094)
- **Use:** define directional static/dynamic tests and avoid treating a bushing as a single scalar spring.

## Air, active and semi-active suspension

### Air-spring modeling

- **Evidence class:** B/F.
- **Source lead:** [SAE 2013-26-0157 — air-suspension modeling](https://saemobilus.sae.org/search/?qt=2013-26-0157)
- **Reported approach:** measured load–displacement curves at different pressures, leveling/control behavior and full suspension simulation.
- **Use:** future optional powertrain-independent pneumatic state model.

### Skyhook and semi-active control

- **Evidence class:** C.
- **Foundational citation:** D. Karnopp, M. J. Crosby and R. A. Harwood, “Vibration Control Using Semi-Active Force Generators,” 1974, DOI `10.1115/1.3438373`.
- **Use:** source for the semi-active/skyhook concept and control-energy distinction.

### Magnetorheological damper model

- **Evidence class:** B/C.
- **Source:** [Spencer et al., Phenomenological Model for Magnetorheological Dampers](https://doi.org/10.1061/%28ASCE%290733-9399%281997%29123%3A3%28230%29)
- **Scope:** nonlinear phenomenological model compared with prototype-damper experiments over a range of operating conditions.
- **Use:** future semi-active component research, not part of the initial passive suspension milestone.

### Inerter

- **Evidence class:** C.
- **Foundational citation:** Malcolm C. Smith, “Synthesis of Mechanical Networks: The Inerter,” IEEE Transactions on Automatic Control 47(10), 2002, DOI `10.1109/TAC.2002.803532`.
- **Use:** optional passive-network research only after conventional spring/damper validation.

## Open measured and generated datasets

### ISO 8608 suspension benchmark dataset

- **Evidence class:** A/B depending on interpretation of the generated signals.
- **Source:** [Zenodo — Benchmark vehicle suspension data based on ISO 8608 road profiles](https://zenodo.org/records/17232645/latest)
- **DOI:** `10.5281/zenodo.17232645` for version 2; check the concept DOI/latest version before import.
- **Files:**
  - approximately 40 MB MATLAB suspension dataset,
  - body and tire displacements,
  - ISO 8608 classes A–E,
  - plotting script,
  - state-space subspace identification script,
  - Hammerstein–Wiener identification script.
- **RealCars use:** parser, system-identification and transfer-response fixture for a reduced-order suspension model.
- **Required review:** inspect `readme.pdf`, license, parameter provenance and whether outputs are experimental or generated before treating them as physical validation data.

### Quanser active-suspension measurements

- **Evidence class:** A.
- **Source:** [Mendeley Data — Acceleration Dataset](https://data.mendeley.com/datasets/v886n2rwp4/1)
- **DOI:** `10.17632/v886n2rwp4.1`.
- **License:** CC BY 4.0.
- **Platform:** Quanser active-suspension test rig.
- **Inputs:** step, stairs, sine, variable-frequency and bump disturbances.
- **Controllers:** passive, PID, fuzzy logic and hybrid FPID.
- **Channels:** body and tire position, body velocity and acceleration, actuator force and vertical displacement error.
- **RealCars use:** excellent regression dataset for reduced-order passive/active models and controller-independent channel conventions.
- **Caveat:** laboratory rig parameters and scaling must be documented before comparison.

## Road-profile and excitation sources

### FHWA Long-Term Pavement Performance profiles

- **Evidence class:** A.
- **Sources:**
  - [LTPP AIMS storage details](https://www.fhwa.dot.gov/publications/research/infrastructure/pavements/ltpp/12058/005.cfm)
  - [LTPP profile-data inventory](https://www.fhwa.dot.gov/publications/research/infrastructure/pavements/ltpp/15049/009.cfm)
  - [LTPP Profile Variability report](https://www.fhwa.dot.gov/publications/research/infrastructure/pavements/ltpp/00113/)
- **Data:** measured longitudinal profiles from high-speed profilers and dipstick surveys, including left/right/center paths and ERD files where available.
- **Quality note:** the variability report documents spikes, lost lock, shifted starts, wrong locations and other acquisition problems; profile files require quality filtering.
- **RealCars use:** measured road inputs for ride, wheel-hop and convergence tests.

### ASAM OpenCRG

- **Evidence class:** D/A depending on the individual file.
- **Source:** [ASAM OpenCRG](https://github.com/asam-ev/OpenCRG)
- **License:** Apache-2.0 for the software.
- **Use:** high-resolution two-dimensional road surfaces, kerbs, cobbles, proving-ground features and spatial friction fields.
- **Caveat:** every sample surface needs separate provenance and license review.

## Open reference implementations

### Project Chrono

- **Evidence class:** D.
- **License:** BSD-3-Clause.
- **Strengths:** C++, explicit multibody constraints, many suspension topologies, hardpoint-driven file models, full vehicle examples, tires and terrain integration.
- **Potential RealCars use:** independent oracle in offline tests and source-schema comparison.
- **Do not:** copy fixture values as real vehicle parameters or make Chrono a hidden validation authority without test correlation.

### Simbody

- **Evidence class:** D.
- **Source:** [simbody/simbody](https://github.com/simbody/simbody)
- **License:** Apache-2.0.
- **Capabilities:** articulated mechanical systems, constraints, generalized coordinates, forces, integrators, assemblers and O(n) multibody dynamics.
- **Use:** general multibody algorithm and constraint reference; less vehicle-specific than Chrono.

### Modelica Standard Library and VehicleInterfaces

- **Evidence class:** D.
- **Sources:**
  - [Modelica Standard Library](https://github.com/modelica/ModelicaStandardLibrary)
  - [VehicleInterfaces](https://github.com/modelica/VehicleInterfaces)
- **Licenses:** BSD-3-Clause for current repositories.
- **Capabilities:** mechanical 1D/3D components, force elements, joints and standardized automotive subsystem interfaces.
- **Use:** equation-oriented reference for component composition, energy accounting and subsystem interfaces.
- **Caveat:** current VehicleInterfaces repository licensing should be used rather than relying on older generated documentation that may describe the legacy Modelica License.

### MathWorks Simscape Multibody and Vehicle Dynamics Blockset

- **Evidence class:** D/E.
- **Sources:**
  - [Simscape Multibody examples](https://www.mathworks.com/help/sm/examples.html)
  - [Vehicle Suspension System Templates](https://www.mathworks.com/help/sm/ug/independent-suspension-system-templates.html)
  - [K&C Virtual Test Laboratory](https://www.mathworks.com/help/vdynblks/ug/kinematics-and-compliance-virtual-test-laboratory.html)
- **Examples include:** full vehicle on a four-post rig, double wishbone/MacPherson/pushrod suspensions and mapped K&C generation.
- **Use:** independent comparison and validation workflow design.
- **Reuse status:** commercial license required.

## Commercial and industrial workflow references

These tools are not proposed as dependencies. Their public documentation is useful for identifying accepted data and validation workflows.

- **SuspensionSim / VehicleSim:** quasi-static K&C simulation, suspension data sets and export to CarSim/TruckSim/BikeSim.
- **CarSim:** mapped suspension characteristics, K&C-derived toe/camber/jounce behavior and full-vehicle maneuvers.
- **MSC Adams/Car:** widely cited multibody vehicle and K&C correlation workflow.
- **Siemens Simcenter 3D Motion / Simcenter Amesim:** rigid/flexible multibody, controls and component/subsystem models.
- **Abaqus:** nonlinear finite-element K&C where component and body flexibility matter.
- **K&C and component laboratories:** AB Dynamics, MIRA/Horiba, Morse Measurements, Smithers and equivalent accredited facilities are potential sources of measured suspension, mass-property and component data.

Any purchased or commissioned data must be stored according to its contract; it may not be suitable for a public repository.

## Full-vehicle and rig validation methods

### Four-post and N-post tests

Validate:

- bounce, pitch and roll modes,
- wheel-hop frequencies,
- suspension travel and stop engagement,
- dynamic wheel loads,
- cross-axle phase response,
- damping and transient decay.

Reference examples include MathWorks' full-vehicle four-post model and industrial N-post road-load systems.

### K&C tests

Validate separately:

- parallel and opposite wheel travel,
- body heave, roll and pitch,
- lateral and longitudinal compliance,
- aligning torque and steering compliance,
- camber, toe, caster, wheel-center and track changes,
- ride rate and roll stiffness.

### Component tests

- spring and stop force–displacement,
- damper force–velocity/displacement loops over frequency and temperature,
- six-axis bushing static and dynamic stiffness,
- anti-roll-bar torque–angle,
- joint friction and breakaway,
- unsprung component mass, center of gravity and inertia.

### Full-vehicle tests

After tire and steering integration:

- static equilibrium and corner weights,
- free decay,
- single bump and paired bump,
- sinusoidal sweep and random road,
- kerb/cleat crossing,
- steady-state circle,
- step steer and sine steer,
- double lane change,
- braking and acceleration with pitch response,
- split-friction braking,
- lift-off and combined braking/cornering.

## Required suspension dataset schema

Before accepting any suspension data, record at least:

- source, authors/organization, URL/DOI and retrieval date,
- license, redistribution and commercial-use rights,
- vehicle, axle, side and suspension topology,
- coordinate system, origin, axis orientation and sign conventions,
- complete hardpoint definitions and measurement datum,
- wheel/rim/tire geometry used during testing,
- sprung and unsprung masses, centers of gravity and inertia tensors,
- static corner load and ride height,
- spring, stop, damper, anti-roll-bar and bushing identifiers,
- preload, installed length and motion ratios,
- steering state, rack position and brake state,
- K&C input definitions and load application points,
- temperature and warm-up state,
- sample rate, filtering and synchronization,
- sensor types, calibration and uncertainty,
- raw versus processed/fitted status,
- valid input domain and extrapolation policy,
- file hash and immutable source-file location.

## Public-data gaps

No unrestricted production-passenger-car source was found that combines all of the following:

- exact hardpoints and component inertias,
- six-axis nonlinear bushings,
- complete spring/stop/damper curves over temperature and frequency,
- anti-roll-bar geometry and stiffness,
- steering-system compliance,
- measured full-vehicle K&C tables,
- four-post or road-load time histories,
- matching tire vertical and force-and-moment data,
- documented sensor uncertainty,
- rights suitable for public redistribution and commercial game development.

The weakest public areas are production damper dyno loops, six-axis bushing maps, complete K&C tables and matching full-vehicle test data.

## Recommended next work

1. Download and review the licenses/readmes of the Zenodo and Mendeley suspension datasets; record hashes without importing data until approved.
2. Define RealCars suspension coordinate, hardpoint and K&C-table schemas before implementing equations.
3. Build an analytical quarter-car laboratory with closed-form/reference transfer functions and deterministic road inputs.
4. Add a 7-DOF ride reference model before explicit suspension geometry.
5. Build a standalone suspension-corner kinematics rig producing camber, toe, wheel-center and motion-ratio sweeps.
6. Compare the same generic hardpoint fixture against Project Chrono or another independent multibody implementation.
7. Add component interfaces for spring, stop, damper, anti-roll bar and bushing without selecting undocumented default values.
8. Select a legally obtainable real suspension or measured K&C target before fitting production parameters.
9. Add compliance only after rigid kinematics and coordinate/sign conventions pass regression tests.
10. Keep optional active, semi-active and high-frequency NVH models outside the initial passive-suspension milestone.

## Current decision

This register is a research inventory. It does not select a suspension topology, numerical integration method, bushing formulation, damper model, road profile, target vehicle or validation tolerance. No numerical parameter from any listed fixture may be copied into production data without a dedicated source and license review.
