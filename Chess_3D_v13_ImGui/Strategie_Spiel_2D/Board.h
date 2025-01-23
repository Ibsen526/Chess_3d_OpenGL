#pragma once
//#include "Figure.h"

#include "Camera.h"
#include "Model.h"
#include "GUI.h"
#include "King.h"
#include "GameState.h"

#include <array>
#include <thread>

struct BoardData
{
	GameState gameState;
	GameState lastGameState;

	Uint16 boardState[8][8] = { //origBoard
		1, 2, 3, 4, 5, 3, 2, 1, //white
		0, 0, 0, 0, 0, 0, 0, 0,
		6, 6, 6, 6, 6, 6, 6, 6,
		6, 6, 6, 6, 6, 6, 6, 6,
		6, 6, 6, 6, 6, 6, 6, 6,
		6, 6, 6, 6, 6, 6, 6, 6,
		7, 7, 7, 7, 7, 7, 7, 7, //black
		8, 9, 10, 12, 11, 10, 9, 8
	};
	// 0 bauer weiß, 1 turm w, 2 springer w, 3 läufer w, 4 dame w, 5 könig w, 
	// 6 leer
	// 7 bauer schwarz, 8 turm s, 9 springer s, 10 läufer s, 11 dame s, 12 könig s
	std::vector<std::array<Uint16, 15>> lostFigures;
	Uint16 currentSelectedVal;
	glm::u16vec2 currentSelectedBoardPos;

	King kingPlayerOne;
	King kingPlayerTwo;

	bool currentPlayer = PLAYER_ONE;
	//0-5 player one, 7-12 player two
	glm::u16vec2 pawnEndPosition;
};

class Board
{
#define PLAYER_ONE true
#define PLAYER_TWO false
#define CHECK_KING true
#define NORMAL_CHECK false

public:
	Board();
	Board(Board* b);
	void DrawFullBoard(int mId, int vId, glm::mat4 V, int pId, glm::mat4 P, float time);
	bool SelectFigure(glm::vec3 chooseFigure, Camera* cam);
	bool MoveFigure(glm::vec3 chooseSpot, GLuint shaderIdGUI, Camera* cam);
	bool GetCurrentPlayer();
	void SetCurrentPlayer(Camera* cam);

	BoardData bd;

private:
	Model boardModel;
	Model selectionMarker;
	Model possibleCellMarker;
	std::vector<Model> lostFiguresModels;

	std::vector<Model> figures;
	glm::mat4 boardModelMatrix;

	GLuint boardTexId;
	GLuint whiteTexId;
	GLuint blackTexId;
	GLuint markerTexId;
	GLuint lostFiguresWhiteTexId;
	GLuint lostFiguresBlackTexId;

	bool MovePossible(glm::u16vec2 src, glm::u16vec2 dest, Uint16 currVal, Uint16(&boardStateP)[8][8], bool checkKing);
	bool MoveObstructed(short sourceX, short sourceY, short spotX, short spotY, Uint16(&boardStateP)[8][8]);
	bool IsCheckAvoidable(bool check, glm::u16vec2 src, glm::u16vec2 dest);
	bool VictoryCondition();
	void PawnChooseFigure(Uint16 spotX, Uint16 spotY);

};
