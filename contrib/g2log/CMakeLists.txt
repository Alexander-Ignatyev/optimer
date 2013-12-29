# CMakeLists.txt cmake configuration for g2log test
# g2log is a KjellKod Logger
# 2011 @author Kjell Hedström, hedstrom@kjellkod.cc
# ==================================================================
# 2010 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own
#    risk and comes  with no warranties.
#
# This code is yours to share, use and modify with no strings attached
#   and no restrictions or obligations.
# ===================================================================

# Below are details for compiling on Windows and Linux
# by default only an example g2log binary is created
# the performance and unit tests creation can be enabled by switching their
# OPTIONs from OFF to ON --- See below at around line 110

# 2012-05-29: justthread is no longer necessary on Windows (vs2011) and
#             linux (gcc4.7.1)


# WINDOWS == README: Example how to setup environment + running an example
# Below written for VS11 (2012)
# 1. please use the "Visual Studio Command Prompt 11 (2012)"
# 2. from the g2log folder
#    mkdir build
#    cd build;
# 3. cmake -DCMAKE_BUILD_TYPE=Release -G "Visual Studio 11" ..
#              the "Visual Studio 11" .. does not require just::thread!
# 4. msbuild g2log_by_kjellkod.sln /p:Configuration=Release
# 5. Release\g2log-FATAL-example.exe
#
#
# . LINUX:To try this out from folder g2log:
#    mkdir build
#    cd build
#    cmake ..    # create makefiles in g2log/build directory
#    make    # link active_object, g2log and example code to get an "example" executable
#     ./g2log-FATAL-example
# ============================================================================

cmake_minimum_required (VERSION 2.6)
ENABLE_LANGUAGE(CXX)
set(CMAKE_BUILD_TYPE Release)

project (g2log_by_kjellkod)
set(LOG_SRC ${g2log_by_kjellkod_SOURCE_DIR}/src)

set(SRC_CPP 
    ${LOG_SRC}/g2logworker.cpp
    ${LOG_SRC}/g2log.cpp 
    ${LOG_SRC}/g2time.cpp 
    ${LOG_SRC}/active.cpp
)

set(SRC_H 
    ${LOG_SRC}/g2logworker.h
    ${LOG_SRC}/g2log.h
    ${LOG_SRC}/crashhandler.h
    ${LOG_SRC}/g2time.h
    ${LOG_SRC}/g2future.h
    ${LOG_SRC}/active.h
    ${LOG_SRC}/shared_queue.h
)

if(UNIX)
    set(SRC_PLATFORM_SPECIFIC ${LOG_SRC}/crashhandler_unix.cpp)
else()
    set(SRC_PLATFORM_SPECIFIC ${LOG_SRC}/crashhandler_win.cpp)
endif()

if(APPLE)
    set(CMAKE_CXX_FLAGS "-D_XOPEN_SOURCE ${CMAKE_CXX_FLAGS}")
endif()

if(UNIX)
    set(CMAKE_CXX_FLAGS "-D_GLIBCXX_USE_NANOSLEEP ${CMAKE_CXX_FLAGS}")
endif()

set(SRC_FILES ${SRC_CPP} ${SRC_H} ${SRC_PLATFORM_SPECIFIC})
add_library(lib_g2logger  ${SRC_FILES})
