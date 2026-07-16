#pragma once

#include "realcars/tire/TireModel.hpp"

namespace realcars::tire {

struct LinearTireModelParameters {
    double longitudinal_stiffness_n{80'000.0};
    double cornering_stiffness_n_per_rad{70'000.0};
    double camber_stiffness_n_per_rad{8'000.0};
    double friction_coefficient{1.0};
    double pneumatic_trail_m{0.08};
};

class LinearTireModel final : public TireModel {
public:
    explicit LinearTireModel(LinearTireModelParameters parameters = {});

    [[nodiscard]] TireForces evaluate(const TireState& state) const override;
    [[nodiscard]] const LinearTireModelParameters& parameters() const noexcept;

private:
    LinearTireModelParameters parameters_;
};

}  // namespace realcars::tire
