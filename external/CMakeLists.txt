set(CMAKE_FOLDER "external")

include(FetchContent)

if (ASAM_CMP_LIB_ENABLE_TESTS)
    if (BUILDING_AS_SUBMODULE)
        set(INSTALL_GTEST Off CACHE BOOL "" FORCE)
    endif()
    
    set(gtest_force_shared_crt On CACHE BOOL "Force shared CRT")
    add_subdirectory(gtest)
endif()