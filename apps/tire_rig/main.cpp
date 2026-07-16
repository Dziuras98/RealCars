#include "realcars/tire/BrushTireModel.hpp"
#include "realcars/tire/BrushTireModelParametersIO.hpp"
#include "realcars/tire/LinearTireModel.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <numbers>
#include <stdexcept>
#include <string>
#include <string_view>

namespace {

constexpr std::array<double, 3> kNormalLoadsN{2'000.0, 4'000.0, 6'000.0};
constexpr double kRoadSpeedMps = 20.0;
constexpr double kGradientStep = 1.0e-5;

constexpr double degrees_to_radians(double degrees) {
    return degrees * std::numbers::pi / 180.0;
}

constexpr double radians_to_degrees(double radians) {
    return radians * 180.0 / std::numbers::pi;
}

struct ForceLimits {
    double longitudinal_n{0.0};
    double lateral_n{0.0};
};

struct ModelRun {
    std::string_view name;
    const realcars::tire::TireModel& model;
    std::function<ForceLimits(double)> force_limits;
};

struct PeakValue {
    double magnitude{-1.0};
    double coordinate{0.0};
    double signed_value{0.0};
};

[[nodiscard]] std::ofstream open_csv(const std::filesystem::path& path) {
    std::ofstream output(path);
    if (!output) {
        throw std::runtime_error("Could not create " + path.filename().string());
    }
    output << std::setprecision(17);
    return output;
}

[[nodiscard]] std::string csv_quote(std::string_view value) {
    std::string result;
    result.reserve(value.size() + 2U);
    result.push_back('"');
    for (const char character : value) {
        if (character == '"') {
            result.push_back('"');
        }
        result.push_back(character);
    }
    result.push_back('"');
    return result;
}

[[nodiscard]] double combined_utilization(
    const realcars::tire::TireForces& forces,
    const ForceLimits& limits) {
    if (limits.longitudinal_n <= 0.0 || limits.lateral_n <= 0.0) {
        return 0.0;
    }
    return std::hypot(
        forces.longitudinal_force_n / limits.longitudinal_n,
        forces.lateral_force_n / limits.lateral_n);
}

void update_peak(PeakValue& peak, double coordinate, double value) {
    const double magnitude = std::abs(value);
    if (magnitude > peak.magnitude) {
        peak = {.magnitude = magnitude, .coordinate = coordinate, .signed_value = value};
    }
}

void write_legacy_lateral_sweep(
    const realcars::tire::TireModel& model,
    const std::filesystem::path& output_directory) {
    auto output = open_csv(output_directory / "lateral_sweep.csv");
    output << "slip_angle_deg,normal_load_n,fx_n,fy_n,mz_nm\n";
    for (const double normal_load_n : kNormalLoadsN) {
        for (int angle_tenths = -150; angle_tenths <= 150; ++angle_tenths) {
            const double angle_deg = static_cast<double>(angle_tenths) / 10.0;
            const auto forces = model.evaluate({
                .normal_load_n = normal_load_n,
                .slip_ratio = 0.0,
                .slip_angle_rad = degrees_to_radians(angle_deg),
                .camber_angle_rad = 0.0,
                .road_speed_mps = kRoadSpeedMps,
            });
            output << angle_deg << ',' << normal_load_n << ','
                   << forces.longitudinal_force_n << ','
                   << forces.lateral_force_n << ','
                   << forces.aligning_moment_nm << '\n';
        }
    }
}

void write_legacy_longitudinal_sweep(
    const realcars::tire::TireModel& model,
    const std::filesystem::path& output_directory) {
    auto output = open_csv(output_directory / "longitudinal_sweep.csv");
    output << "slip_ratio,normal_load_n,fx_n,fy_n,mz_nm\n";
    for (const double normal_load_n : kNormalLoadsN) {
        for (int slip_milli = -250; slip_milli <= 250; ++slip_milli) {
            const double slip_ratio = static_cast<double>(slip_milli) / 1'000.0;
            const auto forces = model.evaluate({
                .normal_load_n = normal_load_n,
                .slip_ratio = slip_ratio,
                .slip_angle_rad = 0.0,
                .camber_angle_rad = 0.0,
                .road_speed_mps = kRoadSpeedMps,
            });
            output << slip_ratio << ',' << normal_load_n << ','
                   << forces.longitudinal_force_n << ','
                   << forces.lateral_force_n << ','
                   << forces.aligning_moment_nm << '\n';
        }
    }
}

void write_legacy_combined_sweep(
    const realcars::tire::TireModel& model,
    const std::filesystem::path& output_directory) {
    auto output = open_csv(output_directory / "combined_sweep.csv");
    output << "slip_ratio,slip_angle_deg,normal_load_n,fx_n,fy_n,mz_nm\n";
    constexpr double normal_load_n = 4'000.0;
    for (int slip_percent = -20; slip_percent <= 20; ++slip_percent) {
        const double slip_ratio = static_cast<double>(slip_percent) / 100.0;
        for (int angle_half_degrees = -24; angle_half_degrees <= 24; ++angle_half_degrees) {
            const double angle_deg = static_cast<double>(angle_half_degrees) / 2.0;
            const auto forces = model.evaluate({
                .normal_load_n = normal_load_n,
                .slip_ratio = slip_ratio,
                .slip_angle_rad = degrees_to_radians(angle_deg),
                .camber_angle_rad = 0.0,
                .road_speed_mps = kRoadSpeedMps,
            });
            output << slip_ratio << ',' << angle_deg << ',' << normal_load_n << ','
                   << forces.longitudinal_force_n << ','
                   << forces.lateral_force_n << ','
                   << forces.aligning_moment_nm << '\n';
        }
    }
}

void write_characterization_sample(
    std::ofstream& output,
    const ModelRun& run,
    std::string_view sweep,
    const realcars::tire::TireState& state) {
    const auto forces = run.model.evaluate(state);
    const ForceLimits limits = run.force_limits(state.normal_load_n);
    output << run.name << ',' << sweep << ','
           << state.normal_load_n << ','
           << state.slip_ratio << ','
           << radians_to_degrees(state.slip_angle_rad) << ','
           << radians_to_degrees(state.camber_angle_rad) << ','
           << state.road_speed_mps << ','
           << forces.longitudinal_force_n << ','
           << forces.lateral_force_n << ','
           << forces.aligning_moment_nm << ','
           << limits.longitudinal_n << ','
           << limits.lateral_n << ','
           << combined_utilization(forces, limits) << '\n';
}

[[nodiscard]] std::size_t write_characterization_samples(
    const std::array<ModelRun, 2>& runs,
    const std::filesystem::path& output_directory) {
    auto output = open_csv(output_directory / "characterization_samples.csv");
    output << "model,sweep,normal_load_n,slip_ratio,slip_angle_deg,camber_angle_deg,"
              "road_speed_mps,fx_n,fy_n,mz_nm,fx_limit_n,fy_limit_n,combined_utilization\n";

    std::size_t sample_count = 0;
    for (const ModelRun& run : runs) {
        for (const double normal_load_n : kNormalLoadsN) {
            for (int angle_tenths = -150; angle_tenths <= 150; ++angle_tenths) {
                write_characterization_sample(output, run, "lateral", {
                    .normal_load_n = normal_load_n,
                    .slip_ratio = 0.0,
                    .slip_angle_rad = degrees_to_radians(static_cast<double>(angle_tenths) / 10.0),
                    .camber_angle_rad = 0.0,
                    .road_speed_mps = kRoadSpeedMps,
                });
                ++sample_count;
            }

            for (int slip_milli = -250; slip_milli <= 250; ++slip_milli) {
                write_characterization_sample(output, run, "longitudinal", {
                    .normal_load_n = normal_load_n,
                    .slip_ratio = static_cast<double>(slip_milli) / 1'000.0,
                    .slip_angle_rad = 0.0,
                    .camber_angle_rad = 0.0,
                    .road_speed_mps = kRoadSpeedMps,
                });
                ++sample_count;
            }

            for (int camber_tenths = -30; camber_tenths <= 30; ++camber_tenths) {
                write_characterization_sample(output, run, "camber", {
                    .normal_load_n = normal_load_n,
                    .slip_ratio = 0.0,
                    .slip_angle_rad = 0.0,
                    .camber_angle_rad = degrees_to_radians(static_cast<double>(camber_tenths) / 10.0),
                    .road_speed_mps = kRoadSpeedMps,
                });
                ++sample_count;
            }
        }

        constexpr double combined_load_n = 4'000.0;
        for (int slip_percent = -20; slip_percent <= 20; ++slip_percent) {
            for (int angle_half_degrees = -24; angle_half_degrees <= 24; ++angle_half_degrees) {
                write_characterization_sample(output, run, "combined", {
                    .normal_load_n = combined_load_n,
                    .slip_ratio = static_cast<double>(slip_percent) / 100.0,
                    .slip_angle_rad = degrees_to_radians(
                        static_cast<double>(angle_half_degrees) / 2.0),
                    .camber_angle_rad = 0.0,
                    .road_speed_mps = kRoadSpeedMps,
                });
                ++sample_count;
            }
        }
    }

    if (!output) {
        throw std::runtime_error("Failed while writing characterization_samples.csv");
    }
    return sample_count;
}

void write_characterization_summary(
    const std::array<ModelRun, 2>& runs,
    const std::filesystem::path& output_directory) {
    auto output = open_csv(output_directory / "characterization_summary.csv");
    output << "model,normal_load_n,zero_force_residual_n,initial_longitudinal_stiffness_n,"
              "initial_cornering_stiffness_n_per_rad,peak_fx_n,peak_fx_slip_ratio,"
              "peak_fy_n,peak_fy_slip_angle_deg,peak_mz_nm,peak_mz_slip_angle_deg,"
              "max_combined_utilization\n";

    for (const ModelRun& run : runs) {
        for (const double normal_load_n : kNormalLoadsN) {
            const auto zero = run.model.evaluate({
                .normal_load_n = normal_load_n,
                .road_speed_mps = kRoadSpeedMps,
            });
            const double zero_residual = std::hypot(
                std::hypot(zero.longitudinal_force_n, zero.lateral_force_n),
                zero.aligning_moment_nm);

            const auto positive_longitudinal = run.model.evaluate({
                .normal_load_n = normal_load_n,
                .slip_ratio = kGradientStep,
                .road_speed_mps = kRoadSpeedMps,
            });
            const auto negative_longitudinal = run.model.evaluate({
                .normal_load_n = normal_load_n,
                .slip_ratio = -kGradientStep,
                .road_speed_mps = kRoadSpeedMps,
            });
            const double longitudinal_stiffness =
                (positive_longitudinal.longitudinal_force_n
                 - negative_longitudinal.longitudinal_force_n)
                / (2.0 * kGradientStep);

            const auto positive_lateral = run.model.evaluate({
                .normal_load_n = normal_load_n,
                .slip_angle_rad = kGradientStep,
                .road_speed_mps = kRoadSpeedMps,
            });
            const auto negative_lateral = run.model.evaluate({
                .normal_load_n = normal_load_n,
                .slip_angle_rad = -kGradientStep,
                .road_speed_mps = kRoadSpeedMps,
            });
            const double cornering_stiffness =
                -(positive_lateral.lateral_force_n - negative_lateral.lateral_force_n)
                / (2.0 * kGradientStep);

            PeakValue longitudinal_peak;
            for (int slip_milli = -250; slip_milli <= 250; ++slip_milli) {
                const double slip_ratio = static_cast<double>(slip_milli) / 1'000.0;
                const auto forces = run.model.evaluate({
                    .normal_load_n = normal_load_n,
                    .slip_ratio = slip_ratio,
                    .road_speed_mps = kRoadSpeedMps,
                });
                update_peak(longitudinal_peak, slip_ratio, forces.longitudinal_force_n);
            }

            PeakValue lateral_peak;
            PeakValue aligning_peak;
            for (int angle_tenths = -150; angle_tenths <= 150; ++angle_tenths) {
                const double angle_deg = static_cast<double>(angle_tenths) / 10.0;
                const auto forces = run.model.evaluate({
                    .normal_load_n = normal_load_n,
                    .slip_angle_rad = degrees_to_radians(angle_deg),
                    .road_speed_mps = kRoadSpeedMps,
                });
                update_peak(lateral_peak, angle_deg, forces.lateral_force_n);
                update_peak(aligning_peak, angle_deg, forces.aligning_moment_nm);
            }

            double maximum_utilization = 0.0;
            for (int slip_percent = -20; slip_percent <= 20; ++slip_percent) {
                for (int angle_half_degrees = -24; angle_half_degrees <= 24;
                     ++angle_half_degrees) {
                    const auto state = realcars::tire::TireState{
                        .normal_load_n = normal_load_n,
                        .slip_ratio = static_cast<double>(slip_percent) / 100.0,
                        .slip_angle_rad = degrees_to_radians(
                            static_cast<double>(angle_half_degrees) / 2.0),
                        .camber_angle_rad = 0.0,
                        .road_speed_mps = kRoadSpeedMps,
                    };
                    maximum_utilization = std::max(
                        maximum_utilization,
                        combined_utilization(
                            run.model.evaluate(state),
                            run.force_limits(normal_load_n)));
                }
            }

            output << run.name << ',' << normal_load_n << ',' << zero_residual << ','
                   << longitudinal_stiffness << ',' << cornering_stiffness << ','
                   << longitudinal_peak.signed_value << ',' << longitudinal_peak.coordinate << ','
                   << lateral_peak.signed_value << ',' << lateral_peak.coordinate << ','
                   << aligning_peak.signed_value << ',' << aligning_peak.coordinate << ','
                   << maximum_utilization << '\n';
        }
    }
}

void write_run_metadata(
    const realcars::tire::BrushTireModel& brush_model,
    const realcars::tire::LinearTireModel& linear_model,
    const std::filesystem::path& parameter_path,
    std::size_t sample_count,
    const std::filesystem::path& output_directory) {
    auto output = open_csv(output_directory / "run_metadata.csv");
    output << "key,value\n";
    output << "schema_version,1\n";
    output << "sample_count," << sample_count << '\n';
    output << "road_speed_mps," << kRoadSpeedMps << '\n';
    output << "parameter_file," << csv_quote(parameter_path.string()) << '\n';
    output << "validation_status,"
           << csv_quote("diagnostic baseline; not calibrated to measured tire data") << '\n';
    output << "sweep_policy,"
           << csv_quote("legacy rig ranges retained; camber range mirrors existing 3 degree test")
           << '\n';

    const auto& brush = brush_model.parameters();
    output << "brush.reference_load_n," << brush.reference_load_n << '\n';
    output << "brush.friction_coefficient_at_reference_load,"
           << brush.friction_coefficient_at_reference_load << '\n';
    output << "brush.load_sensitivity," << brush.load_sensitivity << '\n';
    output << "brush.longitudinal_friction_scale," << brush.longitudinal_friction_scale << '\n';
    output << "brush.lateral_friction_scale," << brush.lateral_friction_scale << '\n';
    output << "brush.longitudinal_stiffness_at_reference_load_n,"
           << brush.longitudinal_stiffness_at_reference_load_n << '\n';
    output << "brush.cornering_stiffness_at_reference_load_n_per_rad,"
           << brush.cornering_stiffness_at_reference_load_n_per_rad << '\n';
    output << "brush.camber_stiffness_at_reference_load_n_per_rad,"
           << brush.camber_stiffness_at_reference_load_n_per_rad << '\n';
    output << "brush.stiffness_load_exponent," << brush.stiffness_load_exponent << '\n';
    output << "brush.contact_patch_half_length_m," << brush.contact_patch_half_length_m << '\n';
    output << "brush.pneumatic_trail_fraction," << brush.pneumatic_trail_fraction << '\n';
    output << "brush.trail_falloff_exponent," << brush.trail_falloff_exponent << '\n';

    const auto& linear = linear_model.parameters();
    output << "linear.longitudinal_stiffness_n," << linear.longitudinal_stiffness_n << '\n';
    output << "linear.cornering_stiffness_n_per_rad,"
           << linear.cornering_stiffness_n_per_rad << '\n';
    output << "linear.camber_stiffness_n_per_rad," << linear.camber_stiffness_n_per_rad << '\n';
    output << "linear.friction_coefficient," << linear.friction_coefficient << '\n';
    output << "linear.pneumatic_trail_m," << linear.pneumatic_trail_m << '\n';
}

}  // namespace

int main(int argc, char* argv[]) {
    try {
        const std::filesystem::path output_directory =
            argc > 1 ? std::filesystem::path{argv[1]} : std::filesystem::path{"telemetry"};
        std::filesystem::create_directories(output_directory);

        const std::filesystem::path parameter_path = argc > 2
            ? std::filesystem::path{argv[2]}
            : std::filesystem::path{"<built-in defaults>"};
        const auto brush_parameters = argc > 2
            ? realcars::tire::load_brush_tire_parameters(parameter_path)
            : realcars::tire::BrushTireModelParameters{};
        const realcars::tire::BrushTireModel brush_model{brush_parameters};
        const realcars::tire::LinearTireModel linear_model;

        const std::array<ModelRun, 2> runs{
            ModelRun{
                .name = "brush",
                .model = brush_model,
                .force_limits = [&brush_model](double normal_load_n) {
                    const auto& parameters = brush_model.parameters();
                    const double friction = brush_model.friction_coefficient(normal_load_n);
                    return ForceLimits{
                        .longitudinal_n = friction * normal_load_n
                            * parameters.longitudinal_friction_scale,
                        .lateral_n = friction * normal_load_n
                            * parameters.lateral_friction_scale,
                    };
                },
            },
            ModelRun{
                .name = "linear",
                .model = linear_model,
                .force_limits = [&linear_model](double normal_load_n) {
                    const double limit =
                        linear_model.parameters().friction_coefficient * normal_load_n;
                    return ForceLimits{.longitudinal_n = limit, .lateral_n = limit};
                },
            },
        };

        write_legacy_lateral_sweep(brush_model, output_directory);
        write_legacy_longitudinal_sweep(brush_model, output_directory);
        write_legacy_combined_sweep(brush_model, output_directory);
        const std::size_t sample_count = write_characterization_samples(runs, output_directory);
        write_characterization_summary(runs, output_directory);
        write_run_metadata(
            brush_model,
            linear_model,
            parameter_path,
            sample_count,
            output_directory);

        std::cout << "Wrote tire characterization data to " << output_directory
                  << " (" << sample_count << " comparative samples)\n";
        return 0;
    } catch (const std::exception& exception) {
        std::cerr << "Tire rig failed: " << exception.what() << '\n';
        return 1;
    }
}
