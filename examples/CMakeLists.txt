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

add_executable(basic basic.cpp)
add_executable(tabular_ tabular.cpp)
add_executable(tasks tasks.cpp)

set_target_properties(tabular_ PROPERTIES OUTPUT_NAME tabular)
