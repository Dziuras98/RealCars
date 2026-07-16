# Coordinate system and sign conventions

RealCars uses SI units throughout the simulation core.

## Vehicle coordinate system

The vehicle-fixed right-handed coordinate system is:

- `+X`: forward,
- `+Y`: left,
- `+Z`: upward.

Positive rotations follow the right-hand rule:

- roll is rotation about `+X`,
- pitch is rotation about `+Y`,
- yaw is rotation about `+Z`.

## Tire quantities

The tire models use these conventions:

- positive slip ratio represents driven-wheel overspeed,
- negative slip ratio represents braking slip,
- positive slip angle produces a lateral force in the `-Y` direction,
- positive camber angle follows a right-hand rotation about the wheel's forward axis,
- tire forces are forces exerted by the road on the tire,
- normal load is non-negative and expressed in newtons.

The aligning moment is returned as `Mz = -pneumatic_trail * Fy`. Positive slip angle therefore produces a restoring positive `Mz` under the current convention.

These conventions are part of the simulation API. Changes require an explicit migration and regression-test update.
