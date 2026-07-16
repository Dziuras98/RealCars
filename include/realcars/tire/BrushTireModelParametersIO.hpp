#pragma once

#include "realcars/tire/BrushTireModel.hpp"

#include <filesystem>

namespace realcars::tire {

[[nodiscard]] BrushTireModelParameters load_brush_tire_parameters(
    const std::filesystem::path& path);

}  // namespace realcars::tire
