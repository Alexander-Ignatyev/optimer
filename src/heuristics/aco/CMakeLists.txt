# Copyright (c) 2014 Alexander Ignatyev. All rights reserved.

add_subdirectory(tsp)
if(CXX11_ENABLED)
add_optimer_executable(${CMAKE_CURRENT_SOURCE_DIR}/aco_tsp.cpp)
endif()
