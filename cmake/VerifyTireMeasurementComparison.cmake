if(NOT DEFINED COMPARATOR)
    message(FATAL_ERROR "COMPARATOR is required")
endif()
if(NOT DEFINED FIXTURE_DIR)
    message(FATAL_ERROR "FIXTURE_DIR is required")
endif()
if(NOT DEFINED OUTPUT_DIR)
    message(FATAL_ERROR "OUTPUT_DIR is required")
endif()
if(NOT DEFINED PARAMETER_FILE)
    message(FATAL_ERROR "PARAMETER_FILE is required")
endif()

file(REMOVE_RECURSE "${OUTPUT_DIR}")
execute_process(
    COMMAND "${COMPARATOR}" "${FIXTURE_DIR}" "${OUTPUT_DIR}" "${PARAMETER_FILE}"
    RESULT_VARIABLE comparison_result
    OUTPUT_VARIABLE comparison_stdout
    ERROR_VARIABLE comparison_stderr
)
if(NOT comparison_result EQUAL 0)
    message(FATAL_ERROR
        "Tire comparison failed (${comparison_result})\n"
        "stdout:\n${comparison_stdout}\n"
        "stderr:\n${comparison_stderr}"
    )
endif()

set(required_files
    model_measurement_samples.csv
    model_measurement_summary.csv
    model_measurement_report.md
)
foreach(file_name IN LISTS required_files)
    set(file_path "${OUTPUT_DIR}/${file_name}")
    if(NOT EXISTS "${file_path}")
        message(FATAL_ERROR "Missing comparison output: ${file_path}")
    endif()
    file(SIZE "${file_path}" file_size)
    if(file_size EQUAL 0)
        message(FATAL_ERROR "Empty comparison output: ${file_path}")
    endif()
endforeach()

file(READ "${OUTPUT_DIR}/model_measurement_samples.csv" samples)
foreach(required_text IN ITEMS
    "KIT_LONG_SYNTHETIC.TDA"
    "KIT_SIDE_SYNTHETIC.TDX"
    "KIT_COMB_SYNTHETIC.TDB"
    "brush"
    "linear"
)
    string(FIND "${samples}" "${required_text}" required_position)
    if(required_position LESS 0)
        message(FATAL_ERROR "Missing ${required_text} in model_measurement_samples.csv")
    endif()
endforeach()

file(READ "${OUTPUT_DIR}/model_measurement_summary.csv" summary)
foreach(required_text IN ITEMS
    "brush,longitudinal,fx,all"
    "brush,lateral,fy,all"
    "brush,lateral,mz,all"
    "brush,combined,fx,all"
    "brush,combined,fy,all"
    "brush,combined,mz,all"
    "linear,longitudinal,fx,all"
)
    string(FIND "${summary}" "${required_text}" required_position)
    if(required_position LESS 0)
        message(FATAL_ERROR "Missing ${required_text} in model_measurement_summary.csv")
    endif()
endforeach()

file(READ "${OUTPUT_DIR}/model_measurement_report.md" report)
string(FIND "${report}" "Measurement samples: 6" sample_count_position)
string(FIND "${report}" "Parameter fitting performed: no" no_fit_position)
if(sample_count_position LESS 0 OR no_fit_position LESS 0)
    message(FATAL_ERROR "Comparison report is missing required provenance statements")
endif()
