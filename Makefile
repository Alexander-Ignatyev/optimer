INCLUDE = 
LIB = 
SRC = main.cpp data_loader.cpp
all:
	clang++ --std=c++11 --stdlib=libc++ -g -o optimer $(SRC)