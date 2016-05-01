set(TESTS
    barycentric
    math
)

foreach (TEST_NAME ${TESTS})
    add_executable(${TEST_NAME} ${TEST_NAME}.cpp)
    target_link_libraries(${TEST_NAME} catch_main)
    add_test(geometric/${TEST_NAME} ${TEST_NAME})
endforeach ()
