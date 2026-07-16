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
    constexpr double slip_ratio = 1.0e-5;
    constexpr double slip_angle_rad = 1.0e-5;

    const auto longitudinal = model.evaluate({
        .normal_load_n = parameters.reference_load_n,
        .slip_ratio = slip_ratio,
    });
    const auto lateral = model.evaluate({
        .normal_load_n = parameters.reference_load_n,
        .slip_angle_rad = slip_angle_rad,
    });

    const double measured_longitudinal_stiffness = longitudinal.longitudinal_force_n / slip_ratio;
    const double measured_cornering_stiffness = -lateral.lateral_force_n / slip_angle_rad;
    expect(
        std::abs(measured_longitudinal_stiffness / parameters.longitudinal_stiffness_at_reference_load_n - 1.0) < 1.0e-3,
        "small-slip longitudinal gradient matches parameter");
    expect(
        std::abs(measured_cornering_stiffness / parameters.cornering_stiffness_at_reference_load_n_per_rad - 1.0) < 1.0e-3,
        "small-slip lateral gradient matches parameter");
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
                expect(utilization <= 1.0 + 1.0e-12, "combined force remains inside ellipse");
            }
        }
    }
}

void load_sensitivity_reduces_normalized_peak_force() {
    const realcars::tire::BrushTireModel model;
    const auto low_load = model.evaluate({
        .normal_load_n = 2'000.0,
        .slip_ratio = 1.0,
    });
    const auto high_load = model.evaluate({
        .normal_load_n = 8'000.0,
        .slip_ratio = 1.0,
    });

    expect(
        low_load.longitudinal_force_n / 2'000.0 > high_load.longitudinal_force_n / 8'000.0,
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
        std::abs(forces.lateral_force_n) <= model.friction_coefficient(4'000.0) * 4'000.0,
        "camber force remains friction bounded");
}

void aligning_moment_falls_near_saturation() {
    const realcars::tire::BrushTireModel model;
    const auto moderate = model.evaluate({
        .normal_load_n = 4'000.0,
        .slip_angle_rad = degrees_to_radians(2.0),
    });
    const auto saturated = model.evaluate({
        .normal_load_n = 4'000.0,
        .slip_angle_rad = degrees_to_radians(30.0),
    });

    expect(std::abs(moderate.aligning_moment_nm) > 1.0, "moderate slip produces aligning moment");
    expect(std::abs(saturated.aligning_moment_nm) < std::abs(moderate.aligning_moment_nm), "aligning moment falls near saturation");
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
    combined_forces_remain_inside_friction_ellipse();
    load_sensitivity_reduces_normalized_peak_force();
    camber_thrust_is_bounded_and_has_restoring_sign();
    aligning_moment_falls_near_saturation();
    invalid_parameters_are_rejected();
    parameter_file_is_loaded_and_unknown_keys_are_rejected();

    if (failures == 0) {
        std::cout << "All brush-tire tests passed\n";
    }
    return failures == 0 ? 0 : 1;
}
