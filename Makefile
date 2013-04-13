INCLUDE = 
LIB = 
SRC = main.cpp data_loader.cpp
all:
	clang++ --std=c++11 --stdlib=libc++ -g -o optimer $(SRC)

clang:
	clang++ --std=c++11 --stdlib=libc++ -O2 -o optimer-clang $(SRC)

release:
	clang++ --std=c++11 --stdlib=libc++ -O2 -o optimer-clang $(SRC)
	clang++-mp-3.3 --std=c++11 --stdlib=libc++ -O2 -o optimer-clang++-mp-3.3 $(SRC)
	g++-mp-4.7 --std=c++11 -O2 -o optimer-g++-mp-4.7 main.cpp data_loader.cpp