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

cmake_minimum_required(VERSION 3.15)
project(tabular VERSION 0.1 DESCRIPTION "C++ tabling library" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(EXAMPLES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/examples)
set(TABULAR_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/tabular)

set(HEADER_FILES
    ${TABULAR_INCLUDE_DIR}/column.hpp
    ${TABULAR_INCLUDE_DIR}/row.hpp
    ${TABULAR_INCLUDE_DIR}/enums.hpp
    ${TABULAR_INCLUDE_DIR}/glVars.hpp
    ${TABULAR_INCLUDE_DIR}/style.hpp
    ${TABULAR_INCLUDE_DIR}/utilities.hpp
    ${TABULAR_INCLUDE_DIR}/table.hpp
)

# HERE set the EXAMPLE you want to try
set(TEST_EXAMPLE "simple")

# simple example
add_executable(${TEST_EXAMPLE} ${EXAMPLES_DIR}/${TEST_EXAMPLE}.cpp ${HEADER_FILES})

target_include_directories(${TEST_EXAMPLE} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)