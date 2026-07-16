#include "realcars/tire/BrushTireModel.hpp"
#include "realcars/tire/BrushTireModelParametersIO.hpp"
#include "realcars/tire/LinearTireModel.hpp"

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <numbers>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>

namespace {

constexpr std::string_view kDatasetDoi = "10.35097/p0rr2jc5wmf0drf8";
constexpr std::string_view kDatasetLicense = "CC BY-NC-SA 4.0";

[[nodiscard]] constexpr double degrees_to_radians(double degrees) {
    return degrees * std::numbers::pi / 180.0;
}

[[nodiscard]] constexpr double radians_to_degrees(double radians) {
    return radians * 180.0 / std::numbers::pi;
}

[[nodiscard]] std::string_view trim(std::string_view value) {
    while (!value.empty() && std::isspace(static_cast<unsigned char>(value.front())) != 0) {
        value.remove_prefix(1);
    }
    while (!value.empty() && std::isspace(static_cast<unsigned char>(value.back())) != 0) {
        value.remove_suffix(1);
    }
    return value;
}

[[nodiscard]] std::vector<std::string> split_whitespace(std::string_view text) {
    std::istringstream input{std::string{text}};
    std::vector<std::string> tokens;
    std::string token;
    while (input >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

[[nodiscard]] double parse_number(std::string_view text, const std::filesystem::path& path) {
    std::size_t consumed = 0;
    const std::string owned{text};
    double value = 0.0;
    try {
        value = std::stod(owned, &consumed);
    } catch (const std::exception&) {
        throw std::runtime_error("Invalid number in " + path.string() + ": " + owned);
    }
    if (consumed != owned.size() || !std::isfinite(value)) {
        throw std::runtime_error("Invalid number in " + path.string() + ": " + owned);
    }
    return value;
}

[[nodiscard]] std::string upper_extension(const std::filesystem::path& path) {
    std::string extension = path.extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), [](unsigned char value) {
        return static_cast<char>(std::toupper(value));
    });
    return extension;
}

[[nodiscard]] std::string csv_quote(std::string_view value) {
    std::string quoted;
    quoted.reserve(value.size() + 2U);
    quoted.push_back('"');
    for (const char character : value) {
        if (character == '"') {
            quoted.push_back('"');
        }
        quoted.push_back(character);
    }
    quoted.push_back('"');
    return quoted;
}

enum class Sweep {
    longitudinal,
    lateral,
    combined,
};

[[nodiscard]] std::string_view sweep_name(Sweep sweep) {
    switch (sweep) {
    case Sweep::longitudinal:
        return "longitudinal";
    case Sweep::lateral:
        return "lateral";
    case Sweep::combined:
        return "combined";
    }
    throw std::logic_error("Unknown sweep");
}

[[nodiscard]] std::optional<Sweep> classify_sweep(const std::filesystem::path& path) {
    const std::string filename = path.filename().string();
    if (filename.starts_with("KIT_LONG_")) {
        return Sweep::longitudinal;
    }
    if (filename.starts_with("KIT_SIDE_")) {
        return Sweep::lateral;
    }
    if (filename.starts_with("KIT_COMB_")) {
        return Sweep::combined;
    }
    return std::nullopt;
}

struct Channel {
    std::string name;
    double scale{1.0};
};

struct Measurement {
    std::string source_file;
    Sweep sweep{Sweep::longitudinal};
    double normal_load_n{0.0};
    double slip_ratio{0.0};
    double slip_angle_rad{0.0};
    double camber_angle_rad{0.0};
    double source_trajectory_velocity{0.0};
    realcars::tire::TireForces measured;
};

[[nodiscard]] std::optional<std::size_t> channel_index(
    const std::vector<Channel>& channels,
    std::string_view name) {
    for (std::size_t index = 0; index < channels.size(); ++index) {
        if (channels[index].name == name) {
            return index;
        }
    }
    return std::nullopt;
}

[[nodiscard]] std::vector<Measurement> read_tydex_h_file(
    const std::filesystem::path& path,
    const std::filesystem::path& dataset_root) {
    const std::optional<Sweep> sweep = classify_sweep(path);
    if (!sweep.has_value()) {
        return {};
    }

    std::ifstream input(path);
    if (!input) {
        throw std::runtime_error("Could not open TYDEX file: " + path.string());
    }

    enum class Section {
        other,
        channels,
        data,
    };
    Section section = Section::other;
    std::vector<Channel> channels;
    std::vector<std::vector<double>> rows;
    std::string line;
    while (std::getline(input, line)) {
        const std::string_view content = trim(line);
        if (content.starts_with("**")) {
            if (content == "**MEASURCHANNELS") {
                section = Section::channels;
            } else if (content == "**MEASURDATA") {
                section = Section::data;
            } else {
                section = Section::other;
            }
            continue;
        }
        if (content.empty() || content.starts_with("!")) {
            continue;
        }

        const std::vector<std::string> tokens = split_whitespace(content);
        if (section == Section::channels) {
            if (tokens.size() < 2U) {
                continue;
            }
            channels.push_back(Channel{
                .name = tokens.front(),
                .scale = parse_number(tokens.back(), path),
            });
        } else if (section == Section::data) {
            if (tokens.size() != channels.size()) {
                throw std::runtime_error(
                    "TYDEX data column count does not match channel count in " + path.string());
            }
            std::vector<double> values;
            values.reserve(tokens.size());
            for (std::size_t index = 0; index < tokens.size(); ++index) {
                values.push_back(parse_number(tokens[index], path) * channels[index].scale);
            }
            rows.push_back(std::move(values));
        }
    }

    const auto fx_index = channel_index(channels, "FXH");
    const auto fy_index = channel_index(channels, "FYH");
    const auto fz_index = channel_index(channels, "FZH");
    const auto mz_index = channel_index(channels, "MZH");
    const auto slip_index = channel_index(channels, "LONGSLIP");
    const auto angle_index = channel_index(channels, "SLIPANGL");
    const auto camber_index = channel_index(channels, "INCLANGL");
    const auto velocity_index = channel_index(channels, "TRAJVELW");
    if (!fx_index.has_value() || !fy_index.has_value() || !fz_index.has_value()
        || !mz_index.has_value() || !slip_index.has_value() || !angle_index.has_value()
        || !camber_index.has_value()) {
        return {};
    }

    const std::filesystem::path relative_path = path.lexically_relative(dataset_root);
    std::vector<Measurement> measurements;
    measurements.reserve(rows.size());
    for (const std::vector<double>& values : rows) {
        const double normal_load_n = values[*fz_index];
        const double slip_ratio = values[*slip_index] / 100.0;
        const double slip_angle_rad = degrees_to_radians(values[*angle_index]);
        const double camber_angle_rad = degrees_to_radians(values[*camber_index]);
        const double velocity = velocity_index.has_value() ? values[*velocity_index] : 0.0;
        if (normal_load_n <= 0.0) {
            throw std::runtime_error("TYDEX row has non-positive normal load in " + path.string());
        }
        measurements.push_back(Measurement{
            .source_file = relative_path.generic_string(),
            .sweep = *sweep,
            .normal_load_n = normal_load_n,
            .slip_ratio = slip_ratio,
            .slip_angle_rad = slip_angle_rad,
            .camber_angle_rad = camber_angle_rad,
            .source_trajectory_velocity = velocity,
            .measured = {
                .longitudinal_force_n = values[*fx_index],
                .lateral_force_n = values[*fy_index],
                .aligning_moment_nm = values[*mz_index],
            },
        });
    }
    return measurements;
}

[[nodiscard]] std::vector<Measurement> load_measurements(
    const std::filesystem::path& dataset_root) {
    if (!std::filesystem::exists(dataset_root)) {
        throw std::runtime_error("Measurement directory does not exist: " + dataset_root.string());
    }

    std::vector<std::filesystem::path> files;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(dataset_root)) {
        if (!entry.is_regular_file()) {
            continue;
        }
        const std::string extension = upper_extension(entry.path());
        if (extension == ".TDX" || extension == ".TDA" || extension == ".TDB") {
            files.push_back(entry.path());
        }
    }
    std::sort(files.begin(), files.end());

