#include <gl/glew.h>
#include <gl/freeglut.h>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include "Board.h"
#include "Shader.h"
#pragma comment(lib, "glew32.lib")

float eyeX = 60.0f, eyeY = 60.0f, eyeZ = 60.0f; // camera positions
bool win, lose = false; // for win / lose conditions
GLuint buffer[128] = {0}; // buffer for picking
GLuint screenHeight, screenWidth;
// for fps
int time;
int frame = 0;
int timebase = 0;
int fps = 0; 

// standalone components
Board board;
Shader shader;

// colours
double black[4] = {0, 0, 0, 1};
double white[4] = {1, 1, 1, 1};

// check the win / lose conditions
void checkConditions() {
	if (board.checkForWin()) {
		// show the win screen
		win = true;
		shader.doubleSpeed();
	} else if (board.checkForLose()) {
		// show the lose screen
		lose = true;
		shader.doubleSpeed();
	}
}

// check for glut special keys
void keySpecial(int key, int x, int y) {  
	switch(key) {
	case GLUT_KEY_RIGHT:
		board.rotate(true);
		break;
	case GLUT_KEY_LEFT:
		board.rotate(false);
		break;
	case GLUT_KEY_UP:
		if (eyeZ > 100.0f) break;

		eyeZ++;
		eyeX = eyeY -= 0.5f;
		break;
	case GLUT_KEY_DOWN:
		if (eyeZ < 55.0f) break;

		eyeZ--;
		eyeX = eyeY += 0.5f;
		break;
	}
}  

