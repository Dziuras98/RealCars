# Coupled contact-patch brush tire model

## Scope

`BrushTireModel` is a steady-state, one-dimensional contact-patch model for the tire-laboratory milestone. It now calculates longitudinal force, lateral force and self-aligning moment from one distributed brush solution rather than calculating two independent pure-slip forces and projecting their result onto a global friction ellipse.

The model covers:

- pure longitudinal and lateral slip,
- locally coupled longitudinal and lateral brush deformation,
- load-sensitive friction and stiffness,
- anisotropic longitudinal and lateral stiffness and friction limits,
- camber thrust represented as equivalent lateral brush deformation,
- self-aligning moment obtained by integrating lateral shear stress about the patch centre.

It does not yet cover relaxation length, transient carcass deformation, temperature, pressure, wear, speed-sensitive friction, rolling resistance, turn slip, residual aligning torque or road enveloping.

## Contact-patch coordinate and pressure

The contact patch is represented by one brush row of length `l = 2a`, where `a` is `contact_patch_half_length_m`. The coordinate `xi` starts at the leading edge and ends at the trailing edge.

A normalized parabolic line-pressure distribution is used:

```text
p(xi) = 6 Fz xi (l - xi) / l^3,
0 <= xi <= l.
```

It satisfies `integral(p dxi) = Fz`. Parabolic longitudinal pressure is a standard assumption in classical brush models and has also been found to approximate measured footprint pressure away from localized edge and groove effects.

The implementation evaluates the integral with 256 midpoint segments. This is a numerical resolution, not a physical tire coefficient. The discretized pressure is normalized again so its numerical integral is exactly `Fz`.

## Bristle deformation

In the adhesion solution, bristle displacement grows with distance travelled through the contact patch. Distributed stiffness is selected so the integrated small-slip gradients equal the configured tire stiffnesses:

```text
cx = 2 Cx / l^2
cy = 2 Cy / l^2

qx,trial(xi) =  cx kappa xi
qy,trial(xi) = -cy sy xi
```

The lateral brush input is

```text
sy = tan(alpha) + (Cgamma / Cy) gamma.
```

The second term is an explicit approximation: camber is converted to the equivalent lateral deflection that preserves the configured small-angle camber stiffness. Camber then consumes the same local friction capacity as slip-angle deformation.

## Local combined-slip saturation

At every contact-patch segment, the trial shear stress is normalized by the local friction capacities:

```text
qx_limit(xi) = mu(Fz) p(xi) longitudinal_friction_scale
qy_limit(xi) = mu(Fz) p(xi) lateral_friction_scale

u(xi) = sqrt((qx,trial/qx_limit)^2 + (qy,trial/qy_limit)^2).
```

When `u <= 1`, the bristle remains in adhesion. When `u > 1`, both shear components are divided by `u`. Thus adhesion and sliding regions emerge locally along the patch. Longitudinal slip can reduce lateral force, and lateral slip can reduce longitudinal force, before the integrated force reaches a global limit.

This replaces the former procedure that calculated pure `Fx` and `Fy` independently and scaled them only after their global ellipse utilization exceeded one.

## Integrated forces and moment

The tire outputs are numerical integrals over the same shear distribution:

```text
Fx = integral(qx dxi)
Fy = integral(qy dxi)
Mz = integral((a - xi) qy dxi).
```

The leading edge is at longitudinal position `+a`; the trailing edge is at `-a`. Under the RealCars sign convention, positive slip angle produces negative `Fy` and positive restoring `Mz`.

For a fully adhered brush at small lateral slip, the model produces pneumatic trail `a/3`. As sliding spreads through the patch, lateral shear becomes increasingly symmetric about the centre and `Mz` naturally approaches zero. No separate trail-falloff curve is applied.

## Pure-slip compatibility

With a parabolic pressure distribution and constant friction, integrating one active shear component reproduces the existing Fiala polynomial within numerical integration tolerance:

```text
F(s) = C s - C^2 |s| s / (3 F_limit) + C^3 s^3 / (27 F_limit^2)
```

up to `|s| = 3 F_limit / C`, followed by saturation at `F_limit`. Automated tests compare the numerical solution to this analytical result at multiple loads and slips.

## Load sensitivity

The existing provisional load laws remain unchanged:

```text
mu(Fz) = mu_ref (Fz / Fz_ref)^(-load_sensitivity)
C(Fz)  = C_ref  (Fz / Fz_ref)^(stiffness_load_exponent).
```

No coefficient was changed or fitted in the coupled-model PR.

## Legacy parameter compatibility

`pneumatic_trail_fraction` and `trail_falloff_exponent` remain readable temporarily so existing parameter files and tire-rig metadata do not break. They no longer affect `Fx`, `Fy` or `Mz`. Automated tests enforce this. A later schema-cleanup PR should remove them from the public parameter structure, configuration file and telemetry metadata together.

## Sources

- J. Svendenius and M. Gäfvert, *A Brush-Model Based Semi-Empirical Tire-Model for Combined Slips*, SAE 2004-01-1064, DOI `10.4271/2004-01-1064`. This establishes combined-slip force and aligning-torque construction from adhesion and sliding regions of a brush contact patch.
- L. Romano, F. Bruzelius and B. Jacobson, *Unsteady-state brush theory*, Vehicle System Dynamics 59(11), 2021, DOI `10.1080/00423114.2020.1774625`. The paper derives brush contact equations and explicitly uses a rectangular patch with parabolic pressure distribution.
- A. O'Neill et al., *Enhancing brush tyre model accuracy through friction measurements*, Vehicle System Dynamics, DOI `10.1080/00423114.2021.1893766`. Measured footprint pressure is compared with the parabolic approximation used by common brush models.
- L. Romano et al., *Analytical results in transient brush tyre models: theory for large camber angles and classic solutions with limited friction*, Meccanica, DOI `10.1007/s11012-021-01422-3`. Forces and self-aligning moment are obtained by integration of the contact-patch shear distribution.

## Parameter status

`data/tires/reference_sport_tire.cfg` still contains generic, uncalibrated development values. The coupled formulation is structurally more consistent, but the model must not be described as calibrated to the KIT measurement tire or to a production road tire.
