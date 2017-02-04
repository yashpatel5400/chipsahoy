#include <GL/glut.h>
#include "chip.h"

Chip chip8;

// since Chip8 only supports 64x32 graphics output, we have to introduce
// a stretch factor to properly draw it on-screen
// each STRETCH x STRETCH block now represents one pixel
int STRETCH = 10;

// sets up the keyboard bindings -- one for the press and
// one for release (from http://www.swiftless.com/tutorials/opengl/keyboard.html)	
void keyPressed (unsigned char key, int x, int y) {  
	/* Use the keyboard <-> keypad the layout given in tutorial
	Keypad                   Keyboard
	+-+-+-+-+                +-+-+-+-+
	|1|2|3|C|                |1|2|3|4|
	+-+-+-+-+                +-+-+-+-+
	|4|5|6|D|                |Q|W|E|R|
	+-+-+-+-+       =>       +-+-+-+-+
	|7|8|9|E|                |A|S|D|F|
	+-+-+-+-+                +-+-+-+-+
	|A|0|B|F|                |Z|X|C|V|
	+-+-+-+-+                +-+-+-+-+
	*/

	if (key == '1')      chip8.keys[0x0] = 1;
	else if (key == '2') chip8.keys[0x1] = 1;
	else if (key == '3') chip8.keys[0x2] = 1;
	else if (key == '4') chip8.keys[0x3] = 1;

	else if (key == 'q') chip8.keys[0x4] = 1;
	else if (key == 'w') chip8.keys[0x5] = 1;
	else if (key == 'e') chip8.keys[0x6] = 1;
	else if (key == 'r') chip8.keys[0x7] = 1;

	else if (key == 'a') chip8.keys[0x8] = 1;
	else if (key == 's') chip8.keys[0x9] = 1;
	else if (key == 'd') chip8.keys[0xA] = 1;
	else if (key == 'f') chip8.keys[0xB] = 1;

	else if (key == 'z') chip8.keys[0xC] = 1;
	else if (key == 'x') chip8.keys[0xD] = 1;
	else if (key == 'c') chip8.keys[0xE] = 1;
	else if (key == 'v') chip8.keys[0xF] = 1;
}

void keyUp (unsigned char key, int x, int y) {  
	if (key == '1')      chip8.keys[0x0] = 0;
	else if (key == '2') chip8.keys[0x1] = 0;
	else if (key == '3') chip8.keys[0x2] = 0;
	else if (key == '4') chip8.keys[0x3] = 0;

	else if (key == 'q') chip8.keys[0x4] = 0;
	else if (key == 'w') chip8.keys[0x5] = 0;
	else if (key == 'e') chip8.keys[0x6] = 0;
	else if (key == 'r') chip8.keys[0x7] = 0;

	else if (key == 'a') chip8.keys[0x8] = 0;
	else if (key == 's') chip8.keys[0x9] = 0;
	else if (key == 'd') chip8.keys[0xA] = 0;
	else if (key == 'f') chip8.keys[0xB] = 0;

	else if (key == 'z') chip8.keys[0xC] = 0;
	else if (key == 'x') chip8.keys[0xD] = 0;
	else if (key == 'c') chip8.keys[0xE] = 0;
	else if (key == 'v') chip8.keys[0xF] = 0;
}  

void drawPixel(int x, int y) {
	glBegin(GL_QUADS);
		glVertex3f(x * STRETCH,        y * STRETCH,      0.0f);
		glVertex3f(x * STRETCH,       (y + 1) * STRETCH, 0.0f);
		glVertex3f((x + 1) * STRETCH, (y + 1) * STRETCH, 0.0f);
		glVertex3f((x + 1) * STRETCH,  y * STRETCH,      0.0f);
	glEnd();
}

void display() {  
	// sees the update of the emulator in current cycle
	chip8.emulateCycle();
	printf("emulated\n");

	// if the refresh flag has reached refresh, updates the screen
	if (chip8.drawFlag) {
		glClear(GL_COLOR_BUFFER_BIT); //Clear the colour buffer (more buffers later on)  
		
		for (int y = 0; y < chip8.height; y++) {
			for (int x = 0; x < chip8.width; x++) {
				if (chip8.gfx[y * chip8.width + x] == 1)
					glColor3f(1.0f, 1.0f, 1.0f);
				else glColor3f(0.0f, 0.0f, 0.0f);

				drawPixel(x, y);
			}
		}

		glutSwapBuffers(); // Flush the OpenGL buffers to the window 
		chip8.drawFlag = false;
	}
}

void setupGraphics(int *argc, char **argv) {
	int width  = chip8.width  * STRETCH;
	int height = chip8.height * STRETCH;
	int x = 640;
	int y = 640;

	// based on http://www.swiftless.com/tutorials/opengl/keyboard.html code
	glutInit(argc, argv);
	glutInitWindowPosition(width, height);
	glutInitWindowSize(x, y);
	glutInitDisplayMode (GLUT_RGBA | GLUT_DOUBLE);
	glutCreateWindow("ChipsAhoy"); 

	// Tell GLUT to use the method "display" for rendering  
	glutDisplayFunc(display); 

	// Tell GLUT to use the method "keyPressed" for key press/release 
	glutKeyboardFunc(keyPressed);
	glutKeyboardUpFunc(keyUp);   
	
	glutMainLoop(); // Enter GLUT's main loop  
}

int main(int argc, char **argv) {
	// initializes the chip and loads the game
	chip8.initialize();
	chip8.loadGame((char*) "invaders.c8");

	// set up graphics and input
	setupGraphics(&argc, argv);
	return 0;
}