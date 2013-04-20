INCLUDE = -Iinclude -Isrc
LIB = -Llib
SRC = src/data_loader.cpp src/tsp.cpp src/stats.cpp
TESTS_SRC = tests/main.cpp tests/ap.cpp tests/atsp.cpp
TEAMCITY_TESTS_SRC = tests/teamcity_cppunit.cpp tests/teamcity_messages.cpp

all:
	clang++ $(INCLUDE) --std=c++11 --stdlib=libc++ -g -o optimer src/main.cpp $(SRC)

clang:
	clang++ --std=c++11 --stdlib=libc++ -O2 -o optimer-clang src/main.cpp $(SRC)

clang-33:
	clang++-mp-3.3 --std=c++11 --stdlib=libc++ -O2 -o optimer-clang++-mp-3.3 src/main.cpp $(SRC)

gcc-47:
	g++-mp-4.7 --std=c++11 -O2 -o optimer-g++-mp-4.7 $(SRC)

tst:
	g++-mp-4.7 $(INCLUDE) $(LIB) -lcppunit --std=c++11 -O2 -o optimer-tests $(TESTS_SRC) $(TEAMCITY_TESTS_SRC) $(SRC)

clean-all:
	rm -rf optimer* bin include lib share

cpplint:
	python tools/cpplint.py --filter="-build/include,-runtime/reference,-readability/streams" `find src` 2>&1

cpplint-tst:
	python tools/cpplint.py --filter="-build/include,-runtime/reference,-readability/streams" $(TESTS_SRC)  2>&1
