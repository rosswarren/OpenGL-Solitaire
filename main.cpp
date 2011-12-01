#include <gl/glew.h>
#include <gl/freeglut.h>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include "Board.h"
#include "Shader.h"
#pragma comment(lib, "glew32.lib")

float xpos = 512.0f, ypos = 512.0f, zpos = 512.0f, xrot = 758.0f, yrot = 238.0f;

GLuint screenHeight, screenWidth;
// for fps
int time;
int frame = 0;
int timebase = 0;
int fps = 0; 
Board board;
Shader shader;


// keyboard controls
void keyboard(unsigned char key, int x, int y) {
	switch(key) {
	case 'r':
		board.Init();
		break;
	}
}

void list_hits(GLint hits, GLuint *names) {
	int i;
	printf("%d hits \n", hits);

	for (i = 0; i < hits; i++) {
		// each hit is 4 bytes, name is last
		int name = names[(GLuint)(i * 4 + 3)];

		int selectedY = name / 10;
		int selectedX = name - selectedY * 10;

		selectedX -= 1;
		selectedY -= 1;

		// if moving a piece
		if (board.validSelection() && board.validLocation(selectedX, selectedY, board.EMPTY)) {
			printf("Moving Piece \n");
			board.movePiece(board.getSelectedX(), board.getSelectedY(), selectedX, selectedY);
		// if perhaps changing selected
		} else {
			board.setSelected(selectedX, selectedY);
			if (!board.validSelection()) {
				board.revertSelection();
			} else {
				printf("Selection Changed \n");
			}
		}
	}
}

/** draw a string to the screen */
void drawString(std::string s) {
	void * font = GLUT_BITMAP_9_BY_15;
	for (std::string::iterator i = s.begin(); i != s.end(); ++i) {
		char c = *i;
		glutBitmapCharacter(font, c);
	}
}


/** draw the controls and informational text */
void drawText(void) {
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0.0, screenWidth, 0.0, screenHeight);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	std::string strings[5];
	strings[0] = "Ross Warren's OpenGL Assignement";

	std::stringstream ss;
	ss << "Frames per second: " << fps << " Selected: " << board.getSelectedX() << ", " << board.getSelectedY() << " Remaining Pieces: " << board.getRemainingPieces();
	strings[1] = ss.str();

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	int height = 60;

	for (int i = 0; i < 2; i++) {
		glRasterPos2i(10, height -= 10);
		drawString(strings[i]);
	}

	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
}

/** calculate the fps */
void calcFPS() {
	frame++; // increment the frame count since the last check
	time = glutGet(GLUT_ELAPSED_TIME); 

	if (time - timebase > 50) { // check every 1/10th of a second
		fps = (int)(frame * 1000 / (time-timebase)); // set the calculated FPS
	 	timebase = time;
		frame = 0;
	}
}

void ViewOrtho(int x, int y) {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, x , y , 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
}

void ViewPerspective(void) {
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void display() {
	glClearColor(0.2f, 0.2f, 0.2f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    ViewOrtho(screenWidth, screenHeight);
	shader.use();
	shader.updateTimeAndResolution(screenWidth, screenHeight, fps);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(0, screenHeight);
    glVertex2f(screenWidth, screenHeight);
    glVertex2f(screenWidth, 0);
    glEnd();
	shader.disable();
    ViewPerspective();
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

    glLoadIdentity();

	glPushMatrix();
	gluLookAt(60.0f, 60.0f, 60.0f, 0.0f, 0.0f, 0.0f, 0, 0, 1);
	
	board.Display();

	glDisable(GL_LIGHTING);
	calcFPS();
	drawText(); // draw the help and informational text
	glEnable(GL_LIGHTING);
	glPopMatrix();

	glutSwapBuffers();
}

void doSelection(int x, int y) {
	GLuint buffer[128] = {0};

	GLint hits, view[4];

	// tell opengl to put the selection data there
	glSelectBuffer(128, buffer);
	
	// viewport info
	glGetIntegerv(GL_VIEWPORT, view);
	
	// selection mode for picking
	glRenderMode(GL_SELECT);
	
	// initialise the picking name stack
	glInitNames();
	
	// stop errors
	glPushName(-1);
	
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	
	gluPickMatrix(x, y, 2, 2, view); // area around mouse
	gluPerspective(60, (double)screenWidth / (double)screenHeight, 1.0f, 200.0f);

	glMatrixMode(GL_MODELVIEW);
	display();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	// number of hits from the click
	hits = glRenderMode(GL_RENDER);

	list_hits(hits, buffer);

	glMatrixMode(GL_MODELVIEW);
}

void mousedw(int x, int y, int but) {
	doSelection(x, screenHeight-y); // flip mouse coordinates
}

void mouseClick(int button, int state, int x, int y) {
   if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN)) {
	   mousedw(x, y, button);
   }
}

void Init(void) {
	printf("Loading textures, building display lists \n");

	GLfloat pos[3] = {1024.0f, 800.0f, 0.0f};
	GLfloat ambient[3] = {0.1f, 0.1f, 0.1f};
	GLfloat diffuse[3] = {0.8f, 0.8f, 0.8f};

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, pos);

	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);//less or equal
	glEnable(GL_NORMALIZE);
	glEnable(GL_BLEND); //Enable alpha blending
	glEnable(GL_TEXTURE_2D);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //Set the blend function

	glShadeModel(GL_SMOOTH);
	board.Init();
	shader.Init();
	printf("Welcome to Ross's OpenGL Assigment \n");
}

/** reshape called when the window is resized */
void reshape(int w, int h) {
	screenHeight = h;
	screenWidth = w;
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, (GLfloat)w / (GLfloat)h, 1.0, 200.0f);
	glMatrixMode(GL_MODELVIEW);
}

int main (int argc, char **argv) {
    glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA | GLUT_MULTISAMPLE);
	glutInitWindowSize(800, 640);
	glutInitWindowPosition(400, 100);
	glutCreateWindow("Ross Warren's OpenGL Assignment");
	Init();
	glutDisplayFunc(display);
	glutIdleFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouseClick);
    glutMainLoop();

    return 0;
}