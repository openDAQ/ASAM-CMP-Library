cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
set(CMAKE_FOLDER "${CMAKE_FOLDER}/AsamCmpLib")
project(AsamCmpLib CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

option(ASAM_CMP_ENABLE_TESTS "Enable testing" OFF)
option(ASAM_CMP_BUILD_EXAMPLE "Build example" OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

get_filename_component(ROOT_DIR ${CMAKE_SOURCE_DIR} REALPATH)
get_filename_component(PROJ_DIR ${PROJECT_SOURCE_DIR} REALPATH)
if (NOT ${PROJECT_SOURCE_DIR} STREQUAL ${ROOT_DIR})
    set(BUILDING_AS_SUBMODULE ON)
    message(STATUS "Building as submodule")
else()
    message(STATUS "Building standalone")
endif()

if (ASAM_CMP_ENABLE_TESTS)
    enable_testing()
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if (MSVC)
    # As above CMAKE_CXX_STANDARD but for VS, enable paralell build
    # /Zc:__cplusplus forces MSVC to use the correct value of __cplusplus macro (otherwise always C++98)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17 /MP /Zc:__cplusplus")

    # Treat warnings as errors
    add_compile_options($<$<COMPILE_LANGUAGE:C,CXX>:/W4> $<$<COMPILE_LANGUAGE:C,CXX>:/WX>)
endif()

if (CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -Wall -Wextra")

    # Treat warnings as errors
    add_compile_options($<$<COMPILE_LANGUAGE:C,CXX>:-Werror>)
endif()

add_subdirectory(external)
add_subdirectory(src)

if (ASAM_CMP_ENABLE_TESTS)
    add_subdirectory(tests)
endif()

set(CMAKE_FOLDER "")

if (ASAM_CMP_BUILD_EXAMPLE)
    add_subdirectory(example)
endif()