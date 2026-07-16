#include "realcars/tire/LinearTireModel.hpp"

#include <cmath>
#include <iostream>
#include <numbers>
#include <string_view>

namespace {

constexpr double degrees_to_radians(double degrees) {
    return degrees * std::numbers::pi / 180.0;
}

bool approximately_equal(double left, double right, double tolerance = 1e-9) {
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
    const realcars::tire::LinearTireModel model;
    const auto forces = model.evaluate({.normal_load_n = 4'000.0});

    expect(approximately_equal(forces.longitudinal_force_n, 0.0), "zero longitudinal force");
    expect(approximately_equal(forces.lateral_force_n, 0.0), "zero lateral force");
    expect(approximately_equal(forces.aligning_moment_nm, 0.0), "zero aligning moment");
}

void positive_slip_angle_produces_opposing_lateral_force() {
    const realcars::tire::LinearTireModel model;
    const auto forces = model.evaluate({
        .normal_load_n = 4'000.0,
        .slip_angle_rad = degrees_to_radians(1.0),
    });

    expect(forces.lateral_force_n < 0.0, "lateral force opposes positive slip angle");
    expect(forces.aligning_moment_nm > 0.0, "aligning moment follows documented sign convention");
}

void forces_are_limited_by_friction_circle() {
    const realcars::tire::LinearTireModelParameters parameters{
        .friction_coefficient = 1.1,
    };
    const realcars::tire::LinearTireModel model{parameters};
    const auto forces = model.evaluate({
        .normal_load_n = 3'000.0,
        .slip_ratio = 0.5,
        .slip_angle_rad = degrees_to_radians(20.0),
    });

    const double magnitude = std::hypot(
        forces.longitudinal_force_n,
        forces.lateral_force_n);
    expect(
        magnitude <= parameters.friction_coefficient * 3'000.0 + 1e-9,
        "combined force does not exceed friction circle");
}

void unloaded_tire_produces_zero_forces() {
    const realcars::tire::LinearTireModel model;
    const auto forces = model.evaluate({
        .normal_load_n = -10.0,
        .slip_ratio = 0.1,
        .slip_angle_rad = degrees_to_radians(3.0),
    });

    expect(approximately_equal(forces.longitudinal_force_n, 0.0), "unloaded tire has no Fx");
    expect(approximately_equal(forces.lateral_force_n, 0.0), "unloaded tire has no Fy");
}

}  // namespace

int main() {
    zero_slip_produces_zero_forces();
    positive_slip_angle_produces_opposing_lateral_force();
    forces_are_limited_by_friction_circle();
    unloaded_tire_produces_zero_forces();

    if (failures == 0) {
        std::cout << "All tire-model tests passed\n";
    }
    return failures == 0 ? 0 : 1;
}
