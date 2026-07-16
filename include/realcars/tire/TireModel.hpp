#pragma once

namespace realcars::tire {

struct TireState {
    double normal_load_n{0.0};
    double slip_ratio{0.0};
    double slip_angle_rad{0.0};
    double camber_angle_rad{0.0};
    double road_speed_mps{0.0};
};

struct TireForces {
    double longitudinal_force_n{0.0};
    double lateral_force_n{0.0};
    double aligning_moment_nm{0.0};
};

class TireModel {
public:
    virtual ~TireModel() = default;

    [[nodiscard]] virtual TireForces evaluate(const TireState& state) const = 0;
};

}  // namespace realcars::tire
