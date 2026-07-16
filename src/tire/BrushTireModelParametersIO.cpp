#include "realcars/tire/BrushTireModelParametersIO.hpp"

#include <cctype>
#include <fstream>
#include <stdexcept>
#include <string>
#include <string_view>

namespace realcars::tire {

namespace {

[[nodiscard]] std::string_view trim(std::string_view value) {
    while (!value.empty() && std::isspace(static_cast<unsigned char>(value.front())) != 0) {
        value.remove_prefix(1);
    }
    while (!value.empty() && std::isspace(static_cast<unsigned char>(value.back())) != 0) {
        value.remove_suffix(1);
    }
    return value;
}

[[nodiscard]] double parse_number(std::string_view text, std::size_t line_number) {
    std::size_t consumed = 0;
    const std::string owned_text{text};
    double value = 0.0;
    try {
        value = std::stod(owned_text, &consumed);
    } catch (const std::exception&) {
        throw std::runtime_error("Invalid numeric value on line " + std::to_string(line_number));
    }
    if (consumed != owned_text.size()) {
        throw std::runtime_error("Invalid trailing characters on line " + std::to_string(line_number));
    }
    return value;
}

void assign_parameter(
    BrushTireModelParameters& parameters,
    std::string_view key,
    double value,
    std::size_t line_number) {
    if (key == "reference_load_n") {
        parameters.reference_load_n = value;
    } else if (key == "friction_coefficient_at_reference_load") {
        parameters.friction_coefficient_at_reference_load = value;
    } else if (key == "load_sensitivity") {
        parameters.load_sensitivity = value;
    } else if (key == "longitudinal_friction_scale") {
        parameters.longitudinal_friction_scale = value;
    } else if (key == "lateral_friction_scale") {
        parameters.lateral_friction_scale = value;
    } else if (key == "longitudinal_stiffness_at_reference_load_n") {
        parameters.longitudinal_stiffness_at_reference_load_n = value;
    } else if (key == "cornering_stiffness_at_reference_load_n_per_rad") {
        parameters.cornering_stiffness_at_reference_load_n_per_rad = value;
    } else if (key == "camber_stiffness_at_reference_load_n_per_rad") {
        parameters.camber_stiffness_at_reference_load_n_per_rad = value;
    } else if (key == "stiffness_load_exponent") {
        parameters.stiffness_load_exponent = value;
    } else if (key == "contact_patch_half_length_m") {
        parameters.contact_patch_half_length_m = value;
    } else if (key == "pneumatic_trail_fraction") {
        parameters.pneumatic_trail_fraction = value;
    } else if (key == "trail_falloff_exponent") {
        parameters.trail_falloff_exponent = value;
    } else {
        throw std::runtime_error(
            "Unknown tire parameter '" + std::string{key} + "' on line " + std::to_string(line_number));
    }
}

}  // namespace

BrushTireModelParameters load_brush_tire_parameters(const std::filesystem::path& path) {
    std::ifstream input(path);
    if (!input) {
        throw std::runtime_error("Could not open tire parameter file: " + path.string());
    }

    BrushTireModelParameters parameters;
    std::string line;
    std::size_t line_number = 0;
    while (std::getline(input, line)) {
        ++line_number;
        const std::size_t comment = line.find('#');
        const std::string_view content = trim(
            std::string_view{line}.substr(0, comment == std::string::npos ? line.size() : comment));
        if (content.empty()) {
            continue;
        }

        const std::size_t separator = content.find('=');
        if (separator == std::string_view::npos) {
            throw std::runtime_error("Expected key=value on line " + std::to_string(line_number));
        }

        const std::string_view key = trim(content.substr(0, separator));
        const std::string_view value_text = trim(content.substr(separator + 1));
        if (key.empty() || value_text.empty()) {
            throw std::runtime_error("Expected non-empty key and value on line " + std::to_string(line_number));
        }
        assign_parameter(parameters, key, parse_number(value_text, line_number), line_number);
    }

    static_cast<void>(BrushTireModel{parameters});
    return parameters;
}

}  // namespace realcars::tire
