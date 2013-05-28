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

# set paths
set(PROJECT_SRC src)
set(UNI_TESTS_SRC tests)

# set build environment
get_filename_component(CXX ${CMAKE_CXX_COMPILER} NAME)
if(NOT ENV{OPTIMER_BUILD_ENV})
    set(ENV{OPTIMER_BUILD_ENV} "${CMAKE_HOME_DIRECTORY}/env")
    message("[OPTIMER]: Set OPTIMER_BUILD_ENV variable = $ENV{OPTIMER_BUILD_ENV}")
endif()

set(BUILD_ENV "$ENV{OPTIMER_BUILD_ENV}/${CXX}")

if(NOT EXISTS ${BUILD_ENV})
    message("[OPTIMER]: Build environment path ${BUILD_ENV} not found!")
    message("[OPTIMER]: Starting to build environment...")
    execute_process(COMMAND bash ${CMAKE_HOME_DIRECTORY}/contrib/build_env.sh ${CXX} $ENV{OPTIMER_BUILD_ENV}
        WORKING_DIRECTORY ${CMAKE_HOME_DIRECTORY}/contrib)
endif()

link_directories("${BUILD_ENV}/lib")
include_directories("${BUILD_ENV}/include" "${PROJECT_SRC}")

message("[OPTIMER]: Compiler: ${CXX}")
message("[OPTIMER]: Build environment: ${BUILD_ENV}")

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

# gl2log: https://bitbucket.org/KjellKod/g2log/src
include_directories("${CMAKE_HOME_DIRECTORY}/contrib/g2log/src")
add_subdirectory (contrib/g2log)
set (LIBS ${LIBS} lib_activeobject lib_g2logger)

# optimer lib
set(SRC_CPP ${PROJECT_SRC}/data_loader.cpp ${PROJECT_SRC}/ini_file.cpp ${PROJECT_SRC}/stats.cpp ${PROJECT_SRC}/tsp.cpp)
file(GLOB SRC_H ${PROJECT_SRC}/*.h)
set(SRC_FILES ${SRC_CPP} ${SRC_H})
add_library(optimer STATIC ${SRC_FILES})
set(LIBS ${LIBS} optimer)

# atsp executable
add_executable(atsp ${PROJECT_SRC}/main.cpp)
target_link_libraries(atsp ${LIBS})

if(USE_UNIT_TESTS)
	# teamcity extension for cppunit
	include_directories("${CMAKE_HOME_DIRECTORY}/contrib/cppunit_teamcity")
	add_subdirectory(contrib/cppunit_teamcity)
	set(TEST_LIBS ${TEST_LIBS} cppunit_teamcity)

	# unittests
	set(TEST_LIBS ${TEST_LIBS} cppunit)
	file (GLOB SRC_TEST_FILES ${UNI_TESTS_SRC}/*.cpp)
	add_executable(optimer-tests ${SRC_TEST_FILES})
	target_link_libraries(optimer-tests ${LIBS} ${TEST_LIBS})

	add_test(NAME optimer-tests
			WORKING_DIRECTORY "${CMAKE_HOME_DIRECTORY}"
			COMMAND "${PROJECT_BINARY_DIR}/optimer-tests")
endif()
