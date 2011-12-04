#include "Board.h"

// initialise all the variables
Board::Board(void) {
	highlightAngle = boardRotation = 0.0f;
	selectedX = selectedY = lastSelectedX = lastSelectedY = 0;
}

// destructor
Board::~Board(void) {

}

// gets the number of pieces remaining on the board
int Board::getRemainingPieces(void) {
	int count = 0;

	for (int y = 0; y < 7; y++) {
		for (int x = 0; x < 7; x++) {
			if (places[x][y] == PIECE) {
				count++;
			}
		}
	}

	return count;
}

// checks if the current selection is valid
bool Board::validSelection(void) {
	return validLocation(selectedX, selectedY, PIECE);
}

// checks if the specified location is the specified type and if it is valid
bool Board::validLocation(int x, int y, Place type) {
	if (x < 0 || y < 0 || x > 6 || y > 6) {
		return false;
	}

	// check it is of the specified type
	if (places[x][y] != type) {
		return false;
	}

	// check it is not going ouside the array
	if (x > 6 || x < 0 || y > 6 || y < 0) {
		return false;
	}

	return true;
}

// initialise the board
void Board::init() {
	quadric = gluNewQuadric();
	gluQuadricNormals(quadric, GL_TRUE);
	gluQuadricTexture(quadric, 1);

	RawLoader rawLoader;
	woodTexture = rawLoader.LoadTextureRAW("woodtexture.raw", 1, 400, 400);
	ballTexture = rawLoader.LoadTextureRAW("crystal.raw", 1, 512, 512);
	selectedX = selectedY = lastSelectedX = lastSelectedY = 0;

	// set all to forbidden
	for (int y = 0; y < 7; y++) {
		for (int x = 0; x < 7; x++) {
			places[x][y] = FORBIDDEN;
		}
	}

	// do the pieces
	for (int y = 2; y < 5; y++) {
		for (int x = 0; x < 7; x++) {
			places[x][y] = PIECE;
		}
	}

	for (int y = 0; y < 7; y++) {
		for (int x = 2; x < 5; x++) {
			places[x][y] = PIECE;
		}
	}

	// remove centre piece
	places[3][3] = EMPTY;
}

// runs all the checks in preperation for moving a piece
bool Board::tryMovePiece(int fromX, int fromY, int toX, int toY, bool suppressText) {
	if (!validLocation(toX, toY, EMPTY)) {
		if (!suppressText) printf("Invalid target \n");
		return false;
	}

	if (!validLocation(fromX, fromY, PIECE)) {
		if (!suppressText) printf("Invalid Piece \n");
		return false;
	}

	// check from and to are not the same
	if (toX == fromX && toY == fromY) {
		if (!suppressText) printf("Piece and target are the same \n");
		return false;
	}

	bool posX = false, posY = false;

	// if moving from left to right
	if (toX > fromX) {
		posX = true;
	}

	// if moving from top to bottom
	if (toY > fromY) {
		posY = true;
	}

	int offsetX = fromX - toX;
	int offsetY =  fromY - toY;

	// check it moved
	if (offsetX != 2 && offsetY != 2 && offsetX != -2 && offsetY != -2) {
		if (!suppressText) printf("No offset is 2 \n");
		return false;
	}

	// check it only moved on one axis
	if (offsetX != 0 && offsetY != 0) {
		if (!suppressText) printf("No offset is 0 \n");
		return false;
	}

	// check it is jumping a piece
	if (places[toX + offsetX / 2][toY + offsetY / 2] != PIECE) {
		if (!suppressText) printf("Not Jumping a piece \n");
		return false;
	}

	return true;
}

// moves a piece with full validation
bool Board::movePiece(int fromX, int fromY, int toX, int toY) {
	if (!tryMovePiece(fromX, fromY, toX, toY, false)) {
		return false;
	}

	int offsetX = fromX - toX;
	int offsetY =  fromY - toY;

	// empty departure place
	places[fromX][fromY] = EMPTY;

	//fill destination place
	places[toX][toY] = PIECE;

	//remove jumped piece
	places[toX + offsetX / 2][toY + offsetY / 2] = EMPTY;

	// change selected to moved piece
	setSelected(toX, toY);

	return true;
}

// get the selected x coordinate
int Board::getSelectedX(void) {
	return selectedX;
}

