#pragma once
#include "Shader.h"
#include <gl/freeglut.h>
#include <iostream>
#include "RawLoader.h"
#include "Shapes.h"

class Board
{
public:
	Board(void);
	~Board(void);

	enum Place {FORBIDDEN, EMPTY, PIECE};
	void Display(Shader shader);
	void Init(void);
	bool movePiece(int fromX, int fromY, int toX, int toY);
	void setSelected(int x, int y);
	void revertSelection(void);
	int getSelectedX(void);
	int getSelectedY(void);
	bool validLocation(int x, int y, Place type);
	bool validSelection(void);
	int getRemainingPieces(void);
private:
	Place places[7][7];
	int selectedX;
	int selectedY;
	int lastSelectedX;
	int lastSelectedY;
	GLuint woodTexture;
	GLUquadricObj* quadric;
};

