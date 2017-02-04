chipsdl: main.cpp chip.cpp
	g++ emulator.cpp chip.cpp -lGL -lGLU -lglut -o chip8

chipglut: emulator.cpp chip.cpp
	g++ emulator.cpp chip.cpp -lGL -lGLU -lglut -o chip8
	
clean:
	rm chip8