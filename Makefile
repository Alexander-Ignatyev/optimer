INCLUDE = $(HOME)/usr/include
LIB = 
all:
	clang++ -I$(INCLUDE) -g -o optimer main.cpp
#	g++ -O2 -o optimer main.cpp -fopenmp
#	icpc -xc++ -O2 -o optimer main.cpp -openmp