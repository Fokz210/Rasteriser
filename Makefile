all:
	g++ -o main main.cpp -O3 -march=native -ffast-math
debug:
	g++ -o main main.cpp -ggdb -ffast-math 	
