# Copyright (c) 2014 Alexander Ignatyev. All rights reserved.

project(tsp_common)
include(${CMAKE_SOURCE_DIR}/utils.cmake)
add_ordinary_optimer_library()
target_link_libraries(${PROJECT_NAME} common)
