cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
set(CMAKE_FOLDER "${CMAKE_FOLDER}/AsamCmpLib")
project(AsamCmpLib CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

option(ASAMCMP_ENABLE_TESTS "Enable testing" ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(ASAMCMP_SOURCE_DIR ${PROJECT_SOURCE_DIR})

add_compile_options(/bigobj)

get_filename_component(ROOT_DIR ${CMAKE_SOURCE_DIR} REALPATH)
get_filename_component(PROJ_DIR ${PROJECT_SOURCE_DIR} REALPATH)
if (NOT ${PROJECT_SOURCE_DIR} STREQUAL ${ROOT_DIR})
    set(BUILDING_AS_SUBMODULE ON)
    message(STATUS "Building as submodule")
else()
    message(STATUS "Building standalone")
endif()

if (ASAMCMP_ENABLE_TESTS)
    enable_testing()
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if (MSVC)
    # As above CMAKE_CXX_STANDARD but for VS, enable paralell build
    # /Zc:__cplusplus forces MSVC to use the correct value of __cplusplus macro (otherwise always C++98)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17 /MP /Zc:__cplusplus")

    # Treat warnings as errors
    add_compile_options(/W4 /WX)
endif()

if (CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -Wall -Wextra")

    # Treat warnings as errors if not Debug or DSLIB_DEBUG_WARNINGS_AS_ERRORS is ON
    add_compile_options(-Werror)
endif()

add_subdirectory(external)

add_subdirectory(asam_decoder)
add_subdirectory(asam_encoder)
