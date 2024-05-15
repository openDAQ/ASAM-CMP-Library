cmake_minimum_required(VERSION 3.19)
project(AsamEncoder CXX)

add_subdirectory(src)

if (ASAMCMP_ENABLE_TESTS)
    add_subdirectory(tests)
endif()
