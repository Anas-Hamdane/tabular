cmake_minimum_required(VERSION 3.15)
project(tabular VERSION 0.1 DESCRIPTION "C++ tabling library" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(EXAMPLES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/examples)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

# HERE set the EXAMPLE you want to try
set(TEST_EXAMPLE "simple")

# simple example
add_executable(${TEST_EXAMPLE} ${EXAMPLES_DIR}/${TEST_EXAMPLE}.cpp ${INCLUDE_DIR}/tabular/tabular.hpp)