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

set(EXAMPLES_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/source)
set(EXAMPLES_BIN_DIR ${CMAKE_BINARY_DIR}/bin/examples)

# all header files
file(GLOB HEADER_FILES ${CMAKE_SOURCE_DIR}/include/tabular/*.hpp)

# all examples
file(GLOB EXAMPLE_SOURCES ${EXAMPLES_SOURCE_DIR}/*.cpp)

foreach(EXAMPLE_SOURCE ${EXAMPLE_SOURCES})
    get_filename_component(EXAMPLE_NAME ${EXAMPLE_SOURCE} NAME_WE)
    
    add_executable(${EXAMPLE_NAME} ${EXAMPLE_SOURCE} ${HEADER_FILES})
    
    target_include_directories(${EXAMPLE_NAME} 
        PRIVATE 
        ${CMAKE_SOURCE_DIR}/include 
        ${CMAKE_SOURCE_DIR}/single_include
    )

    set_target_properties(${EXAMPLE_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${EXAMPLES_BIN_DIR}
    )
    
    message(STATUS "Added example: ${EXAMPLE_NAME}")
endforeach()

# custom target
add_custom_target(run_examples
    COMMAND ${CMAKE_COMMAND} -E echo "Running all examples..."
    WORKING_DIRECTORY ${EXAMPLES_BIN_DIR}
)

# run all examples
foreach(EXAMPLE_SOURCE ${EXAMPLE_SOURCES})
    get_filename_component(EXAMPLE_TO_RUN ${EXAMPLE_SOURCE} NAME_WE)
    add_custom_command(
        TARGET run_examples
        POST_BUILD
        COMMAND ${EXAMPLES_BIN_DIR}/${EXAMPLE_TO_RUN}
        COMMENT "Running example ${EXAMPLE_TO_RUN}\n"
    )
endforeach()