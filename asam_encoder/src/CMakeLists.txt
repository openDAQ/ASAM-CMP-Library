set(LIB_NAME asam_encoder)

set(SRC_Headers ../include/${LIB_NAME}/asam_encoder.h               
)

set(SRC_Sources asam_encoder.cpp
)

add_library(${LIB_NAME} STATIC ${SRC_Headers} ${SRC_Sources})
add_library(AsamCmpLib::${LIB_NAME} ALIAS ${LIB_NAME})

target_include_directories(${LIB_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/../include>
)