    std::vector<Measurement> measurements;
    for (const std::filesystem::path& path : files) {
        std::vector<Measurement> file_measurements = read_tydex_h_file(path, dataset_root);
        measurements.insert(
            measurements.end(),
            std::make_move_iterator(file_measurements.begin()),
            std::make_move_iterator(file_measurements.end()));
    }
    if (measurements.empty()) {
        throw std::runtime_error("No TYDEX H-axis measurements found under " + dataset_root.string());
    }
    return measurements;
}

struct Prediction {
    const Measurement* measurement{nullptr};
    std::string_view model_name;
    realcars::tire::TireForces predicted;
};

enum class Component {
    fx,
    fy,
    mz,
};

[[nodiscard]] std::string_view component_name(Component component) {
    switch (component) {
    case Component::fx:
        return "fx";
    case Component::fy:
        return "fy";
    case Component::mz:
        return "mz";
    }
    throw std::logic_error("Unknown component");
}

[[nodiscard]] double component_value(
    const realcars::tire::TireForces& forces,
    Component component) {
    switch (component) {
    case Component::fx:
        return forces.longitudinal_force_n;
    case Component::fy:
        return forces.lateral_force_n;
    case Component::mz:
        return forces.aligning_moment_nm;
    }
    throw std::logic_error("Unknown component");
}

