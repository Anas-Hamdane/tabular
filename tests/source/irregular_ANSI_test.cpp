/*
      __        ___.         .__
    _/  |______ \_ |__  __ __|  | _____ _______
    \   __\__  \ | __ \|  |  \  | \__  \\_  __ \
     |  |  / __ \| \_\ \  |  /  |__/ __ \|  | \/
     |__| (____  /___  /____/|____(____  /__|
               \/    \/                \/

    *  Author: Anas Hamdane
    *  Github: https://github.com/Anas-Hamdane

    * test for an irregular ANSI table
*/

#include <fstream>
#include <gtest/gtest.h>
#include <sstream>
#include <tabular/table.hpp>

#ifndef EXPECTED_OUTPUT_DIR
#error "EXPECTED_OUTPUT_DIR not defined"
#endif

#ifndef TEST_NAME
#error "TEST_NAME not defined"
#endif

std::string get_expected_output() {
    std::string file_path = std::string(EXPECTED_OUTPUT_DIR) + "/" + TEST_NAME + ".txt";

    std::ifstream file(file_path);
    if (!file.is_open()) {
        throw std::runtime_error("Missing expected output file: " + file_path);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

TEST(irr_ansi_table_test, output_eq_expected) {
    using namespace tabular;

    Table table;
    table.add_row({"Top 5 Math universities in the world"});
    table.add_row({"Massachusetts Institute of Technology (MIT)", "95.5"});
    table.add_row({"Harvard University", "93.2"});
    table.add_row({"University of Oxford", "93.1"});
    table.add_row({"University of Cambridge", "92.5"});
    table.add_row({"Stanford University", "91.6"});

    table.rows[0].set_row_padding(1);
    table.configure().set_global_align(Alignment::center);

    table.format().border(BorderStyle::ANSI);

    // important to be forced
    table.set_forced_width(50);

    table.set_forced_ansi();

    std::stringstream out;
    out << table;

    std::string expected = get_expected_output();

    EXPECT_EQ(out.str(), expected)
        << "\nTest Failed: " << TEST_NAME
        << "\nExpected:\n"
        << expected
        << "\nActual:\n"
        << out.str();
}