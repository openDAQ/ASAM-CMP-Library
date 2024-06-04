set(LIB_NAME asam_cmp)

set(SRC_Headers ../include/${LIB_NAME}/common.h
                ../include/${LIB_NAME}/decoder.h
                ../include/${LIB_NAME}/encoder.h
                ../include/${LIB_NAME}/packet.h
                ../include/${LIB_NAME}/payload.h
                ../include/${LIB_NAME}/can_payload_base.h
                ../include/${LIB_NAME}/can_payload.h
                ../include/${LIB_NAME}/can_fd_payload.h
                ../include/${LIB_NAME}/ethernet_payload.h
)

set(SRC_Sources decoder.cpp
                encoder.cpp
                packet.cpp
                payload.cpp
                can_payload_base.cpp
                can_payload.cpp
                can_fd_payload.cpp
                ethernet_payload.cpp
)

add_library(${LIB_NAME} STATIC ${SRC_Headers} ${SRC_Sources})

target_include_directories(${LIB_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/../include>
)
