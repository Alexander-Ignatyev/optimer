INCLUDE = -Iinclude -Isrc
LIB = -Llib
MAIN_CPP = src/main.cpp
SRC = src/data_loader.cpp src/tsp.cpp src/stats.cpp
TESTS_SRC = tests/main.cpp tests/ap.cpp tests/atsp.cpp
TEAMCITY_TESTS_SRC = tests/teamcity_cppunit.cpp tests/teamcity_messages.cpp

all:
	clang++ $(INCLUDE) --std=c++11 --stdlib=libc++ -g -o optimer $(MAIN_CPP) $(SRC)

clang:
	clang++ --std=c++11 --stdlib=libc++ -O2 -o optimer-clang $(MAIN_CPP) $(SRC)

clang-33:
	clang++-mp-3.3 --std=c++11 --stdlib=libc++ -O2 -o optimer-clang++-mp-3.3 $(MAIN_CPP) $(SRC)

gcc-47:
	g++-mp-4.7 --std=c++11 -O2 -o optimer-g++-mp-4.7 $(MAIN_CPP) $(SRC)

tst:
	g++-mp-4.7 $(INCLUDE) $(LIB) -lcppunit --std=c++11 -O2 -o optimer-tests $(TESTS_SRC) $(TEAMCITY_TESTS_SRC) $(SRC)

clean-all:
	rm -rf optimer* bin include lib share

cpplint:
	python tools/cpplint.py --filter="-build/include,-runtime/reference,-readability/streams" `find src` 2>&1

cpplint-tst:
	python tools/cpplint.py --filter="-build/include,-runtime/reference,-readability/streams" $(TESTS_SRC)  2>&1
