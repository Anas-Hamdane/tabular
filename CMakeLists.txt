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

# All Header files
file(GLOB HEADER_FILES ${TABULAR_INCLUDE_DIR}/*.hpp)

# All examples
file(GLOB EXAMPLE_SOURCES ${EXAMPLES_DIR}/*.cpp)

# compile every example
foreach(EXAMPLE_SOURCE ${EXAMPLE_SOURCES})
    get_filename_component(EXAMPLE_NAME ${EXAMPLE_SOURCE} NAME_WE)
    
    add_executable(${EXAMPLE_NAME} ${EXAMPLE_SOURCE} ${HEADER_FILES})
    
    target_include_directories(${EXAMPLE_NAME} 
        PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/include 
        ${CMAKE_CURRENT_SOURCE_DIR}/single_include
    )
    
    message(STATUS "Added example: ${EXAMPLE_NAME}")
endforeach()