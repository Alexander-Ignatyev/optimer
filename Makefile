INCLUDE = -Iinclude -Isrc
LIB = -Llib
SRC = src/main.cpp src/data_loader.cpp
TESTS_SRC = tests/atsp.cpp tests/teamcity_cppunit.cpp tests/teamcity_messages.cpp src/data_loader.cpp
all:
	clang++ $(INCLUDE) --std=c++11 --stdlib=libc++ -g -o optimer $(SRC)

clang:
	clang++ --std=c++11 --stdlib=libc++ -O2 -o optimer-clang $(SRC)

release:
	clang++ --std=c++11 --stdlib=libc++ -O2 -o optimer-clang $(SRC)
	clang++-mp-3.3 --std=c++11 --stdlib=libc++ -O2 -o optimer-clang++-mp-3.3 $(SRC)
	g++-mp-4.7 --std=c++11 -O2 -o optimer-g++-mp-4.7 main.cpp data_loader.cpp

tst:
	g++-mp-4.7 $(INCLUDE) $(LIB) -lcppunit --std=c++11 -g -o optimer-tests $(TESTS_SRC)

clean-all:
	rm -rf optimer* bin include lib share
