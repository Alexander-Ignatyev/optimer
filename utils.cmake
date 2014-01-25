# Copyright (c) 2014 Alexander Ignatyev. All rights reserved.

macro(list_contains var value)
  set(${var})
  foreach(value2 ${ARGN})
    if(${value} STREQUAL ${value2})
      set(${var} TRUE)
    endif()
  endforeach (value2)
endmacro(list_contains)

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
