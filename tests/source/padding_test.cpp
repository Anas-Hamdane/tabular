/*
      __        ___.         .__
    _/  |______ \_ |__  __ __|  | _____ _______
    \   __\__  \ | __ \|  |  \  | \__  \\_  __ \
     |  |  / __ \| \_\ \  |  /  |__/ __ \|  | \/
     |__| (____  /___  /____/|____(____  /__|
               \/    \/                \/

    *  Author: Anas Hamdane
    *  Github: https://github.com/Anas-Hamdane

    * test for controlling table padding
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
  if (!file.is_open())
    throw std::runtime_error("Missing expected output file: " + file_path);

  std::stringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}

TEST(simple_table_test, output_eq_expected) {
  using namespace tabular;

  Table table;
  table.add_row({"Company", "Market Cap"});
  table.add_row({"Microsoft", "$3.256 T"});
  table.add_row({"Apple", "$2.949 T"});
  table.add_row({"NVIDIA", "$2.863 T"});
  table.add_row({"Amazon", "$2.039 T"});
  table.add_row({"Google", "$1.880 T"});

  table[0].config().alignment(Alignment::center);

  table[0].config().padding(1);

  // important to be forced
  table.set().forced_width(50, true);

  std::ostringstream out;
  out << table;

  std::string expected = get_expected_output();

  EXPECT_EQ(out.str(), expected)
      << "\nTest Failed: " << TEST_NAME
      << "\nExpected:\n"
      << expected
      << "\nActual:\n"
      << out.str();
}
