set(LIB_NAME asam_cmp)

set(SRC_Headers ../include/${LIB_NAME}/common.h
                ../include/${LIB_NAME}/cmp_header.h
                ../include/${LIB_NAME}/message_header.h
                ../include/${LIB_NAME}/payload_type.h
                ../include/${LIB_NAME}/decoder.h
                ../include/${LIB_NAME}/encoder.h
                ../include/${LIB_NAME}/packet.h
                ../include/${LIB_NAME}/payload.h
                ../include/${LIB_NAME}/can_payload_base.h
                ../include/${LIB_NAME}/can_payload.h
                ../include/${LIB_NAME}/can_fd_payload.h
                ../include/${LIB_NAME}/ethernet_payload.h
                ../include/${LIB_NAME}/analog_payload.h
                ../include/${LIB_NAME}/capture_module_payload.h
                ../include/${LIB_NAME}/interface_payload.h
                ../include/${LIB_NAME}/status.h
                ../include/${LIB_NAME}/device_status.h
                ../include/${LIB_NAME}/interface_status.h
)

set(SRC_Sources cmp_header.cpp
                message_header.cpp
                decoder.cpp
                encoder.cpp
                packet.cpp
                payload.cpp
                can_payload_base.cpp
                can_payload.cpp
                can_fd_payload.cpp
                ethernet_payload.cpp
                analog_payload.cpp
                capture_module_payload.cpp
                interface_payload.cpp
                status.cpp
                device_status.cpp
                interface_status.cpp
)

add_library(${LIB_NAME} STATIC ${SRC_Headers} ${SRC_Sources})

target_include_directories(${LIB_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/../include>
)
