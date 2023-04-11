all: make_inter

make_inter:
	g++ -o interpreter.out -std=c++17 -Wall interpreter.cpp
