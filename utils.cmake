# Copyright (c) 2014 Alexander Ignatyev. All rights reserved.

macro(init_optimer)
    # clear global lists
    set(OPTIMER_LIBS "" CACHE INTERNAL "")
    set(SRC_TEST_FILES "" CACHE INTERNAL "")
endmacro(init_optimer)

macro(list_contains var value)
  set(${var})
  foreach(value2 ${ARGN})
    if(${value} STREQUAL ${value2})
      set(${var} TRUE)
    endif()
  endforeach (value2)
endmacro(list_contains)

macro(set_cpp11_flags)
    get_filename_component(CXX ${CMAKE_CXX_COMPILER} NAME_WE)

    # compiler flags
    if(${CXX} MATCHES "^clang*" OR ${CXX} MATCHES "g\\+\\+" OR ${CXX} MATCHES "^icpc")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    endif()
    if (${CXX} MATCHES "^clang*")
       set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
    endif()
    if (${CMAKE_SYSTEM_NAME} MATCHES "^Linux")
       set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
    endif()
    if(XCODE_VERSION)
        set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++11")
        set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
    endif()
endmacro(set_cpp11_flags)

macro(set_cpp_flags)
    get_filename_component(CXX ${CMAKE_CXX_COMPILER} NAME_WE)
    if (${CXX} MATCHES "^clang*" AND NOT XCODE_VERSION)
       set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize-undefined-trap-on-error")
       set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize-address")
    endif()
    if(MSVC)
    	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
    endif()
endmacro()

macro(add_optimer_library name sources)
    add_library(${name} STATIC ${sources})
    set(OPTIMER_LIBS ${OPTIMER_LIBS} ${name} CACHE INTERNAL "")
endmacro(add_optimer_library)

macro(add_ordinary_optimer_library)
    if("${PROJECT_NAME}" STREQUAL "")
        message(FATAL_ERROR "Please define project name for ${CMAKE_CURRENT_SOURCE_DIR}")
    endif()
    list_contains(NOT_UNIQUE_PROJECT "${PROJECT_NAME}" ${OPTIMER_LIBS})
    if(NOT_UNIQUE_PROJECT)
        message(FATAL_ERROR "Please define unique project name for ${CMAKE_CURRENT_SOURCE_DIR}")
    endif()
    file(GLOB SRC *.cpp *.h)
    add_optimer_library(${PROJECT_NAME} "${SRC}")
endmacro(add_ordinary_optimer_library)

macro(add_optimer_executable main_cpp_path)
    get_filename_component(EXECUTABLE_NAME "${main_cpp_path}" NAME_WE)
    add_executable("${EXECUTABLE_NAME}" "${main_cpp_path}")
    target_link_libraries("${EXECUTABLE_NAME}" ${OPTIMER_LIBS})
endmacro(add_optimer_executable)

macro(add_optimer_unit_test file_name)
    set(SRC_TEST_FILES ${SRC_TEST_FILES} ${file_name} CACHE INTERNAL "")
endmacro(add_optimer_unit_test)

macro(add_unit_tests)
    enable_testing()

    set(UNIT_TESTS_SRC tests)

    # UnitTest++
    include_directories("${CMAKE_HOME_DIRECTORY}/contrib/UnitTest++/src")
    add_subdirectory("${CMAKE_HOME_DIRECTORY}/contrib/UnitTest++")
    set(TEST_OPTIMER_LIBS ${TEST_OPTIMER_LIBS} UnitTestPP)

    # unittests
    add_executable(optimer-tests ${CMAKE_SOURCE_DIR}/tests/main.cpp ${SRC_TEST_FILES})
    target_link_libraries(optimer-tests ${OPTIMER_LIBS} ${TEST_OPTIMER_LIBS})

    add_test(NAME optimer-tests
    		WORKING_DIRECTORY "${CMAKE_HOME_DIRECTORY}"
    		COMMAND "${PROJECT_BINARY_DIR}/optimer-tests")
endmacro()