// keyboard controls
void keyboard(unsigned char key, int x, int y) {
	switch(key) {
	case 'r':
		board.init();
		win = lose = false;
		shader.normalSpeed();
		break;
	case 'w':
		board.setupWinCondition();
		checkConditions();
		break;
	case 'l':
		board.setupLoseCondition();
		checkConditions();
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

			checkConditions();
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

// draw a string to the screen
void drawString(std::string s, void * font) {
	glColor4dv(black);
	for (std::string::iterator i = s.begin(); i != s.end(); ++i) {
		char c = *i;
		glutBitmapCharacter(font, c);
	}
}

// setup some things needed for drawing text
void setUpTextMode() {
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0.0, screenWidth, 0.0, screenHeight);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
}

void stopTextMode() {
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

// draw the controls and informational text 
void drawText(void) {
	std::string strings[6];
	strings[0] = "Ross Warren's OpenGL Assignement";
	std::stringstream ss;
	ss << "Frames per second: " << fps << " Selected: " << board.getSelectedX() << ", " << board.getSelectedY() << " Remaining Pieces: " << board.getRemainingPieces();
	strings[1] = ss.str();
	strings[2] = "To play, select balls by clicking on them. Click an empty space to move it there.";
	strings[3] = "Clicking on other balls switches your selection.";
	strings[4] = "Try to only have one ball remaining in the centre. Press 'r' to restart if you get stuck";
	strings[5] = "Arrow keys: camera position and board rotation.";

	int height = 100;

	for (int i = 0; i < 6; i++) {
		glRasterPos2i(10, height -= 15);
		drawString(strings[i], GLUT_BITMAP_9_BY_15);
	}
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

// Render a stroke string for big letters
void renderStrokeString(void* font, const char* text, double colour [4]) {
	glPushMatrix();
	glColor4dv(colour);

	// move the text so that it is moved left on the x by half its width
	GLfloat length = (GLfloat)glutStrokeLength(font, (unsigned char*)text);
	glTranslatef(-length/2.0f, 0.0f, 0.0f);

	// move along the text, displaying it as it goes
	char* p;
	p = (char*)text;
	
	while (*p != '\0') {
		glutStrokeCharacter(font, *p++);
	}

	glPopMatrix();
}

// draw the winning screen. Draws once white, and once black but smaller
void drawWinScreen() {
	glTranslated(screenWidth / 2, screenHeight  - screenHeight / 5, 0);
	glLineWidth(6.0f);
	renderStrokeString(GLUT_STROKE_ROMAN, "You Win", white);
	glLineWidth(3.0f);
	renderStrokeString(GLUT_STROKE_ROMAN, "You Win", black);
}

// draw the lose condition screen.
void drawLoseScreen() {
	glLineWidth(5.0f);
	glPushMatrix();
	glTranslated(screenWidth / 2, screenHeight  - screenHeight / 5, 0);
	glLineWidth(6.0f);
	renderStrokeString(GLUT_STROKE_ROMAN, "You Lose", white);
	glLineWidth(3.0f);
	renderStrokeString(GLUT_STROKE_ROMAN, "You Lose", black);
	glPopMatrix();
	glPushMatrix();
	glTranslated(screenWidth / 2, screenHeight / 2 - (glutStrokeHeight(GLUT_STROKE_ROMAN) / 4), 0);
	glScalef(0.5f, 0.5f, 1.0f);
	glLineWidth(6.0f);
	renderStrokeString(GLUT_STROKE_ROMAN, "Press 'r' to Retry", white);
	glLineWidth(3.0f);
	renderStrokeString(GLUT_STROKE_ROMAN, "Press 'r' to Retry", black);
	glPopMatrix();
}

// setup the modelview perspective
void viewPerspective(void) {
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}


// draw the baclground with the shader
void drawBackground(void) {
	glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    ViewOrtho(screenWidth, screenHeight);

	shader.use();
	shader.updateTimeAndResolution(screenWidth, screenHeight, fps);
    
	glBegin(GL_QUADS);
    glVertex2d(0, 0);
    glVertex2d(0, screenHeight);
    glVertex2d(screenWidth, screenHeight);
    glVertex2d(screenWidth, 0);
    glEnd();
	
	shader.disable();
    viewPerspective();
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
}


// display EVERYTHING
void display() {
	// clear stuff
	glClearColor(0.2f, 0.2f, 0.2f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	drawBackground(); // draw the background
    glLoadIdentity();
	glPushMatrix();

	// setup camera matrix
	gluLookAt(eyeX, eyeY, eyeZ, 0.0f, 0.0f, 0.0f, 0, 0, 1);
	
	board.display(); // draw the board with pieces

	// dont use lighting for text and fps
	glDisable(GL_LIGHTING);
	calcFPS();

	// sets up stuff for drawing text
	setUpTextMode();
	drawText(); // draw the help and informational text
	
	// draw win or lose screen if conditions have been detected
	if (win) drawWinScreen();
	else if (lose) drawLoseScreen();

	stopTextMode();

	glEnable(GL_LIGHTING);
	glPopMatrix();

	glutSwapBuffers();
}

// do the picking at the specified coordinates
void doSelection(int x, int y) {
	GLint hits, view[4];
	
	// get viewport info
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


// mouse click handler
void mouseClick(int button, int state, int x, int y) {
   if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
	   doSelection(x, screenHeight-y);
   }
}

void init(void) {
	printf("Loading textures, building display lists \n");

	GLfloat pos[3] = {1024.0f, 800.0f, 0.0f};
	GLfloat ambient[3] = {0.1f, 0.1f, 0.1f};
	GLfloat diffuse[3] = {0.8f, 0.8f, 0.8f};
	GLfloat specular[4] = {1.0f, 1.0f, 1.0f, 1.0f};

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, pos);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);//less or equal
	glEnable(GL_NORMALIZE);
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_BLEND); //Enable alpha blending
	glEnable(GL_TEXTURE_2D);

	// tell opengl to put the selection data there
	glSelectBuffer(128, buffer);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //Set the blend function

	glShadeModel(GL_SMOOTH);
	board.init();
	shader.init();
	printf("Welcome to Ross's OpenGL Assigment \n");
}

// reshape called when the window is resized
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
	glutInitWindowSize(1024, 768);
	glutInitWindowPosition(400, 100);
	glutCreateWindow("Ross Warren's OpenGL Assignment");
	init();
	glutDisplayFunc(display);
	glutIdleFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(keySpecial);
	glutMouseFunc(mouseClick);
    glutMainLoop();

    return 0;
}