// get the selected y coordinate
int Board::getSelectedY(void) {
	return selectedY;
}

// set the selected coordinates (remembers one previous selection)
void Board::setSelected(int x, int y) {
	lastSelectedX = selectedX;
	lastSelectedY = selectedY;
	selectedX = x;
	selectedY = y;
	highlightAngle = 0;
}

// set up a lose condition for testing
void Board::setupLoseCondition() {
	for (int y = 0; y < 7; y++) {
		for (int x = 0; x < 7; x++) {
			places[x][y] = EMPTY;
		}
	}

	places[3][3] = PIECE;
	places[3][5] = PIECE;
}

// set up a win condition for testing
void Board::setupWinCondition() {
	for (int y = 0; y < 7; y++) {
		for (int x = 0; x < 7; x++) {
			places[x][y] = EMPTY;
		}
	}

	places[3][3] = PIECE;
}

// revert the selection to the saved values
void Board::revertSelection(void) {
	selectedX = lastSelectedX;
	selectedY = lastSelectedY;
}

// check for a win condition
bool Board::checkForWin(void) {
	if (getRemainingPieces() == 1) {
		return true;
	} else {
		return false;
	}
}

// rotate the board
void Board::rotate(bool right) {
	boardRotation += right? 0.5f : -0.5f;
}

// check for a lose condition
bool Board::checkForLose(void) {
	for (int y = 0; y < 7; y++) {
		for (int x = 0; x < 7; x++) {
			if (validLocation(x, y, PIECE)) {
				if (tryMovePiece(x, y, x, y - 2, true) || tryMovePiece(x, y, x, y + 2, true) || tryMovePiece(x, y, x + 2, y, true) || tryMovePiece(x, y, x - 2, y, true)) {
					return false;
				}
			}
		}
	}

	return true;
}

// display the board
void Board::display() {
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	glRotatef(boardRotation, 0.0, 0.0, 1.0f);
	Shapes shapes;
	glBindTexture(GL_TEXTURE_2D, woodTexture);
	shapes.cylinder(5, 50, 50, 200, 1, quadric);
	glTranslatef(0.0f, 0.0f, 6.0f);

	GLfloat colour [4] = {1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat fadeColour [4] = {1.0f, 1.0f, 1.0f, 0.35f};

	glTranslatef(-36.0, -36.0f, 0.0f);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 120);

	glMatrixMode(GL_TEXTURE);
	glPushMatrix();
	glLoadIdentity();
	srand(1);
	
	for (GLuint y = 0; y < 7; y++) {
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		// move to the appropriate position for the row
		glTranslatef(0.0f, 12.0f * y, 0.0f);
		glMatrixMode(GL_TEXTURE);

		for (GLuint x = 0; x < 7; x++) {
			glMatrixMode(GL_TEXTURE);
			glLoadIdentity();
			glTranslatef(rand() % 100 / 100.0f, rand() % 100 / 100.0f, 0.0); // change the texture coords so all the balls look different
			glMatrixMode(GL_MODELVIEW);

			glPushMatrix();

			// load name for picking
			glLoadName((GLuint)((y + 1) * 10) + x + 1);

			// move to the appropriate position for the ball
			glTranslatef(12.0f * x, 0.0f, 0.0f);

			// disable textures
			glDisable(GL_TEXTURE_2D);

			// switch to the ball texture
			glBindTexture(GL_TEXTURE_2D, ballTexture);

			if (places[x][y] == PIECE) {
				// enable textures if it is a piece
				glEnable(GL_TEXTURE_2D);

				// check if it is the currently selected one
				if (x == selectedX && y == selectedY) {
					highlightAngle += 0.01f; // angle for rotating selected ball
					glMatrixMode(GL_TEXTURE);
					glTranslatef(highlightAngle, 0, 0); // spin the ball by manipulating texture coords
					glMatrixMode(GL_MODELVIEW);
				}

				// draw the ball
				gluSphere(quadric, 4.0f, 30, 30); 
			} else if (places[x][y] == EMPTY) {
				// switch to a semi transparent colour
				glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, fadeColour);
				gluSphere(quadric, 4.0f, 30, 30); // draw the ball
				glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, colour);
			}
			glPopMatrix();
		}

		glPopMatrix();
	}

	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
}