all: make_inter

make_inter:
	g++ -o out.out -std=c++17 -g -Wall interpreter.cpp
