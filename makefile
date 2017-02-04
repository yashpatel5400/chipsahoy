chip: emulator.cpp chip.cpp
	g++ emulator.cpp chip.cpp -lGL -lGLU -lglut -o chip8