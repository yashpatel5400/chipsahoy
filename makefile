chip: main.cpp chip.cpp
	g++ main.cpp chip.cpp -lGL -lGLU -lglut -lSDL2 -std=gnu++11 -o chip

clean:
	rm chip8