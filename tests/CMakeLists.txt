set(TEST_APP test_asam_cmp)

set(SRC_Include
)

set(SRC_Cpp testapp.cpp
        test_cmp_header.cpp
        test_message_header.cpp
        test_packet.cpp
        test_decoder.cpp
        test_encoder.cpp
        test_payload.cpp
        test_can_payload.cpp
        test_lin_payload.cpp
        test_ethernet_payload.cpp
        test_analog_payload.cpp
        test_capture_module_payload.cpp
        test_interface_payload.cpp
        test_status.cpp
        test_device_status.cpp
        test_interface_status.cpp
        create_message.h
        test_tecmp_header.cpp
        test_tecmp_capture_module_payload.cpp
        test_tecmp_can_payload.cpp
        test_tecmp_interface_payload.cpp
        test_tecmp_decoder.cpp
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