[[nodiscard]] std::vector<Component> compared_components(Sweep sweep) {
    switch (sweep) {
    case Sweep::longitudinal:
        return {Component::fx};
    case Sweep::lateral:
        return {Component::fy, Component::mz};
    case Sweep::combined:
        return {Component::fx, Component::fy, Component::mz};
    }
    throw std::logic_error("Unknown sweep");
}

struct GroupKey {
    std::string model;
    Sweep sweep{Sweep::longitudinal};
    Component component{Component::fx};
    std::optional<double> normal_load_n;

    [[nodiscard]] bool operator<(const GroupKey& other) const noexcept {
        return std::tie(model, sweep, component, normal_load_n)
            < std::tie(other.model, other.sweep, other.component, other.normal_load_n);
    }
};

struct Metric {
    std::size_t sample_count{0U};
    double rmse{0.0};
    double mae{0.0};
    double bias{0.0};
    double maximum_absolute_error{0.0};
    double measured_absolute_peak{0.0};
    double normalized_rmse{0.0};
    double r_squared{0.0};
    double correlation{0.0};
};

[[nodiscard]] Metric calculate_metric(const std::vector<std::pair<double, double>>& samples) {
    if (samples.empty()) {
        throw std::invalid_argument("Metric requires at least one sample");
    }

    double measured_mean = 0.0;
    double predicted_mean = 0.0;
    for (const auto& [measured, predicted] : samples) {
        measured_mean += measured;
        predicted_mean += predicted;
    }
    const double count = static_cast<double>(samples.size());
    measured_mean /= count;
    predicted_mean /= count;

    double squared_error_sum = 0.0;
    double absolute_error_sum = 0.0;
    double signed_error_sum = 0.0;
    double maximum_absolute_error = 0.0;
    double measured_absolute_peak = 0.0;
    double measured_variance_sum = 0.0;
    double predicted_variance_sum = 0.0;
    double covariance_sum = 0.0;
    for (const auto& [measured, predicted] : samples) {
        const double error = predicted - measured;
        squared_error_sum += error * error;
        absolute_error_sum += std::abs(error);
        signed_error_sum += error;
        maximum_absolute_error = std::max(maximum_absolute_error, std::abs(error));
        measured_absolute_peak = std::max(measured_absolute_peak, std::abs(measured));
        const double measured_delta = measured - measured_mean;
        const double predicted_delta = predicted - predicted_mean;
        measured_variance_sum += measured_delta * measured_delta;
        predicted_variance_sum += predicted_delta * predicted_delta;
        covariance_sum += measured_delta * predicted_delta;
    }

    const double rmse = std::sqrt(squared_error_sum / count);
    const double nan = std::numeric_limits<double>::quiet_NaN();
    return Metric{
        .sample_count = samples.size(),
        .rmse = rmse,
        .mae = absolute_error_sum / count,
        .bias = signed_error_sum / count,
        .maximum_absolute_error = maximum_absolute_error,
        .measured_absolute_peak = measured_absolute_peak,
        .normalized_rmse = measured_absolute_peak > 0.0 ? rmse / measured_absolute_peak : nan,
        .r_squared = measured_variance_sum > 0.0
            ? 1.0 - squared_error_sum / measured_variance_sum
            : nan,
        .correlation = measured_variance_sum > 0.0 && predicted_variance_sum > 0.0
            ? covariance_sum / std::sqrt(measured_variance_sum * predicted_variance_sum)
            : nan,
    };
}

