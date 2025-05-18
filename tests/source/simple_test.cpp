/*
      __        ___.         .__
    _/  |______ \_ |__  __ __|  | _____ _______
    \   __\__  \ | __ \|  |  \  | \__  \\_  __ \
     |  |  / __ \| \_\ \  |  /  |__/ __ \|  | \/
     |__| (____  /___  /____/|____(____  /__|
               \/    \/                \/

    *  Author: Anas Hamdane
    *  Github: https://github.com/Anas-Hamdane

    * test for a simple table
*/

#include <gtest/gtest.h>
#include <tabular/table.hpp>
#include <sstream>
#include <fstream>

#ifndef EXPECTED_OUTPUT_DIR
#error "EXPECTED_OUTPUT_DIR not defined"
#endif

#ifndef TEST_NAME
#error "TEST_NAME not defined"
#endif

std::string get_expected_output() {
    std::string file_path = std::string(EXPECTED_OUTPUT_DIR) + "/" + TEST_NAME + ".txt";

    std::ifstream file(file_path);
    if (!file.is_open())
        throw std::runtime_error("Missing expected output file: " + file_path);

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

TEST(table_test, output_eq_expected) {
    using namespace tabular;

    Table table;
    table.add_row({"Countries Capitals"});
    table.add_row({"United States", "Washington"});
    table.add_row({"Brazil", "Brasilia"});
    table.add_row({"Countries Population"});
    table.add_row({"United States", "340.1 million (2024)"});
    table.add_row({"Brazil", "211.1 million (2023)"});

    // important to be forced
    table.set().forced_width(50, true);

    std::ostringstream out;
    out << table;

    std::string expected = get_expected_output();

    EXPECT_EQ(out.str(), expected)
        << "\nTest Failed: " << TEST_NAME
        << "\nExpected:\n" << expected
        << "\nActual:\n" << out.str();
}