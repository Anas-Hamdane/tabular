/*
      __        ___.         .__
    _/  |______ \_ |__  __ __|  | _____ _______
    \   __\__  \ | __ \|  |  \  | \__  \\_  __ \
     |  |  / __ \| \_\ \  |  /  |__/ __ \|  | \/
     |__| (____  /___  /____/|____(____  /__|
               \/    \/                \/

    *  Author: Anas Hamdane
    *  Github: https://github.com/Anas-Hamdane

    * test for controlling the width of the table
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

TEST(width_table_test, output_eq_expected) {
    using namespace tabular;

    Table table;
    table.add_row({"5-Top Largest Countries"});
    table.add_row({"#1", "Russia"});
    table.add_row({"#2", "Canada"});
    table.add_row({"#3", "USA"});
    table.add_row({"#4", "China"});
    table.add_row({"#5", "Brazil"});

    table[0].config().alignment(Alignment::center);

    table.set().forced_width(50, true);
 
    table.config().columns_width(4, 0, Range(1, 5));

    std::ostringstream out;
    out << table;

    std::string expected = get_expected_output();

    EXPECT_EQ(out.str(), expected)
        << "\nTest Failed: " << TEST_NAME
        << "\nExpected:\n" << expected
        << "\nActual:\n" << out.str();
}