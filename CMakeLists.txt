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

cmake_minimum_required(VERSION 3.15)
project(tabular
        VERSION 1.5.0
        DESCRIPTION "lightweight, header-only C++ library for creating well-formatted, fully-customizable CLI tables."
        LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(TABULAR_BUILD_TESTS "Build tests" OFF)
if (TABULAR_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif ()

add_library(tabular INTERFACE)
add_library(tabular::tabular ALIAS tabular)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

target_compile_features(tabular INTERFACE cxx_std_11)
target_include_directories(tabular INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

configure_package_config_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/tabularConfig.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/tabularConfig.cmake"
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/tabular
)

write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/tabularConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY ExactVersion
)

install(DIRECTORY include/tabular
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        FILES_MATCHING PATTERN "*.h"
)

install(TARGETS tabular
        EXPORT tabularTargets
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT tabularTargets
        FILE tabularTargets.cmake
        NAMESPACE tabular::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/tabular
)

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/tabularConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/tabularConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/tabular
)

# examples
add_subdirectory(examples)
