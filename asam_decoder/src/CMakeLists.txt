set(LIB_NAME asam_decoder)

set(SRC_Headers ../include/${LIB_NAME}/asam_decoder.h               
)

set(SRC_Sources asam_decoder.cpp
)

add_library(${LIB_NAME} STATIC ${SRC_Headers} ${SRC_Sources})
add_library(AsamCmpLib::${LIB_NAME} ALIAS ${LIB_NAME})

target_include_directories(${LIB_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/../include>
)
