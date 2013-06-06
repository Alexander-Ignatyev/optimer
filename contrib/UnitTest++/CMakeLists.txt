# Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

cmake_minimum_required(VERSION 2.8)
enable_language(CXX)
enable_testing()

option (UT_USE_UNIT_TESTS "Create unit tests" OFF)

project(UnitTest++2)

file(GLOB UT_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*.cpp src/*.h)
# include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

if(UNIX)
    set(UT_PLAT_DIR Posix)
else()
    set(UT_PLAT_DIR Win32)
endif(UNIX)

file(GLOB UT_PLAT_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/${UT_PLAT_DIR}/*.cpp src/${UT_PLAT_DIR}/*.h)
# include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/${UT_PLAT_DIR})

add_library(UnitTestPP STATIC ${UT_SRCS} ${UT_PLAT_SRCS})

if(UT_USE_UNIT_TESTS)
    file(GLOB UT_TEST_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/tests/*.cpp src/tests/*.h)
    # include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/tests)
    add_executable(TestUnitTestPP ${UT_TEST_SRCS})
    target_link_libraries(TestUnitTestPP UnitTestPP)

    add_test(NAME TestUnitTestPP COMMAND TestUnitTestPP ${CONFIG_PATH} ${CONFIG_TASKS_PATH} ${SOUND_LOG_PATH})
endif(UT_USE_UNIT_TESTS)

