FLAGS	:= -std=c++17 -ffast-math -Wall -Wextra -pedantic
LIBS	:= -lX11

all:
	g++ -o main main.cpp -O3 -march=native $(FLAGS) $(LIBS)

debug:
	g++ -o main main.cpp -ggdb $(FLAGS) $(LIBS)

clean:
	rm -f main
