# Copyright (c) 2013 Alexander Ignatyev. All rights reserved.
# CMakeLists.txt cmake configuration for optimer
# 
# Build instruction:
#  % mdkir build
#  % cd build
#  % export CXX=clang++ # or another compiler that you wish to use
#  % cmake ..
#  % make
#  % make test
#  % ./atsp <path-to-config-file>
# run cmake -DCMAKE_BUILD_TYPE=Release .. to build release version
#
# Build instruction for Visual Studio 2012
# % mkdir build
# % cd build
# % cmake -DUSE_UNIT_TESTS=OFF ..
# unit tests are not supported for VS now

cmake_minimum_required(VERSION 2.8)
enable_language(CXX)
enable_testing()

project(optimer)

option (USE_UNIT_TESTS "Create unit tests" ON)

get_filename_component(CXX ${CMAKE_CXX_COMPILER} NAME)
message("[OPTIMER]: Compiler: ${CXX}")

# compiler flags
if(${CXX} MATCHES "^clang*" OR ${CXX} MATCHES "g\\+\\+")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()
if (${CXX} MATCHES "^clang*")
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()
if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
endif()

message("[OPTIMER]: Compiler flags: ${CMAKE_CXX_FLAGS}")
message("[OPTIMER]: Debug compiler flags: ${CMAKE_CXX_FLAGS_DEBUG}")
message("[OPTIMER]: Minsizerel compiler flags: ${CMAKE_CXX_FLAGS_MINSIZEREL}")
message("[OPTIMER]: Release compiler flags: ${CMAKE_CXX_FLAGS_RELEASE}")

if(XCODE_VERSION)
    set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++11")
    set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
endif()

# set paths
set(PROJECT_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${PROJECT_SRC})

# gl2log: https://bitbucket.org/KjellKod/g2log/src
include_directories("${CMAKE_HOME_DIRECTORY}/contrib/g2log/src")
add_subdirectory (contrib/g2log)
set (LIBS ${LIBS} lib_g2logger)

# common lib
file(GLOB COMMON_SRC ${PROJECT_SRC}/common/*.cpp ${PROJECT_SRC}/common/*.h)
add_library(common STATIC ${COMMON_SRC})
set(LIBS ${LIBS} common)

# bnb lib
file(GLOB BNB_SRC ${PROJECT_SRC}/bnb/*.cpp ${PROJECT_SRC}/bnb/*.h)
add_library(bnb STATIC ${BNB_SRC})
set(LIBS ${LIBS} bnb)

# atsp library
file(GLOB ASSYMETRIC_TSP_SRC ${PROJECT_SRC}/tsp/assymetric/*.cpp ${PROJECT_SRC}/tsp/assymetric/*.h)
add_library(assymetric_tsp STATIC ${ASSYMETRIC_TSP_SRC})
set(LIBS ${LIBS} assymetric_tsp)

# atsp executable
file(GLOB ATSP_SRC ${PROJECT_SRC}/tsp/assymetric/*.cpp ${PROJECT_SRC}/tsp/assymetric/*.h)
add_executable(atsp ${PROJECT_SRC}/atsp.cpp)
target_link_libraries(atsp ${LIBS})

# stsp library
file(GLOB SYMMETRIC_TSP_SRC ${PROJECT_SRC}/tsp/symmetric/*.cpp ${PROJECT_SRC}/tsp/symmetric/*.h)
add_library(symmetric_tsp STATIC ${SYMMETRIC_TSP_SRC})
set(LIBS ${LIBS} symmetric_tsp)

# stsp executable
add_executable(stsp ${PROJECT_SRC}/stsp.cpp)
target_link_libraries(stsp ${LIBS})

if(USE_UNIT_TESTS)
    set(UNIT_TESTS_SRC tests)

	# UnitTest++
	include_directories("${CMAKE_HOME_DIRECTORY}/contrib/UnitTest++/src")
	add_subdirectory(contrib/UnitTest++)
	set(TEST_LIBS ${TEST_LIBS} UnitTestPP)

	# unittests
	file (GLOB SRC_TEST_FILES ${UNIT_TESTS_SRC}/*.cpp)
	add_executable(optimer-tests ${SRC_TEST_FILES})
	target_link_libraries(optimer-tests ${LIBS} ${TEST_LIBS})

	add_test(NAME optimer-tests
			WORKING_DIRECTORY "${CMAKE_HOME_DIRECTORY}"
			COMMAND "${PROJECT_BINARY_DIR}/optimer-tests")
endif()
