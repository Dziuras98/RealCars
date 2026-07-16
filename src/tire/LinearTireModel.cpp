#include "realcars/tire/LinearTireModel.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace realcars::tire {

namespace {

void validate_parameters(const LinearTireModelParameters& parameters) {
    if (parameters.longitudinal_stiffness_n < 0.0) {
        throw std::invalid_argument("Longitudinal stiffness must be non-negative");
    }
    if (parameters.cornering_stiffness_n_per_rad < 0.0) {
        throw std::invalid_argument("Cornering stiffness must be non-negative");
    }
    if (parameters.camber_stiffness_n_per_rad < 0.0) {
        throw std::invalid_argument("Camber stiffness must be non-negative");
    }
    if (parameters.friction_coefficient < 0.0) {
        throw std::invalid_argument("Friction coefficient must be non-negative");
    }
    if (parameters.pneumatic_trail_m < 0.0) {
        throw std::invalid_argument("Pneumatic trail must be non-negative");
    }
}

}  // namespace

LinearTireModel::LinearTireModel(LinearTireModelParameters parameters)
    : parameters_(parameters) {
    validate_parameters(parameters_);
}

TireForces LinearTireModel::evaluate(const TireState& state) const {
    const double normal_load_n = std::max(0.0, state.normal_load_n);
    if (normal_load_n == 0.0) {
        return {};
    }

    double longitudinal_force_n =
        parameters_.longitudinal_stiffness_n * state.slip_ratio;
    double lateral_force_n =
        -parameters_.cornering_stiffness_n_per_rad * state.slip_angle_rad
        - parameters_.camber_stiffness_n_per_rad * state.camber_angle_rad;

    const double friction_limit_n = parameters_.friction_coefficient * normal_load_n;
    const double combined_force_n = std::hypot(longitudinal_force_n, lateral_force_n);

    if (combined_force_n > friction_limit_n && combined_force_n > 0.0) {
        const double scale = friction_limit_n / combined_force_n;
        longitudinal_force_n *= scale;
        lateral_force_n *= scale;
    }

    return TireForces{
        .longitudinal_force_n = longitudinal_force_n,
        .lateral_force_n = lateral_force_n,
        .aligning_moment_nm = -parameters_.pneumatic_trail_m * lateral_force_n,
    };
}

const LinearTireModelParameters& LinearTireModel::parameters() const noexcept {
    return parameters_;
}

}  // namespace realcars::tire
