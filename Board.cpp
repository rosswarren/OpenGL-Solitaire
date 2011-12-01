#include "Board.h"


Board::Board(void) {
	selectedX = selectedY = lastSelectedX = lastSelectedY = 0;
}


Board::~Board(void) {
}

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

bool Board::validSelection(void) {
	return validLocation(selectedX, selectedY, PIECE);
}

bool Board::validLocation(int x, int y, Place type) {
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


void Board::Init() {
	quadric = gluNewQuadric();
	gluQuadricNormals(quadric, GL_TRUE);
	gluQuadricTexture(quadric, 1);

	RawLoader rawLoader;
	woodTexture = rawLoader.LoadTextureRAW("woodtexture.raw", 1, 400, 400);

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

bool Board::movePiece(int fromX, int fromY, int toX, int toY) {
	if (!validLocation(toX, toY, EMPTY)) {
		printf("Invalid target \n");
		return false;
	}

	if (!validLocation(fromX, fromY, PIECE)) {
		printf("Invalid Piece \n");
		return false;
	}

	// check from and to are not the same
	if (toX == fromX && toY == fromY) {
		printf("Piece and target are the same \n");
		return false;
	}

	bool posX = false;
	bool posY = false;

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
		printf("No offset is 2 \n");
		return false;
	}

	// check it only moved on one axis
	if (offsetX != 0 && offsetY != 0) {
		printf("No offset is 0 \n");
		return false;
	}

	// check it is jumping a piece
	if (places[toX + offsetX / 2][toY + offsetY / 2] != PIECE) {
		printf("Not Jumping a piece \n");
		return false;
	}

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

int Board::getSelectedX(void) {
	return selectedX;
}

int Board::getSelectedY(void) {
	return selectedY;
}

void Board::setSelected(int x, int y) {
	lastSelectedX = selectedX;
	lastSelectedY = selectedY;
	selectedX = x;
	selectedY = y;
}

void Board::revertSelection(void) {
	selectedX = lastSelectedX;
	selectedY = lastSelectedY;
}

void Board::Display() {
	glPushMatrix();

	Shapes shapes;

	glBindTexture(GL_TEXTURE_2D, woodTexture);
	shapes.cylinder(5, 50, 50, 200, 1, quadric);

	glTranslatef(0.0f, 0.0f, 6.0f);

	GLfloat colour [4] = {1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat fadeColour [4] = {1.0f, 1.0f, 1.0f, 0.35f};
	GLfloat highlightColour [4] = {0.0f, 1.0f, 0.0f, 1.0f};

	glTranslatef(-36.0, -36.0f, 0.0f);
	
	for (GLuint y = 0; y < 7; y++) {
		glPushMatrix();
		glTranslatef(0.0f, 12.0f * y, 0.0f);

		for (GLuint x = 0; x < 7; x++) {
			glPushMatrix();
			glLoadName((GLuint)((y + 1) * 10) + x + 1);

			glTranslatef(12.0f * x, 0.0f, 0.0f);

			glDisable(GL_TEXTURE_2D);

			if (places[x][y] == PIECE) {
				if (x == selectedX && y == selectedY) {
					glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, highlightColour);
				}

				glutSolidSphere(4.0f, 30, 30);
				glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, colour);
			} else if (places[x][y] == EMPTY) {
				glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, fadeColour);
				glMateriali(GL_FRONT, GL_SHININESS, 100);
				glutSolidSphere(4.0f, 30, 30);
				glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, colour);
			}
			glPopMatrix();

			glEnable(GL_TEXTURE_2D);
		}

		glPopMatrix();
	}

	glPopMatrix();
}