struct MetricRow {
    GroupKey key;
    Metric metric;
};

[[nodiscard]] std::vector<MetricRow> calculate_metrics(
    const std::vector<Prediction>& predictions) {
    std::map<GroupKey, std::vector<std::pair<double, double>>> groups;
    for (const Prediction& prediction : predictions) {
        const Measurement& measurement = *prediction.measurement;
        for (const Component component : compared_components(measurement.sweep)) {
            const double measured = component_value(measurement.measured, component);
            const double predicted = component_value(prediction.predicted, component);
            groups[GroupKey{
                .model = std::string{prediction.model_name},
                .sweep = measurement.sweep,
                .component = component,
                .normal_load_n = std::nullopt,
            }].emplace_back(measured, predicted);
            groups[GroupKey{
                .model = std::string{prediction.model_name},
                .sweep = measurement.sweep,
                .component = component,
                .normal_load_n = measurement.normal_load_n,
            }].emplace_back(measured, predicted);
        }
    }

    std::vector<MetricRow> metrics;
    metrics.reserve(groups.size());
    for (const auto& [key, samples] : groups) {
        metrics.push_back(MetricRow{.key = key, .metric = calculate_metric(samples)});
    }
    return metrics;
}

[[nodiscard]] std::ofstream open_output(const std::filesystem::path& path) {
    std::ofstream output(path);
    if (!output) {
        throw std::runtime_error("Could not create output file: " + path.string());
    }
    output << std::setprecision(17);
    return output;
}

void write_samples(
    const std::vector<Prediction>& predictions,
    const std::filesystem::path& output_directory) {
    auto output = open_output(output_directory / "model_measurement_samples.csv");
    output << "source_file,sweep,model,normal_load_n,slip_ratio,slip_angle_deg,"
              "camber_angle_deg,source_trajectory_velocity,measured_fx_n,predicted_fx_n,"
              "fx_error_n,measured_fy_n,predicted_fy_n,fy_error_n,measured_mz_nm,"
              "predicted_mz_nm,mz_error_nm\n";
    for (const Prediction& prediction : predictions) {
        const Measurement& measurement = *prediction.measurement;
        output << csv_quote(measurement.source_file) << ','
               << sweep_name(measurement.sweep) << ','
               << prediction.model_name << ','
               << measurement.normal_load_n << ','
               << measurement.slip_ratio << ','
               << radians_to_degrees(measurement.slip_angle_rad) << ','
               << radians_to_degrees(measurement.camber_angle_rad) << ','
               << measurement.source_trajectory_velocity << ','
               << measurement.measured.longitudinal_force_n << ','
               << prediction.predicted.longitudinal_force_n << ','
               << prediction.predicted.longitudinal_force_n
                    - measurement.measured.longitudinal_force_n << ','
               << measurement.measured.lateral_force_n << ','
               << prediction.predicted.lateral_force_n << ','
               << prediction.predicted.lateral_force_n - measurement.measured.lateral_force_n
               << ',' << measurement.measured.aligning_moment_nm << ','
               << prediction.predicted.aligning_moment_nm << ','
               << prediction.predicted.aligning_moment_nm
                    - measurement.measured.aligning_moment_nm << '\n';
    }
}

void write_summary(
    const std::vector<MetricRow>& metrics,
    const std::filesystem::path& output_directory) {
    auto output = open_output(output_directory / "model_measurement_summary.csv");
    output << "model,sweep,component,normal_load_n,sample_count,rmse,mae,bias,"
              "maximum_absolute_error,measured_absolute_peak,normalized_rmse,r_squared,"
              "correlation\n";
    for (const MetricRow& row : metrics) {
        output << row.key.model << ',' << sweep_name(row.key.sweep) << ','
               << component_name(row.key.component) << ',';
        if (row.key.normal_load_n.has_value()) {
            output << *row.key.normal_load_n;
        } else {
            output << "all";
        }
        output << ',' << row.metric.sample_count << ',' << row.metric.rmse << ','
               << row.metric.mae << ',' << row.metric.bias << ','
               << row.metric.maximum_absolute_error << ','
               << row.metric.measured_absolute_peak << ','
               << row.metric.normalized_rmse << ',' << row.metric.r_squared << ','
               << row.metric.correlation << '\n';
    }
}

