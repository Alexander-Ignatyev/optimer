# Copyright (c) 2014 Alexander Ignatyev. All rights reserved.

project(symmetric_tsp)
include(${CMAKE_SOURCE_DIR}/utils.cmake)
add_ordinary_optimer_library()
add_optimer_unit_test(${CMAKE_SOURCE_DIR}/tests/ms_one_tree.cpp)
add_optimer_unit_test(${CMAKE_SOURCE_DIR}/tests/stsp.cpp)
add_optimer_unit_test(${CMAKE_SOURCE_DIR}/tests/stsp_lagrangean.cpp)
