# Copyright (c) 2014 Alexander Ignatyev. All rights reserved.

project(common)
include(${CMAKE_SOURCE_DIR}/utils.cmake)
add_ordinary_optimer_library()
add_optimer_unit_test(${CMAKE_SOURCE_DIR}/tests/algo_string.cpp)
add_optimer_unit_test(${CMAKE_SOURCE_DIR}/tests/ini_file.cpp)
