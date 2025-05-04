cmake_minimum_required(VERSION 3.15)
project(tabular LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    include_directories(SYSTEM
        ${CMAKE_CXX_COMPILER_EXTERNAL_TOOLCHAIN}/include/c++/
        ${CMAKE_CXX_COMPILER_EXTERNAL_TOOLCHAIN}/include/c++/${CMAKE_CXX_COMPILER_VERSION}
        ${CMAKE_CXX_COMPILER_EXTERNAL_TOOLCHAIN}/include
    )
endif()

# simple example
add_executable(${PROJECT_NAME} examples/simple.cpp)

target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall
    -Wextra
    -pedantic
)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif()