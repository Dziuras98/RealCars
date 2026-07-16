#include "realcars/tire/BrushTireModel.hpp"
#include "realcars/tire/LinearTireModel.hpp"

#include <array>
#include <cmath>
#include <iostream>
#include <numbers>
#include <string_view>

namespace {

constexpr double degrees_to_radians(double degrees) {
    return degrees * std::numbers::pi / 180.0;
}

struct ForceLimits {
    double longitudinal_n{0.0};
    double lateral_n{0.0};
};

struct ModelContract {
    std::string_view name;
    const realcars::tire::TireModel& model;
    ForceLimits (*limits)(double normal_load_n, const void* context);
    const void* context;
};

int failures = 0;

void expect(bool condition, std::string_view model, std::string_view message) {
    if (!condition) {
        std::cerr << "FAILED [" << model << "]: " << message << '\n';
        ++failures;
    }
}

[[nodiscard]] bool finite(const realcars::tire::TireForces& forces) {
    return std::isfinite(forces.longitudinal_force_n)
        && std::isfinite(forces.lateral_force_n)
        && std::isfinite(forces.aligning_moment_nm);
}

[[nodiscard]] bool approximately_equal(double left, double right, double tolerance) {
    return std::abs(left - right) <= tolerance;
}

[[nodiscard]] ForceLimits brush_limits(double normal_load_n, const void* context) {
    const auto& model = *static_cast<const realcars::tire::BrushTireModel*>(context);
    const auto& parameters = model.parameters();
    const double friction = model.friction_coefficient(normal_load_n);
    return {
        .longitudinal_n = friction * normal_load_n * parameters.longitudinal_friction_scale,
        .lateral_n = friction * normal_load_n * parameters.lateral_friction_scale,
    };
}

[[nodiscard]] ForceLimits linear_limits(double normal_load_n, const void* context) {
    const auto& model = *static_cast<const realcars::tire::LinearTireModel*>(context);
    const double force_limit = model.parameters().friction_coefficient * normal_load_n;
    return {.longitudinal_n = force_limit, .lateral_n = force_limit};
}

void verify_zero_and_unloaded_contract(const ModelContract& contract) {
    for (const double load : {1'000.0, 4'000.0, 8'000.0}) {
        const auto forces = contract.model.evaluate({.normal_load_n = load});
        expect(
            approximately_equal(forces.longitudinal_force_n, 0.0, 1.0e-12),
            contract.name,
            "zero slip produces zero Fx");
        expect(
            approximately_equal(forces.lateral_force_n, 0.0, 1.0e-12),
            contract.name,
            "zero slip and camber produce zero Fy");
        expect(
            approximately_equal(forces.aligning_moment_nm, 0.0, 1.0e-12),
            contract.name,
            "zero slip and camber produce zero Mz");
    }

    for (const double load : {-1'000.0, 0.0}) {
        const auto forces = contract.model.evaluate({
            .normal_load_n = load,
            .slip_ratio = 0.2,
            .slip_angle_rad = degrees_to_radians(10.0),
            .camber_angle_rad = degrees_to_radians(3.0),
        });
        expect(
            approximately_equal(forces.longitudinal_force_n, 0.0, 1.0e-12),
            contract.name,
            "non-positive load produces zero Fx");
        expect(
            approximately_equal(forces.lateral_force_n, 0.0, 1.0e-12),
            contract.name,
            "non-positive load produces zero Fy");
        expect(
            approximately_equal(forces.aligning_moment_nm, 0.0, 1.0e-12),
            contract.name,
            "non-positive load produces zero Mz");
    }
}

void verify_sign_and_symmetry_contract(const ModelContract& contract) {
    const auto positive_longitudinal = contract.model.evaluate({
        .normal_load_n = 4'000.0,
        .slip_ratio = 0.05,
    });
    const auto negative_longitudinal = contract.model.evaluate({
        .normal_load_n = 4'000.0,
        .slip_ratio = -0.05,
    });
    expect(
        positive_longitudinal.longitudinal_force_n > 0.0,
        contract.name,
        "positive slip ratio produces positive Fx");
    expect(
        approximately_equal(
            positive_longitudinal.longitudinal_force_n,
            -negative_longitudinal.longitudinal_force_n,
            1.0e-9),
        contract.name,
        "pure longitudinal Fx is odd");

    const auto positive_lateral = contract.model.evaluate({
        .normal_load_n = 4'000.0,
        .slip_angle_rad = degrees_to_radians(3.0),
    });
    const auto negative_lateral = contract.model.evaluate({
        .normal_load_n = 4'000.0,
        .slip_angle_rad = degrees_to_radians(-3.0),
    });
    expect(
        positive_lateral.lateral_force_n < 0.0,
        contract.name,
        "positive slip angle produces opposing Fy");
    expect(
        positive_lateral.aligning_moment_nm > 0.0,
        contract.name,
        "positive slip angle produces restoring Mz");
    expect(
        approximately_equal(
            positive_lateral.lateral_force_n,
            -negative_lateral.lateral_force_n,
            1.0e-9),
        contract.name,
        "pure lateral Fy is odd");
    expect(
        approximately_equal(
            positive_lateral.aligning_moment_nm,
            -negative_lateral.aligning_moment_nm,
            1.0e-9),
        contract.name,
        "pure lateral Mz is odd");
}

void verify_finite_and_force_limit_contract(const ModelContract& contract) {
    constexpr std::array<double, 4> loads{1'000.0, 2'000.0, 4'000.0, 8'000.0};
    constexpr std::array<double, 5> slip_ratios{-0.4, -0.1, 0.0, 0.1, 0.4};
    constexpr std::array<double, 5> slip_angles_deg{-20.0, -5.0, 0.0, 5.0, 20.0};
    constexpr std::array<double, 3> camber_angles_deg{-3.0, 0.0, 3.0};

    for (const double load : loads) {
        const ForceLimits limits = contract.limits(load, contract.context);
        for (const double slip_ratio : slip_ratios) {
            for (const double slip_angle_deg : slip_angles_deg) {
                for (const double camber_angle_deg : camber_angles_deg) {
                    const auto forces = contract.model.evaluate({
                        .normal_load_n = load,
                        .slip_ratio = slip_ratio,
                        .slip_angle_rad = degrees_to_radians(slip_angle_deg),
                        .camber_angle_rad = degrees_to_radians(camber_angle_deg),
                        .road_speed_mps = 20.0,
                    });
                    expect(finite(forces), contract.name, "finite input grid produces finite output");
                    const double utilization = std::hypot(
                        forces.longitudinal_force_n / limits.longitudinal_n,
                        forces.lateral_force_n / limits.lateral_n);
                    expect(
                        utilization <= 1.0 + 1.0e-12,
                        contract.name,
                        "combined force remains inside configured limit");
                }
            }
        }
    }
}

}  // namespace

int main() {
    const realcars::tire::BrushTireModel brush_model;
    const realcars::tire::LinearTireModel linear_model;
    const std::array<ModelContract, 2> contracts{
        ModelContract{
            .name = "brush",
            .model = brush_model,
            .limits = brush_limits,
            .context = &brush_model,
        },
        ModelContract{
            .name = "linear",
            .model = linear_model,
            .limits = linear_limits,
            .context = &linear_model,
        },
    };

    for (const ModelContract& contract : contracts) {
        verify_zero_and_unloaded_contract(contract);
        verify_sign_and_symmetry_contract(contract);
        verify_finite_and_force_limit_contract(contract);
    }

    if (failures == 0) {
        std::cout << "All tire characterization contracts passed\n";
    }
    return failures == 0 ? 0 : 1;
}
