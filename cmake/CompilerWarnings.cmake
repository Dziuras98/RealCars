function(realcars_set_project_warnings target warnings_as_errors)
    if(MSVC)
        set(warnings /W4 /permissive-)
        if(warnings_as_errors)
            list(APPEND warnings /WX)
        endif()
    else()
        set(warnings
            -Wall
            -Wextra
            -Wpedantic
            -Wconversion
            -Wsign-conversion
            -Wshadow
        )
        if(warnings_as_errors)
            list(APPEND warnings -Werror)
        endif()
    endif()

    target_compile_options(${target} PRIVATE ${warnings})
endfunction()
