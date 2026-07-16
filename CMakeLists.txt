cmake_minimum_required(VERSION 3.24)

project(
    RealCars
    VERSION 0.1.0
    DESCRIPTION "A testable vehicle-dynamics simulation core"
    LANGUAGES CXX
)

option(REALCARS_BUILD_TESTS "Build RealCars tests" ON)
option(REALCARS_ENABLE_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(CompilerWarnings)

add_library(realcar_simulation STATIC
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

    add_executable(realcar_simulation_tests
        tests/test_linear_tire_model.cpp
    )
    target_link_libraries(realcar_simulation_tests PRIVATE RealCars::Simulation)
    realcars_set_project_warnings(realcar_simulation_tests ${REALCARS_ENABLE_WARNINGS_AS_ERRORS})

    add_test(NAME realcar_simulation_tests COMMAND realcar_simulation_tests)
endif()
