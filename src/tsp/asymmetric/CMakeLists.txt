# Copyright (c) 2014 Alexander Ignatyev. All rights reserved.

project(asymmetric_tsp)
include(${CMAKE_SOURCE_DIR}/utils.cmake)
add_ordinary_optimer_library()
add_optimer_unit_test(${CMAKE_SOURCE_DIR}/tests/ap.cpp)
add_optimer_unit_test(${CMAKE_SOURCE_DIR}/tests/simple_atsp.cpp)
if(CXX11_ENABLED)
add_optimer_unit_test(${CMAKE_SOURCE_DIR}/tests/atsp.cpp)
endif()
