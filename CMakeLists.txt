cmake_minimum_required(VERSION 3.24)

project(
    RealCars
    VERSION 0.2.0
    DESCRIPTION "A testable vehicle-dynamics simulation core"
    LANGUAGES CXX
)

option(REALCARS_BUILD_TESTS "Build RealCars tests" ON)
option(REALCARS_ENABLE_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(CompilerWarnings)

add_library(realcar_simulation STATIC
    src/tire/BrushTireModel.cpp
    src/tire/BrushTireModelParametersIO.cpp
    src/tire/LinearTireModel.cpp
)
add_library(RealCars::Simulation ALIAS realcar_simulation)

target_compile_features(realcar_simulation PUBLIC cxx_std_20)
target_include_directories(realcar_simulation
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)
realcars_set_project_warnings(realcar_simulation ${REALCARS_ENABLE_WARNINGS_AS_ERRORS})

add_executable(realcar_tire_rig
    apps/tire_rig/main.cpp
)
target_link_libraries(realcar_tire_rig PRIVATE RealCars::Simulation)
realcars_set_project_warnings(realcar_tire_rig ${REALCARS_ENABLE_WARNINGS_AS_ERRORS})

if(REALCARS_BUILD_TESTS)
    include(CTest)
    enable_testing()

    add_executable(realcar_linear_tire_model_tests
        tests/test_linear_tire_model.cpp
    )
    target_link_libraries(realcar_linear_tire_model_tests PRIVATE RealCars::Simulation)
    realcars_set_project_warnings(realcar_linear_tire_model_tests ${REALCARS_ENABLE_WARNINGS_AS_ERRORS})
    add_test(NAME realcar_linear_tire_model_tests COMMAND realcar_linear_tire_model_tests)

    add_executable(realcar_brush_tire_model_tests
        tests/test_brush_tire_model.cpp
    )
    target_link_libraries(realcar_brush_tire_model_tests PRIVATE RealCars::Simulation)
    realcars_set_project_warnings(realcar_brush_tire_model_tests ${REALCARS_ENABLE_WARNINGS_AS_ERRORS})
    add_test(NAME realcar_brush_tire_model_tests COMMAND realcar_brush_tire_model_tests)

    add_test(
        NAME realcar_tire_rig_smoke
        COMMAND realcar_tire_rig
            "${CMAKE_CURRENT_BINARY_DIR}/test-telemetry"
            "${CMAKE_CURRENT_SOURCE_DIR}/data/tires/reference_sport_tire.cfg"
    )
endif()
