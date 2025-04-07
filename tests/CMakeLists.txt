function(add_app_test test_name test_source test_labels)
    add_executable(${test_name} ${test_source})
    target_link_libraries(${test_name}
        PRIVATE
            flora_core
            # flora_nav-dr
            # flora_nav-of
            # flora_nav-sf
            gtest
            gtest_main
    )
    add_test(NAME ${test_name} COMMAND ${test_name})
    set_tests_properties(${test_name} PROPERTIES
        LABELS "${test_labels}"
    )
endfunction()

# UNIT Tests
# -- Core
add_app_test(matrix_tests unit/core/MatrixTests.cpp "UnitTests;Core")
add_app_test(vector_tests unit/core/Vector3DTests.cpp "UnitTests;Core")
add_app_test(quaternion_tests unit/core/QuaternionTests.cpp "UnitTests;Core")

# -- Nav-DR (Dead Reckoning)


# -- Nav-OF (Optical Flow)


# -- Nav-SF (Sensor Fusion)


# INTEGRATION Tests

