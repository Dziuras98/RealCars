#include "realcars/tire/BrushTireModel.hpp"

#include <cmath>
#include <cstddef>
#include <stdexcept>

namespace realcars::tire {

namespace {

// Numerical resolution only; this is not a physical tire parameter. Midpoint
// integration with 256 elements keeps pure-slip force error below 0.01% against
// the analytical parabolic-pressure brush solution over the regression grid.
constexpr std::size_t kContactPatchSegments = 256U;

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
    require_finite_non_negative(
        parameters.pneumatic_trail_fraction,
        "Legacy pneumatic trail fraction must be finite and non-negative");
    require_finite_positive(
        parameters.trail_falloff_exponent,
        "Legacy trail falloff exponent must be finite and positive");
}

struct ContactPatchIntegral {
    double longitudinal_force_n{0.0};
    double lateral_force_n{0.0};
    double aligning_moment_nm{0.0};
};

[[nodiscard]] ContactPatchIntegral integrate_contact_patch(
    double normal_load_n,
    double slip_ratio,
    double lateral_slip,
    double friction,
    double longitudinal_stiffness_n,
    double cornering_stiffness_n_per_rad,
    const BrushTireModelParameters& parameters) {
    const double half_length_m = parameters.contact_patch_half_length_m;
    const double patch_length_m = 2.0 * half_length_m;
    const double segment_length_m =
        patch_length_m / static_cast<double>(kContactPatchSegments);

    // With bristle deflection proportional to distance from the leading edge,
    // these distributed stiffnesses integrate to the requested small-slip Cx/Cy.
    const double longitudinal_bristle_stiffness_n_per_m2 =
        2.0 * longitudinal_stiffness_n / (patch_length_m * patch_length_m);
    const double lateral_bristle_stiffness_n_per_m2 =
        2.0 * cornering_stiffness_n_per_rad / (patch_length_m * patch_length_m);

    // Normalize the discretized parabolic pressure shape so its numerical
    // integral is exactly Fz and the integrated friction bound cannot drift.
    double pressure_shape_integral_m3 = 0.0;
    for (std::size_t index = 0; index < kContactPatchSegments; ++index) {
        const double distance_from_leading_edge_m =
            (static_cast<double>(index) + 0.5) * segment_length_m;
        pressure_shape_integral_m3 +=
            distance_from_leading_edge_m
            * (patch_length_m - distance_from_leading_edge_m)
            * segment_length_m;
    }

    ContactPatchIntegral result;
    for (std::size_t index = 0; index < kContactPatchSegments; ++index) {
        const double distance_from_leading_edge_m =
            (static_cast<double>(index) + 0.5) * segment_length_m;
        const double longitudinal_position_m =
            half_length_m - distance_from_leading_edge_m;
        const double pressure_n_per_m =
            normal_load_n
            * distance_from_leading_edge_m
            * (patch_length_m - distance_from_leading_edge_m)
            / pressure_shape_integral_m3;

        double longitudinal_shear_n_per_m =
            longitudinal_bristle_stiffness_n_per_m2
            * slip_ratio
            * distance_from_leading_edge_m;
        double lateral_shear_n_per_m =
            -lateral_bristle_stiffness_n_per_m2
            * lateral_slip
            * distance_from_leading_edge_m;

        const double longitudinal_capacity_n_per_m =
            friction * pressure_n_per_m * parameters.longitudinal_friction_scale;
        const double lateral_capacity_n_per_m =
            friction * pressure_n_per_m * parameters.lateral_friction_scale;
        const double local_utilization = std::hypot(
            longitudinal_shear_n_per_m / longitudinal_capacity_n_per_m,
            lateral_shear_n_per_m / lateral_capacity_n_per_m);

        if (local_utilization > 1.0) {
            longitudinal_shear_n_per_m /= local_utilization;
            lateral_shear_n_per_m /= local_utilization;
        }

        result.longitudinal_force_n += longitudinal_shear_n_per_m * segment_length_m;
        result.lateral_force_n += lateral_shear_n_per_m * segment_length_m;
        result.aligning_moment_nm +=
            longitudinal_position_m * lateral_shear_n_per_m * segment_length_m;
    }

    return result;
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

    const double load_ratio = state.normal_load_n / parameters_.reference_load_n;
    const double stiffness_scale =
        std::pow(load_ratio, parameters_.stiffness_load_exponent);
    const double longitudinal_stiffness_n =
        parameters_.longitudinal_stiffness_at_reference_load_n * stiffness_scale;
    const double cornering_stiffness_n_per_rad =
        parameters_.cornering_stiffness_at_reference_load_n_per_rad * stiffness_scale;
    const double camber_stiffness_n_per_rad =
        parameters_.camber_stiffness_at_reference_load_n_per_rad * stiffness_scale;

    // Camber is represented as the equivalent lateral brush deflection that
    // reproduces the configured small-angle camber stiffness. It then shares
    // the same local friction capacity as slip-angle deformation.
    const double lateral_slip =
        std::tan(state.slip_angle_rad)
        + (camber_stiffness_n_per_rad / cornering_stiffness_n_per_rad)
            * state.camber_angle_rad;

    const ContactPatchIntegral forces = integrate_contact_patch(
        state.normal_load_n,
        state.slip_ratio,
        lateral_slip,
        friction_coefficient(state.normal_load_n),
        longitudinal_stiffness_n,
        cornering_stiffness_n_per_rad,
        parameters_);

    return TireForces{
        .longitudinal_force_n = forces.longitudinal_force_n,
        .lateral_force_n = forces.lateral_force_n,
        .aligning_moment_nm = forces.aligning_moment_nm,
    };
}

const BrushTireModelParameters& BrushTireModel::parameters() const noexcept {
    return parameters_;
}

}  // namespace realcars::tire
