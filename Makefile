ifndef CXX
	CXX = clang++
endif

ifeq (,$(findstring clang++, $CXX))
	CXXFLAGS = -std=c++11 -stdlib=libc++
else
	CXXFLAGS = -std=c++11
endif

MAIN_CPP = src/main.cpp
SRC = src/data_loader.cpp src/tsp.cpp src/stats.cpp src/ini_file.cpp
TESTS_SRC = tests/main.cpp  tests/algo_string.cpp  tests/ini_file.cpp tests/ap.cpp tests/atsp.cpp
TEAMCITY_TESTS_SRC = tests/teamcity_cppunit.cpp tests/teamcity_messages.cpp

clean-all:
	rm -rf optimer* bin include lib share build* env

cpplint:
	python tools/cpplint.py --filter="-build/include,-runtime/reference,-readability/streams" `find src` 2>&1

cpplint-tst:
	python tools/cpplint.py --filter="-build/include,-runtime/reference,-readability/streams" $(TESTS_SRC)  2>&1

cppcheck:
	cppcheck --quiet --enable=all --inconclusive --std=c++11 -UNDEBUG src 2>&1

cppcheck-tst:
	cppcheck --quiet --enable=all --std=c++11 --includes-file="../src;../include" tests 2>&1
