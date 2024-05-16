set(LIB_NAME asam_cmp)

set(SRC_Headers ../include/${LIB_NAME}/common.h
                ../include/${LIB_NAME}/decoder.h
                ../include/${LIB_NAME}/packet.h
)

set(SRC_Sources decoder.cpp
                packet.cpp
)

add_library(${LIB_NAME} STATIC ${SRC_Headers} ${SRC_Sources})

target_include_directories(${LIB_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/../include>
)
