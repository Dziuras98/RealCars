#pragma once

#include "realcars/tire/TireModel.hpp"

namespace realcars::tire {

struct BrushTireModelParameters {
    double reference_load_n{4'000.0};
    double friction_coefficient_at_reference_load{1.20};
    double load_sensitivity{0.08};
    double longitudinal_friction_scale{1.0};
    double lateral_friction_scale{1.0};
    double longitudinal_stiffness_at_reference_load_n{110'000.0};
    double cornering_stiffness_at_reference_load_n_per_rad{85'000.0};
    double camber_stiffness_at_reference_load_n_per_rad{12'000.0};
    double stiffness_load_exponent{0.90};
    double contact_patch_half_length_m{0.09};
    double pneumatic_trail_fraction{0.55};
    double trail_falloff_exponent{1.50};
};

class BrushTireModel final : public TireModel {
public:
    explicit BrushTireModel(BrushTireModelParameters parameters = {});

    [[nodiscard]] TireForces evaluate(const TireState& state) const override;
    [[nodiscard]] const BrushTireModelParameters& parameters() const noexcept;
    [[nodiscard]] double friction_coefficient(double normal_load_n) const noexcept;

private:
    BrushTireModelParameters parameters_;
};

}  // namespace realcars::tire
