#
#      __        ___.         .__
#    _/  |______ \_ |__  __ __|  | _____ _______
#    \   __\__  \ | __ \|  |  \  | \__  \\_  __ \
#     |  |  / __ \| \_\ \  |  /  |__/ __ \|  | \/
#     |__| (____  /___  /____/|____(____  /__|
#               \/    \/                \/
#
#    *  Author: Anas Hamdane
#    *  Github: https://github.com/Anas-Hamdane
#
#

include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

# Prevent gtest from overriding compiler/linker options
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

enable_testing()

set(TESTS_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/source)
set(TESTS_BIN_DIR ${CMAKE_BINARY_DIR}/bin/tests)
set(TESTS_EXPECTED_DIR ${CMAKE_CURRENT_SOURCE_DIR}/expected)

file(GLOB HEADER_FILES ${CMAKE_SOURCE_DIR}/include/tabular/*.hpp)
file(GLOB TEST_SOURCES ${TESTS_SOURCE_DIR}/*.cpp)

foreach(TEST_SOURCE ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)

    add_executable(${TEST_NAME} ${TEST_SOURCE} ${HEADER_FILES})

    target_include_directories(${TEST_NAME} 
        PRIVATE 
        ${CMAKE_SOURCE_DIR}/include 
        ${CMAKE_SOURCE_DIR}/single_include
    )

    target_compile_definitions(${TEST_NAME} PRIVATE
        EXPECTED_OUTPUT_DIR="${TESTS_EXPECTED_DIR}"
        TEST_NAME="${TEST_NAME}"
    )

    set_target_properties(${TEST_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${TESTS_BIN_DIR}
    )

    target_link_libraries(${TEST_NAME} PRIVATE gtest_main)

    add_test(NAME ${TEST_NAME} COMMAND ${TESTS_BIN_DIR}/${TEST_NAME})

    message(STATUS "Added GTest: ${TEST_NAME}")
endforeach()

# custom target
add_custom_target(run_tests
    COMMAND ${CMAKE_COMMAND} -E echo "Running all tests..."
    WORKING_DIRECTORY ${TESTS_BIN_DIR}
)

# run all tests
foreach(TEST_SOURCE ${TEST_SOURCES})
    get_filename_component(TEST_TO_RUN ${TEST_SOURCE} NAME_WE)
    add_custom_command(
        TARGET run_tests
        POST_BUILD
        COMMAND ${TESTS_BIN_DIR}/${TEST_TO_RUN}
        COMMENT "Running test ${TEST_TO_RUN}\n"
    )
endforeach()