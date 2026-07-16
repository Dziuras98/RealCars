#include "realcars/tire/LinearTireModel.hpp"

#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <numbers>
#include <stdexcept>
#include <string>

namespace {

constexpr double degrees_to_radians(double degrees) {
    return degrees * std::numbers::pi / 180.0;
}

void write_lateral_sweep(
    const realcars::tire::TireModel& model,
    const std::filesystem::path& output_directory) {
    std::ofstream output(output_directory / "lateral_sweep.csv");
    if (!output) {
        throw std::runtime_error("Could not create lateral_sweep.csv");
    }

    output << "slip_angle_deg,normal_load_n,fx_n,fy_n,mz_nm\n";
    for (const double normal_load_n : {2'000.0, 4'000.0, 6'000.0}) {
        for (int angle_tenths = -150; angle_tenths <= 150; ++angle_tenths) {
            const double angle_deg = static_cast<double>(angle_tenths) / 10.0;
            const auto forces = model.evaluate({
                .normal_load_n = normal_load_n,
                .slip_ratio = 0.0,
                .slip_angle_rad = degrees_to_radians(angle_deg),
                .camber_angle_rad = 0.0,
                .road_speed_mps = 20.0,
            });

            output << angle_deg << ',' << normal_load_n << ','
                   << forces.longitudinal_force_n << ','
                   << forces.lateral_force_n << ','
                   << forces.aligning_moment_nm << '\n';
        }
    }
}

void write_longitudinal_sweep(
    const realcars::tire::TireModel& model,
    const std::filesystem::path& output_directory) {
    std::ofstream output(output_directory / "longitudinal_sweep.csv");
    if (!output) {
        throw std::runtime_error("Could not create longitudinal_sweep.csv");
    }

    output << "slip_ratio,normal_load_n,fx_n,fy_n,mz_nm\n";
    for (const double normal_load_n : {2'000.0, 4'000.0, 6'000.0}) {
        for (int slip_milli = -250; slip_milli <= 250; ++slip_milli) {
            const double slip_ratio = static_cast<double>(slip_milli) / 1'000.0;
            const auto forces = model.evaluate({
                .normal_load_n = normal_load_n,
                .slip_ratio = slip_ratio,
                .slip_angle_rad = 0.0,
                .camber_angle_rad = 0.0,
                .road_speed_mps = 20.0,
            });

            output << slip_ratio << ',' << normal_load_n << ','
                   << forces.longitudinal_force_n << ','
                   << forces.lateral_force_n << ','
                   << forces.aligning_moment_nm << '\n';
        }
    }
}

}  // namespace

int main(int argc, char* argv[]) {
    try {
        const std::filesystem::path output_directory =
            argc > 1 ? std::filesystem::path{argv[1]} : std::filesystem::path{"telemetry"};
        std::filesystem::create_directories(output_directory);

        const realcars::tire::LinearTireModel model;
        write_lateral_sweep(model, output_directory);
        write_longitudinal_sweep(model, output_directory);

        std::cout << "Wrote tire sweeps to " << output_directory << '\n';
        return 0;
    } catch (const std::exception& exception) {
        std::cerr << "Tire rig failed: " << exception.what() << '\n';
        return 1;
    }
}
