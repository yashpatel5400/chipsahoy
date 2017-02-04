chip: main.cpp chip.cpp
	g++ main.cpp chip.cpp -lGL -lGLU -lglut -lSDL2 -std=gnu++11 -o chip8

chip8: main8.cpp chip8.cpp
	g++ main8.cpp chip8.cpp -lGL -lGLU -lglut -lSDL2 -std=gnu++11 -o chip8
	
clean:
	rm chip8