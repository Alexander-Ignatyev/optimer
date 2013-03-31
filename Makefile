INCLUDE = $(HOME)/usr/include
LIB = 
SRC = main.cpp data_loader.cpp
all:
	clang++ -I$(INCLUDE) --std=c++11 --stdlib=libc++ -g -o optimer $(SRC)