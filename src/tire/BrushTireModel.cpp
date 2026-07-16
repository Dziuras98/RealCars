#include "realcars/tire/BrushTireModel.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace realcars::tire {

namespace {

constexpr double kMinimumPositive = 1.0e-12;

void require_finite_positive(double value, const char* name) {
    if (!std::isfinite(value) || value <= 0.0) {
        throw std::invalid_argument(name);
    }
}

void require_finite_non_negative(double value, const char* name) {
    if (!std::isfinite(value) || value < 0.0) {
        throw std::invalid_argument(name);
    }
}

void validate_parameters(const BrushTireModelParameters& parameters) {
    require_finite_positive(parameters.reference_load_n, "Reference load must be finite and positive");
    require_finite_positive(
        parameters.friction_coefficient_at_reference_load,
        "Reference friction coefficient must be finite and positive");
    require_finite_non_negative(parameters.load_sensitivity, "Load sensitivity must be finite and non-negative");
    require_finite_positive(parameters.longitudinal_friction_scale, "Longitudinal friction scale must be finite and positive");
    require_finite_positive(parameters.lateral_friction_scale, "Lateral friction scale must be finite and positive");
    require_finite_positive(
        parameters.longitudinal_stiffness_at_reference_load_n,
        "Longitudinal stiffness must be finite and positive");
    require_finite_positive(
        parameters.cornering_stiffness_at_reference_load_n_per_rad,
        "Cornering stiffness must be finite and positive");
    require_finite_non_negative(
        parameters.camber_stiffness_at_reference_load_n_per_rad,
        "Camber stiffness must be finite and non-negative");
    require_finite_positive(parameters.stiffness_load_exponent, "Stiffness load exponent must be finite and positive");
    require_finite_positive(parameters.contact_patch_half_length_m, "Contact patch half-length must be finite and positive");
    require_finite_non_negative(parameters.pneumatic_trail_fraction, "Pneumatic trail fraction must be finite and non-negative");
    if (parameters.pneumatic_trail_fraction > 1.0) {
        throw std::invalid_argument("Pneumatic trail fraction must not exceed one");
    }
    require_finite_positive(parameters.trail_falloff_exponent, "Trail falloff exponent must be finite and positive");
}

[[nodiscard]] double fiala_axis_force(
    double slip,
    double stiffness,
    double force_limit,
    double direction_sign) {
    if (std::abs(slip) <= kMinimumPositive) {
        return 0.0;
    }

    const double transition_slip = 3.0 * force_limit / stiffness;
    if (std::abs(slip) >= transition_slip) {
        return direction_sign * force_limit * std::copysign(1.0, slip);
    }

    const double stiffness_squared = stiffness * stiffness;
    const double stiffness_cubed = stiffness_squared * stiffness;
    const double force_limit_squared = force_limit * force_limit;
    const double polynomial =
        stiffness * slip
        - (stiffness_squared / (3.0 * force_limit)) * std::abs(slip) * slip
        + (stiffness_cubed / (27.0 * force_limit_squared)) * slip * slip * slip;

    return direction_sign * polynomial;
}

}  // namespace

BrushTireModel::BrushTireModel(BrushTireModelParameters parameters)
    : parameters_(parameters) {
    validate_parameters(parameters_);
}

double BrushTireModel::friction_coefficient(double normal_load_n) const noexcept {
    if (!std::isfinite(normal_load_n) || normal_load_n <= 0.0) {
        return 0.0;
    }

    const double load_ratio = normal_load_n / parameters_.reference_load_n;
    return parameters_.friction_coefficient_at_reference_load
        * std::pow(load_ratio, -parameters_.load_sensitivity);
}

TireForces BrushTireModel::evaluate(const TireState& state) const {
    if (!std::isfinite(state.normal_load_n) || state.normal_load_n <= 0.0) {
        return {};
    }
    if (!std::isfinite(state.slip_ratio)
        || !std::isfinite(state.slip_angle_rad)
        || !std::isfinite(state.camber_angle_rad)) {
        return {};
    }

    const double normal_load_n = state.normal_load_n;
    const double load_ratio = normal_load_n / parameters_.reference_load_n;
    const double stiffness_scale = std::pow(load_ratio, parameters_.stiffness_load_exponent);
    const double friction = friction_coefficient(normal_load_n);

    const double longitudinal_limit_n =
        friction * normal_load_n * parameters_.longitudinal_friction_scale;
    const double lateral_limit_n =
        friction * normal_load_n * parameters_.lateral_friction_scale;

    const double longitudinal_stiffness_n =
        parameters_.longitudinal_stiffness_at_reference_load_n * stiffness_scale;
    const double cornering_stiffness_n_per_rad =
        parameters_.cornering_stiffness_at_reference_load_n_per_rad * stiffness_scale;
    const double camber_stiffness_n_per_rad =
        parameters_.camber_stiffness_at_reference_load_n_per_rad * stiffness_scale;

    double longitudinal_force_n = fiala_axis_force(
        state.slip_ratio,
        longitudinal_stiffness_n,
        longitudinal_limit_n,
        1.0);

    const double lateral_slip = std::tan(state.slip_angle_rad);
    double lateral_force_n = fiala_axis_force(
        lateral_slip,
        cornering_stiffness_n_per_rad,
        lateral_limit_n,
        -1.0);
    lateral_force_n -= camber_stiffness_n_per_rad * state.camber_angle_rad;

    const double normalized_longitudinal = longitudinal_force_n / longitudinal_limit_n;
    const double normalized_lateral = lateral_force_n / lateral_limit_n;
    const double combined_utilization = std::hypot(normalized_longitudinal, normalized_lateral);

    if (combined_utilization > 1.0) {
        const double scale = 1.0 / combined_utilization;
        longitudinal_force_n *= scale;
        lateral_force_n *= scale;
    }

    const double lateral_utilization = std::clamp(
        std::abs(lateral_force_n) / lateral_limit_n,
        0.0,
        1.0);
    const double maximum_trail_m =
        parameters_.contact_patch_half_length_m * parameters_.pneumatic_trail_fraction;
    const double pneumatic_trail_m = maximum_trail_m
        * std::pow(1.0 - lateral_utilization, parameters_.trail_falloff_exponent);

    return TireForces{
        .longitudinal_force_n = longitudinal_force_n,
        .lateral_force_n = lateral_force_n,
        .aligning_moment_nm = -pneumatic_trail_m * lateral_force_n,
    };
}

const BrushTireModelParameters& BrushTireModel::parameters() const noexcept {
    return parameters_;
}

}  // namespace realcars::tire
