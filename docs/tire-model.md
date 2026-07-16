# Brush/Fiala tire model

## Scope

`BrushTireModel` is a steady-state, lumped-contact-patch model intended for the first tire-laboratory milestone. It is more physically structured than the bootstrap linear model, but it is not yet a calibrated production-tire model.

The implementation currently covers:

- pure longitudinal slip,
- pure lateral slip,
- combined-slip friction ellipse,
- load-dependent friction coefficient,
- load-dependent stiffness,
- linear camber thrust followed by friction limiting,
- a pneumatic-trail approximation and aligning moment.

It does not yet cover relaxation length, transient carcass deformation, temperature, pressure, wear, speed sensitivity, rolling resistance, turn slip or enveloping over road geometry.

## Pure-axis force

For either longitudinal slip `s = kappa` or lateral slip `s = tan(alpha)`, the unsaturated Fiala polynomial is

```text
F(s) = C s - C^2 |s| s / (3 F_limit) + C^3 s^3 / (27 F_limit^2)
```

up to the transition

```text
|s| = 3 F_limit / C.
```

Above the transition, force magnitude is clamped to `F_limit`. Lateral force receives the opposite sign so that it opposes the slip angle under the project coordinate convention.

## Load sensitivity

The effective friction coefficient is

```text
mu(Fz) = mu_ref * (Fz / Fz_ref)^(-load_sensitivity).
```

A positive load-sensitivity value makes normalized peak force `F/Fz` decrease as vertical load rises. Stiffnesses are scaled separately using

```text
C(Fz) = C_ref * (Fz / Fz_ref)^(stiffness_load_exponent).
```

## Combined slip

Pure longitudinal and lateral forces are calculated first. They are then normalized by their respective limits. When

```text
sqrt((Fx/Fx_limit)^2 + (Fy/Fy_limit)^2) > 1,
```

both components are scaled back onto the ellipse. This preserves force direction but is still a simplified coupling model.

## Aligning moment

Maximum pneumatic trail is a configurable fraction of contact-patch half-length. Trail decreases with lateral-force utilization and reaches zero at saturation:

```text
trail = trail_max * (1 - |Fy|/Fy_limit)^(trail_falloff_exponent)
Mz = -trail * Fy
```

This reproduces the qualitative rise and fall of self-aligning torque. It is not a full brush-model pressure-distribution solution.

## Parameter status

`data/tires/reference_sport_tire.cfg` contains generic development values only. They are intentionally labeled as uncalibrated. No claim is made that they represent a particular road tire.
