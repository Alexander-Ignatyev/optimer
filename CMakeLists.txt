# Copyright (c) 2013-2014 Alexander Ignatyev. All rights reserved.
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
#
# Known issue for MacPorts clang-3.3: http://trac.macports.org/ticket/38527
# to fix it just add a line to ~/.profile: 
# export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:/opt/local/libexec/llvm-3.3/lib/clang/3.3/lib/darwin

cmake_minimum_required(VERSION 2.6)
enable_language(CXX)

project(optimer)
include(utils.cmake)

init_optimer()

option (CXX11_ENABLED "C++11 is enabled" ON)
option (USE_UNIT_TESTS "Create unit tests" ON)
option (USE_G2LOG "Use g2log" ON)

set_cpp_flags()
if(CXX11_ENABLED)
    set_cpp11_flags()
endif()

message("[OPTIMER]: Compiler: ${CMAKE_CXX_COMPILER}")
message("[OPTIMER]: Compiler flags: ${CMAKE_CXX_FLAGS}")
message("[OPTIMER]: Debug compiler flags: ${CMAKE_CXX_FLAGS_DEBUG}")
message("[OPTIMER]: Minsizerel compiler flags: ${CMAKE_CXX_FLAGS_MINSIZEREL}")
message("[OPTIMER]: Release compiler flags: ${CMAKE_CXX_FLAGS_RELEASE}")

# set paths
set(PROJECT_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${PROJECT_SRC})

if(USE_G2LOG AND CXX11_ENABLED)
    # gl2log: https://bitbucket.org/KjellKod/g2log/src
    include_directories("${CMAKE_HOME_DIRECTORY}/contrib/g2log/src")
    add_subdirectory (contrib/g2log)
    set (OPTIMER_LIBS ${OPTIMER_LIBS} lib_g2logger CACHE INTERNAL "")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_G2LOG")
endif()

# components
add_subdirectory(${PROJECT_SRC}/common)
add_subdirectory(${PROJECT_SRC}/bnb)
add_subdirectory(${PROJECT_SRC}/tsp)
add_subdirectory(${PROJECT_SRC}/heuristics)

# executables
if(CXX11_ENABLED)
    add_optimer_executable(${PROJECT_SRC}/atsp.cpp)
    add_optimer_executable(${PROJECT_SRC}/stsp.cpp)
endif()
add_optimer_executable(${PROJECT_SRC}/simple_atsp.cpp)
add_optimer_executable(${PROJECT_SRC}/simple_stsp.cpp)

if(USE_UNIT_TESTS)
    add_unit_tests()
endif()
