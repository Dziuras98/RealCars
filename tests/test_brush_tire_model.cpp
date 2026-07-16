#include "realcars/tire/BrushTireModel.hpp"
#include "realcars/tire/BrushTireModelParametersIO.hpp"

#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <numbers>
#include <stdexcept>
#include <string_view>

namespace {

constexpr double degrees_to_radians(double degrees) {
    return degrees * std::numbers::pi / 180.0;
}

bool approximately_equal(double left, double right, double tolerance) {
    return std::abs(left - right) <= tolerance;
}

int failures = 0;

void expect(bool condition, std::string_view message) {
    if (!condition) {
        std::cerr << "FAILED: " << message << '\n';
        ++failures;
    }
}

[[nodiscard]] double analytical_fiala_force(
    double slip,
    double stiffness,
    double force_limit) {
    if (std::abs(slip) <= 1.0e-12) {
        return 0.0;
    }
    const double transition_slip = 3.0 * force_limit / stiffness;
    if (std::abs(slip) >= transition_slip) {
        return force_limit * std::copysign(1.0, slip);
    }
    return stiffness * slip
        - (stiffness * stiffness / (3.0 * force_limit)) * std::abs(slip) * slip
        + (stiffness * stiffness * stiffness / (27.0 * force_limit * force_limit))
            * slip * slip * slip;
}

void zero_slip_produces_zero_forces() {
    const realcars::tire::BrushTireModel model;
    const auto forces = model.evaluate({.normal_load_n = 4'000.0});

    expect(approximately_equal(forces.longitudinal_force_n, 0.0, 1e-12), "zero Fx");
    expect(approximately_equal(forces.lateral_force_n, 0.0, 1e-12), "zero Fy");
    expect(approximately_equal(forces.aligning_moment_nm, 0.0, 1e-12), "zero Mz");
}

void force_signs_and_symmetry_are_consistent() {
    const realcars::tire::BrushTireModel model;
    const auto positive = model.evaluate({
        .normal_load_n = 4'000.0,
        .slip_ratio = 0.05,
        .slip_angle_rad = degrees_to_radians(3.0),
    });
    const auto negative = model.evaluate({
        .normal_load_n = 4'000.0,
        .slip_ratio = -0.05,
        .slip_angle_rad = degrees_to_radians(-3.0),
    });

    expect(positive.longitudinal_force_n > 0.0, "positive slip ratio produces positive Fx");
    expect(positive.lateral_force_n < 0.0, "positive slip angle produces opposing Fy");
    expect(positive.aligning_moment_nm > 0.0, "positive slip angle produces restoring Mz");
    expect(approximately_equal(positive.longitudinal_force_n, -negative.longitudinal_force_n, 1e-9), "Fx is odd");
    expect(approximately_equal(positive.lateral_force_n, -negative.lateral_force_n, 1e-9), "Fy is odd");
    expect(approximately_equal(positive.aligning_moment_nm, -negative.aligning_moment_nm, 1e-9), "Mz is odd");
}

void small_slip_matches_requested_stiffness() {
    const realcars::tire::BrushTireModelParameters parameters;
    const realcars::tire::BrushTireModel model{parameters};
    constexpr double small_input = 1.0e-5;

    const auto longitudinal = model.evaluate({
        .normal_load_n = parameters.reference_load_n,
        .slip_ratio = small_input,
    });
    const auto lateral = model.evaluate({
        .normal_load_n = parameters.reference_load_n,
        .slip_angle_rad = small_input,
    });
    const auto camber = model.evaluate({
        .normal_load_n = parameters.reference_load_n,
        .camber_angle_rad = small_input,
    });

    expect(
        std::abs(
            longitudinal.longitudinal_force_n / small_input
            / parameters.longitudinal_stiffness_at_reference_load_n - 1.0) < 1.0e-9,
        "small-slip longitudinal gradient matches parameter");
    expect(
        std::abs(
            -lateral.lateral_force_n / small_input
            / parameters.cornering_stiffness_at_reference_load_n_per_rad - 1.0) < 1.0e-9,
        "small-slip lateral gradient matches parameter");
    expect(
        std::abs(
            -camber.lateral_force_n / small_input
            / parameters.camber_stiffness_at_reference_load_n_per_rad - 1.0) < 1.0e-9,
        "small-camber gradient matches parameter");
}

void pure_slip_matches_parabolic_pressure_fiala_solution() {
    const realcars::tire::BrushTireModel model;
    const auto& parameters = model.parameters();

    for (const double load : {2'000.0, 4'000.0, 8'000.0}) {
        const double stiffness_scale =
            std::pow(load / parameters.reference_load_n, parameters.stiffness_load_exponent);
        const double friction = model.friction_coefficient(load);
        const double longitudinal_limit =
            friction * load * parameters.longitudinal_friction_scale;
        const double lateral_limit =
            friction * load * parameters.lateral_friction_scale;
        const double longitudinal_stiffness =
            parameters.longitudinal_stiffness_at_reference_load_n * stiffness_scale;
        const double lateral_stiffness =
            parameters.cornering_stiffness_at_reference_load_n_per_rad * stiffness_scale;

        for (const double slip_ratio : {-0.3, -0.1, -0.02, 0.02, 0.1, 0.3}) {
            const auto forces = model.evaluate({
                .normal_load_n = load,
                .slip_ratio = slip_ratio,
            });
            const double expected = analytical_fiala_force(
                slip_ratio,
                longitudinal_stiffness,
                longitudinal_limit);
            expect(
                std::abs(forces.longitudinal_force_n - expected)
                    <= 1.0e-4 * longitudinal_limit,
                "pure longitudinal numerical integration matches analytical brush force");
        }

        for (const double slip_angle_deg : {-15.0, -5.0, -1.0, 1.0, 5.0, 15.0}) {
            const double lateral_slip = std::tan(degrees_to_radians(slip_angle_deg));
            const auto forces = model.evaluate({
                .normal_load_n = load,
                .slip_angle_rad = degrees_to_radians(slip_angle_deg),
            });
            const double expected = -analytical_fiala_force(
                lateral_slip,
                lateral_stiffness,
                lateral_limit);
            expect(
                std::abs(forces.lateral_force_n - expected) <= 1.0e-4 * lateral_limit,
                "pure lateral numerical integration matches analytical brush force");
        }
    }
}

void local_combined_slip_couples_force_generation() {
    const realcars::tire::BrushTireModel model;
    const auto pure_longitudinal = model.evaluate({
        .normal_load_n = 4'000.0,
        .slip_ratio = 0.05,
    });
    const auto pure_lateral = model.evaluate({
        .normal_load_n = 4'000.0,
        .slip_angle_rad = degrees_to_radians(3.0),
    });
    const auto combined = model.evaluate({
        .normal_load_n = 4'000.0,
        .slip_ratio = 0.05,
        .slip_angle_rad = degrees_to_radians(3.0),
    });

    expect(
        std::abs(combined.longitudinal_force_n)
            < std::abs(pure_longitudinal.longitudinal_force_n),
        "lateral deformation reduces longitudinal force before global saturation");
    expect(
        std::abs(combined.lateral_force_n) < std::abs(pure_lateral.lateral_force_n),
        "longitudinal deformation reduces lateral force before global saturation");
}

void combined_forces_remain_inside_friction_ellipse() {
    const realcars::tire::BrushTireModel model;
    const auto& parameters = model.parameters();

    for (const double load : {1'000.0, 2'000.0, 4'000.0, 8'000.0}) {
        const double friction = model.friction_coefficient(load);
        const double fx_limit = friction * load * parameters.longitudinal_friction_scale;
        const double fy_limit = friction * load * parameters.lateral_friction_scale;
        for (const double slip_ratio : {-0.4, -0.1, 0.0, 0.1, 0.4}) {
            for (const double slip_angle_deg : {-20.0, -5.0, 0.0, 5.0, 20.0}) {
                const auto forces = model.evaluate({
                    .normal_load_n = load,
                    .slip_ratio = slip_ratio,
                    .slip_angle_rad = degrees_to_radians(slip_angle_deg),
                    .camber_angle_rad = degrees_to_radians(2.0),
                });
                const double utilization = std::hypot(
                    forces.longitudinal_force_n / fx_limit,
                    forces.lateral_force_n / fy_limit);
                expect(utilization <= 1.0 + 1.0e-12, "integrated force remains inside ellipse");
            }
        }
    }
}

void load_sensitivity_reduces_normalized_peak_force() {
    const realcars::tire::BrushTireModel model;
    const auto low_load = model.evaluate({.normal_load_n = 2'000.0, .slip_ratio = 1.0});
    const auto high_load = model.evaluate({.normal_load_n = 8'000.0, .slip_ratio = 1.0});

    expect(
        low_load.longitudinal_force_n / 2'000.0
            > high_load.longitudinal_force_n / 8'000.0,
        "normalized peak force decreases with load");
}

void camber_thrust_is_bounded_and_has_restoring_sign() {
    const realcars::tire::BrushTireModel model;
    const auto forces = model.evaluate({
        .normal_load_n = 4'000.0,
        .camber_angle_rad = degrees_to_radians(3.0),
    });

    expect(forces.lateral_force_n < 0.0, "positive camber produces negative lateral force");
    expect(
        std::abs(forces.lateral_force_n)
            <= model.friction_coefficient(4'000.0) * 4'000.0,
        "camber force remains friction bounded");
}

void aligning_moment_comes_from_contact_patch_distribution() {
    const realcars::tire::BrushTireModel model;
    const auto& parameters = model.parameters();
    const auto adhered = model.evaluate({
        .normal_load_n = 4'000.0,
        .slip_angle_rad = 1.0e-5,
    });
    const double pneumatic_trail_m =
        -adhered.aligning_moment_nm / adhered.lateral_force_n;
    expect(
        std::abs(pneumatic_trail_m - parameters.contact_patch_half_length_m / 3.0)
            < 1.0e-6,
        "fully adhered parabolic brush has one-third-half-length pneumatic trail");

    const auto moderate = model.evaluate({
        .normal_load_n = 4'000.0,
        .slip_angle_rad = degrees_to_radians(2.0),
    });
    const auto saturated = model.evaluate({
        .normal_load_n = 4'000.0,
        .slip_angle_rad = degrees_to_radians(30.0),
    });
    expect(std::abs(moderate.aligning_moment_nm) > 1.0, "moderate slip produces Mz");
    expect(
        std::abs(saturated.aligning_moment_nm) < std::abs(moderate.aligning_moment_nm),
        "symmetric sliding pressure drives Mz toward zero");
}

void legacy_trail_parameters_do_not_affect_coupled_solution() {
    auto first_parameters = realcars::tire::BrushTireModelParameters{};
    auto second_parameters = first_parameters;
    first_parameters.pneumatic_trail_fraction = 0.1;
    first_parameters.trail_falloff_exponent = 0.5;
    second_parameters.pneumatic_trail_fraction = 0.9;
    second_parameters.trail_falloff_exponent = 4.0;
    const realcars::tire::BrushTireModel first{first_parameters};
    const realcars::tire::BrushTireModel second{second_parameters};
    const realcars::tire::TireState state{
        .normal_load_n = 4'000.0,
        .slip_ratio = 0.08,
        .slip_angle_rad = degrees_to_radians(4.0),
    };
    const auto first_forces = first.evaluate(state);
    const auto second_forces = second.evaluate(state);

    expect(
        approximately_equal(first_forces.longitudinal_force_n, second_forces.longitudinal_force_n, 1e-12),
        "legacy trail fraction does not alter Fx");
    expect(
        approximately_equal(first_forces.lateral_force_n, second_forces.lateral_force_n, 1e-12),
        "legacy trail fraction does not alter Fy");
    expect(
        approximately_equal(first_forces.aligning_moment_nm, second_forces.aligning_moment_nm, 1e-12),
        "legacy trail parameters do not alter integrated Mz");
}

void invalid_parameters_are_rejected() {
    auto parameters = realcars::tire::BrushTireModelParameters{};
    parameters.reference_load_n = 0.0;
    bool rejected = false;
    try {
        static_cast<void>(realcars::tire::BrushTireModel{parameters});
    } catch (const std::invalid_argument&) {
        rejected = true;
    }
    expect(rejected, "invalid parameters are rejected");
}

void parameter_file_is_loaded_and_unknown_keys_are_rejected() {
    const auto valid_path = std::filesystem::temp_directory_path() / "realcars_brush_tire_valid.cfg";
    {
        std::ofstream output(valid_path);
        output << "friction_coefficient_at_reference_load = 1.37\n";
        output << "load_sensitivity = 0.11 # comment\n";
    }
    const auto loaded = realcars::tire::load_brush_tire_parameters(valid_path);
    expect(approximately_equal(loaded.friction_coefficient_at_reference_load, 1.37, 1e-12), "parameter file overrides friction");
    expect(approximately_equal(loaded.load_sensitivity, 0.11, 1e-12), "parameter file overrides load sensitivity");
    std::filesystem::remove(valid_path);

    const auto invalid_path = std::filesystem::temp_directory_path() / "realcars_brush_tire_invalid.cfg";
    {
        std::ofstream output(invalid_path);
        output << "unknown_parameter = 1.0\n";
    }
    bool rejected = false;
    try {
        static_cast<void>(realcars::tire::load_brush_tire_parameters(invalid_path));
    } catch (const std::runtime_error&) {
        rejected = true;
    }
    std::filesystem::remove(invalid_path);
    expect(rejected, "unknown parameter key is rejected");
}

}  // namespace

int main() {
    zero_slip_produces_zero_forces();
    force_signs_and_symmetry_are_consistent();
    small_slip_matches_requested_stiffness();
    pure_slip_matches_parabolic_pressure_fiala_solution();
    local_combined_slip_couples_force_generation();
    combined_forces_remain_inside_friction_ellipse();
    load_sensitivity_reduces_normalized_peak_force();
    camber_thrust_is_bounded_and_has_restoring_sign();
    aligning_moment_comes_from_contact_patch_distribution();
    legacy_trail_parameters_do_not_affect_coupled_solution();
    invalid_parameters_are_rejected();
    parameter_file_is_loaded_and_unknown_keys_are_rejected();

    if (failures == 0) {
        std::cout << "All brush-tire tests passed\n";
    }
    return failures == 0 ? 0 : 1;
}
