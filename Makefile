INCLUDE = $(HOME)/usr/include
LIB = 
all:
	clang++ -I$(INCLUDE) --std=c++11 --stdlib=libc++ -g -o optimer main.cpp
#	g++ -O2 -o optimer main.cpp -fopenmp
#	icpc -xc++ -O2 -o optimer main.cpp -openmp