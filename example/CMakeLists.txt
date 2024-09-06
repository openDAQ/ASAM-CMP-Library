add_executable(asam_cmp_example main.cpp)

target_link_libraries(asam_cmp_example PRIVATE
    asam_cmp
)

add_dependencies(asam_cmp_example
    asam_cmp
)

set_target_properties(asam_cmp_example
    PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:asam_cmp_example>
)