# Copyright (c) 2014 Alexander Ignatyev. All rights reserved.

project(aco_tsp_lib)
include(${CMAKE_SOURCE_DIR}/utils.cmake)
add_ordinary_optimer_library()
add_optimer_unit_test(${CMAKE_SOURCE_DIR}/tests/aco_tsp.cpp)
