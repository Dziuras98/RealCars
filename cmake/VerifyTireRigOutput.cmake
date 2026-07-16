if(NOT DEFINED TIRE_RIG)
    message(FATAL_ERROR "TIRE_RIG is required")
endif()
if(NOT DEFINED OUTPUT_DIR)
    message(FATAL_ERROR "OUTPUT_DIR is required")
endif()
if(NOT DEFINED PARAMETER_FILE)
    message(FATAL_ERROR "PARAMETER_FILE is required")
endif()

file(REMOVE_RECURSE "${OUTPUT_DIR}")
execute_process(
    COMMAND "${TIRE_RIG}" "${OUTPUT_DIR}" "${PARAMETER_FILE}"
    RESULT_VARIABLE rig_result
    OUTPUT_VARIABLE rig_stdout
    ERROR_VARIABLE rig_stderr
)
if(NOT rig_result EQUAL 0)
    message(FATAL_ERROR "Tire rig failed (${rig_result})\nstdout:\n${rig_stdout}\nstderr:\n${rig_stderr}")
endif()

set(required_files
    lateral_sweep.csv
    longitudinal_sweep.csv
    combined_sweep.csv
    characterization_samples.csv
    characterization_summary.csv
    run_metadata.csv
)
foreach(file_name IN LISTS required_files)
    set(file_path "${OUTPUT_DIR}/${file_name}")
    if(NOT EXISTS "${file_path}")
        message(FATAL_ERROR "Missing tire-rig output: ${file_path}")
    endif()
    file(SIZE "${file_path}" file_size)
    if(file_size EQUAL 0)
        message(FATAL_ERROR "Empty tire-rig output: ${file_path}")
    endif()
endforeach()

file(READ "${OUTPUT_DIR}/characterization_samples.csv" samples)
string(FIND "${samples}" "model,sweep,normal_load_n" samples_header_position)
string(FIND "${samples}" "brush,lateral" brush_position)
string(FIND "${samples}" "linear,lateral" linear_position)
string(FIND "${samples}" "brush,camber" camber_position)
if(samples_header_position LESS 0 OR brush_position LESS 0 OR linear_position LESS 0 OR camber_position LESS 0)
    message(FATAL_ERROR "characterization_samples.csv is missing required schema or sweeps")
endif()

file(READ "${OUTPUT_DIR}/run_metadata.csv" metadata)
string(FIND "${metadata}" "sample_count,9196" sample_count_position)
string(FIND "${metadata}" "validation_status" validation_position)
string(FIND "${metadata}" "brush.reference_load_n" brush_parameters_position)
string(FIND "${metadata}" "linear.longitudinal_stiffness_n" linear_parameters_position)
if(sample_count_position LESS 0 OR validation_position LESS 0 OR brush_parameters_position LESS 0 OR linear_parameters_position LESS 0)
    message(FATAL_ERROR "run_metadata.csv is incomplete")
endif()