void write_report(
    const std::vector<Measurement>& measurements,
    const std::vector<MetricRow>& metrics,
    const std::filesystem::path& dataset_root,
    const std::filesystem::path& parameter_path,
    const std::filesystem::path& output_directory) {
    auto output = open_output(output_directory / "model_measurement_report.md");
    output << "# RealCars model-to-measurement tire comparison\n\n"
           << "- Dataset DOI: `" << kDatasetDoi << "`\n"
           << "- Dataset license: `" << kDatasetLicense << "`\n"
           << "- Coordinate system: TYDEX H\n"
           << "- Measurement root: `" << dataset_root.generic_string() << "`\n"
           << "- Brush parameter file: `" << parameter_path.generic_string() << "`\n"
           << "- Measurement samples: " << measurements.size() << "\n"
           << "- Compared models: current brush baseline and retained linear baseline\n"
           << "- Parameter fitting performed: no\n\n"
           << "The metrics below are diagnostic. They are not pass/fail acceptance criteria and do "
              "not validate the generic RealCars tire as the measured KIT tire.\n\n"
           << "## Aggregate metrics\n\n"
           << "| Model | Sweep | Component | N | RMSE | MAE | Bias | NRMSE of measured peak | R² | Correlation |\n"
           << "|---|---|---:|---:|---:|---:|---:|---:|---:|---:|\n";
    for (const MetricRow& row : metrics) {
        if (row.key.normal_load_n.has_value()) {
            continue;
        }
        output << "| " << row.key.model << " | " << sweep_name(row.key.sweep) << " | "
               << component_name(row.key.component) << " | " << row.metric.sample_count << " | "
               << row.metric.rmse << " | " << row.metric.mae << " | " << row.metric.bias
               << " | " << row.metric.normalized_rmse * 100.0 << "% | "
               << row.metric.r_squared << " | " << row.metric.correlation << " |\n";
    }
    output << "\n## Output files\n\n"
           << "- `model_measurement_samples.csv`: every measured point and prediction.\n"
           << "- `model_measurement_summary.csv`: aggregate and per-load metrics.\n"
           << "- `model_measurement_report.md`: this report.\n";
}

}  // namespace

int main(int argc, char* argv[]) {
    try {
        if (argc != 4) {
            std::cerr << "Usage: realcar_tire_measurement_compare "
                         "<measurement-root> <output-directory> <brush-parameter-file>\n";
            return 2;
        }

        const std::filesystem::path dataset_root{argv[1]};
        const std::filesystem::path output_directory{argv[2]};
        const std::filesystem::path parameter_path{argv[3]};
        std::filesystem::create_directories(output_directory);

        const std::vector<Measurement> measurements = load_measurements(dataset_root);
        const realcars::tire::BrushTireModel brush_model{
            realcars::tire::load_brush_tire_parameters(parameter_path)};
        const realcars::tire::LinearTireModel linear_model;

        std::vector<Prediction> predictions;
        predictions.reserve(measurements.size() * 2U);
        for (const Measurement& measurement : measurements) {
            const realcars::tire::TireState state{
                .normal_load_n = measurement.normal_load_n,
                .slip_ratio = measurement.slip_ratio,
                .slip_angle_rad = measurement.slip_angle_rad,
                .camber_angle_rad = measurement.camber_angle_rad,
                .road_speed_mps = measurement.source_trajectory_velocity,
            };
            predictions.push_back(Prediction{
                .measurement = &measurement,
                .model_name = "brush",
                .predicted = brush_model.evaluate(state),
            });
            predictions.push_back(Prediction{
                .measurement = &measurement,
                .model_name = "linear",
                .predicted = linear_model.evaluate(state),
            });
        }

        const std::vector<MetricRow> metrics = calculate_metrics(predictions);
        write_samples(predictions, output_directory);
        write_summary(metrics, output_directory);
        write_report(measurements, metrics, dataset_root, parameter_path, output_directory);

        std::cout << "Compared " << measurements.size()
                  << " TYDEX H measurement points against two tire models\n";
        return 0;
    } catch (const std::exception& exception) {
        std::cerr << "Tire model-to-measurement comparison failed: " << exception.what() << '\n';
        return 1;
    }
}
