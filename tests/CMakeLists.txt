set(TEST_APP test_asam_cmp)

set(SRC_Include 
)

set(SRC_Cpp testapp.cpp
)

add_executable(${TEST_APP} ${SRC_Cpp}
                           ${SRC_Include}
)

target_link_libraries(${TEST_APP} PRIVATE asam_cmp
                                          gtest
                                          gmock
)

add_test(NAME ${TEST_APP}
         COMMAND $<TARGET_FILE_NAME:${TEST_APP}>
         WORKING_DIRECTORY bin
)

