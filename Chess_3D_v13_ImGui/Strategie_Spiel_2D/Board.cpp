#include "Board.h"

Board::Board()
{
	//Init the board
	boardModel = Model(ASSET_PATH"Board/chessBoard.obj", true);
	boardModel.InitBuffers();

	Model::InitTextures(ASSET_PATH"Board/chessBoardTex.png", boardTexId, GL_RGB, true);

	const int sizeFigures = 7;
	figures = { Model(), Model(), Model(), Model(), Model(), Model(), Model() };
	std::thread createFiguresThread[sizeFigures] = {
		std::thread([&]() { figures[0] = Model(ASSET_PATH"Figures/pawn.mobj", false, {0.0f,-0.7f}); }),
		std::thread([&]() { figures[1] = Model(ASSET_PATH"Figures/rook.mobj", false, { 0.0f,-0.7f }); }),
		std::thread([&]() { figures[2] = Model(ASSET_PATH"Figures/knight.mobj", false, { 0.0f,-0.7f }); }),
		std::thread([&]() { figures[3] = Model(ASSET_PATH"Figures/bishop.mobj", false, { 0.0f,-0.7f }); }),
		std::thread([&]() { figures[4] = Model(ASSET_PATH"Figures/queen.mobj", false, { 0.0f,-0.7f }); }),
		std::thread([&]() { figures[5] = Model(ASSET_PATH"Figures/king.mobj", false, { 0.0f,-0.7f }); }),
		std::thread([&]() { figures[6] = Model(ASSET_PATH"Figures/knightRot.mobj", false, { 0.0f,-0.7f }); })
	};

	for (Uint8 i = 0; i < sizeFigures; i++)
		createFiguresThread[i].join();

	for (Model& m : figures)
		m.InitBuffers();

	Model::InitTextures(ASSET_PATH"Figures/whiteTex.png", whiteTexId, GL_RGB, true);
	Model::InitTextures(ASSET_PATH"Figures/blackTex.png", blackTexId, GL_RGB, true);


	float tempOffsetBoard = 8.4f / 2.0f - 0.2f;

	//Init the board model matrix
	boardModelMatrix = glm::mat4(1.0f);
	boardModelMatrix = glm::translate(boardModelMatrix, glm::vec3(tempOffsetBoard, 0.0f, tempOffsetBoard)); //3.7
	//boardModelMatrix = glm::scale(boardModelMatrix, glm::vec3(1.0f)); //0.755

	//Init the marker models
	selectionMarker = Model(ASSET_PATH"Markers/selectionMarker.obj", true);
	selectionMarker.InitBuffers();
	possibleCellMarker = Model(ASSET_PATH"Markers/possibleMarker.obj", true);
	possibleCellMarker.InitBuffers();

	Model::InitTextures(ASSET_PATH"Markers/markerTex.png", markerTexId, GL_RGB, true);

	//Init the lost figures 
	lostFiguresModels.push_back(Model(ASSET_PATH"Board/lostFiguresDisplay.obj", true));
	lostFiguresModels.push_back(Model(ASSET_PATH"Board/lostFiguresDisplay.obj", true));
	lostFiguresModels[0].InitBuffers();
	lostFiguresModels[1].InitBuffers();

	Model::InitTextures(ASSET_PATH"Board/blackFiguresGround.png", lostFiguresBlackTexId, GL_RGB, true);
	Model::InitTextures(ASSET_PATH"Board/whiteFiguresGround.png", lostFiguresWhiteTexId, GL_RGB, true);

	bd.lostFigures.push_back({ 6,6,6,6,6,6,6,6,6,6,6,6,6,6,6 });
	bd.lostFigures.push_back({ 6,6,6,6,6,6,6,6,6,6,6,6,6,6,6 });

	//Init the other variables
	bd.currentSelectedVal = 6; //nothing
	bd.pawnEndPosition = glm::u16vec2(0.0f);

	for (Uint16 i = 0; i < 8; i++)
	{
		for (Uint16 j = 0; j < 8; j++)
		{
			if (bd.boardState[i][j] == 5)
				bd.kingPlayerOne = King(i, j);
			else if (bd.boardState[i][j] == 12)
				bd.kingPlayerTwo = King(i, j);
		}
	}
}

Board::Board(Board* b)
{
	bd.lostFigures.push_back({ 6,6,6,6,6,6,6,6,6,6,6,6,6,6,6 });
	bd.lostFigures.push_back({ 6,6,6,6,6,6,6,6,6,6,6,6,6,6,6 });

	//Init the other variables
	bd.currentSelectedVal = 6; //nothing
	bd.pawnEndPosition = glm::u16vec2(0.0f);

	for (Uint16 i = 0; i < 8; i++)
	{
		for (Uint16 j = 0; j < 8; j++)
		{
			if (bd.boardState[i][j] == 5)
				bd.kingPlayerOne = King(i, j);
			else if (bd.boardState[i][j] == 12)
				bd.kingPlayerTwo = King(i, j);
		}
	}
}

void Board::DrawFullBoard(int mId, int vId, glm::mat4 V, int pId, glm::mat4 P, float time)
{
	boardModel.DrawModel(boardTexId, mId, boardModelMatrix, vId, V, pId, P);

	//Draw the lost figure sides
	for (Uint8 i = 0; i < lostFiguresModels.size(); i++)
	{
		glm::mat4 figureModelMatrix = glm::mat4(1.0f);
		float boardHalf = 8.4f / 2.0f - 0.2f;
		figureModelMatrix = glm::translate(figureModelMatrix, glm::vec3(8.2f, 0.0f,
			(i == 0) ? (boardHalf - 6.0f) : (boardHalf + 6.0f)));
		//figureModelMatrix = glm::scale(figureModelMatrix, glm::vec3(0.9f));
		lostFiguresModels[i].DrawModel((i == 0) ? (lostFiguresWhiteTexId) : (lostFiguresBlackTexId), mId, figureModelMatrix, vId, V, pId, P);
	}

	//Draw the board figures
	float scalingVal = 0.25f;

	for (Uint16 i = 0; i < 8; i++)
	{
		for (Uint16 j = 0; j < 8; j++)
		{
			Uint16 currentFigure = bd.boardState[i][j];

			glm::mat4 modelMatrix = glm::mat4(1.0f);
			modelMatrix = glm::translate(modelMatrix, glm::vec3((float)i + 0.5f, scalingVal, (float)j + 0.5f));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(scalingVal)); //2er cube hat nur noch 0.4 pro seite


			if (currentFigure < 6)
			{
				figures[currentFigure].DrawModel(whiteTexId, mId, modelMatrix, vId, V, pId, P);
			}
			else if (currentFigure > 6)
			{
				Uint16 currFigureWithRotation = currentFigure - 7;
				if (currFigureWithRotation == 2)
					currFigureWithRotation = 6;
				figures[currFigureWithRotation].DrawModel(blackTexId, mId, modelMatrix, vId, V, pId, P);
			}
						
			//Draw a marker above the currently selected position
			if ((Uint16)bd.currentSelectedBoardPos.x == i && (Uint16)bd.currentSelectedBoardPos.y == j && bd.currentSelectedVal != 6)
			{
				glm::mat4 selectionModelMatrix = glm::mat4(1.0f);
				selectionModelMatrix = glm::translate(selectionModelMatrix, glm::vec3((float)i + 0.5f, 1.5f + sinf(time) / 10.0f, (float)j + 0.5f));
				selectionModelMatrix = glm::scale(selectionModelMatrix, glm::vec3(0.1f));
				selectionMarker.DrawModel(markerTexId, mId, selectionModelMatrix, vId, V, pId, P);
			}
			//Draw another marker above the possible positions
			else if (MovePossible(bd.currentSelectedBoardPos, glm::u16vec2(i, j), bd.currentSelectedVal, bd.boardState, NORMAL_CHECK) &&
				bd.currentSelectedVal != 6 && bd.boardState[i][j] != 5 && bd.boardState[i][j] != 12)
			{
				glm::mat4 possibleModelMatrix = glm::mat4(1.0f);
				//possibleModelMatrix = glm::scale(possibleModelMatrix, glm::vec3(1.0f));
				possibleModelMatrix = glm::translate(possibleModelMatrix, glm::vec3((float)i + 0.5f, 0.0f, (float)j + 0.5f));
				possibleCellMarker.DrawModel(markerTexId, mId, possibleModelMatrix, vId, V, pId, P);
			}

		}
	}

	//Draw lost figures
	float boardHalf = 8.4f / 2.0f - 0.2f;
	for (Uint16 i = 0; i < 15; i++)
	{
		Uint16 currentFigure = bd.lostFigures[0][i];

		if (currentFigure != 6)
		{
			glm::mat4 modelMatrix = glm::mat4(1.0f);
			if (i < 8)
				modelMatrix = glm::translate(modelMatrix, glm::vec3((float)i + 0.5f, 0.2f, boardHalf - 5.5f));
			else
				modelMatrix = glm::translate(modelMatrix, glm::vec3(((float)i - 8 + 0.5f), 0.2f, boardHalf - 6.5f));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(scalingVal)); 

			figures[currentFigure].DrawModel(whiteTexId, mId, modelMatrix, vId, V, pId, P);
		}
	}

	for (Uint16 i = 0; i < 15; i++)
	{
		Uint16 currentFigure = bd.lostFigures[1][i];

		if (currentFigure != 6)
		{
			glm::mat4 modelMatrix = glm::mat4(1.0f);
			if (i < 8)
				modelMatrix = glm::translate(modelMatrix, glm::vec3(8.0f - ((float)i + 0.5f), 0.2f, boardHalf + 5.5f));
			else
				modelMatrix = glm::translate(modelMatrix, glm::vec3(8.0f - ((float)i - 8 + 0.5f), 0.2f, boardHalf + 6.5f));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(scalingVal)); 

			figures[currentFigure - 7].DrawModel(blackTexId, mId, modelMatrix, vId, V, pId, P);
		}
	}
}

bool Board::SelectFigure(glm::vec3 chooseFigure, Camera* cam)
{
	int figureX = (int)floor(chooseFigure.x);
	int figureZ = (int)floor(chooseFigure.z);

	if (!bd.gameState.getBackLostFigure && figureX >= 0 && figureX <= 7 && figureZ >= 0 && figureZ <= 7)
	{
		if (bd.currentPlayer == PLAYER_ONE && bd.boardState[figureX][figureZ] < 6)
		{
			bd.currentSelectedVal = bd.boardState[figureX][figureZ];
			bd.currentSelectedBoardPos = glm::vec2(figureX, figureZ);
		}
		else if (bd.currentPlayer == PLAYER_TWO && bd.boardState[figureX][figureZ] > 6)
		{
			bd.currentSelectedVal = bd.boardState[figureX][figureZ];
			bd.currentSelectedBoardPos = glm::vec2(figureX, figureZ);
		}
	}
	else if (bd.gameState.getBackLostFigure && bd.currentPlayer && //white lost selection
		figureX >= 0 && figureX <= 7 && figureZ >= -3 && figureZ <= -2 
		&& !(figureX == 7 && figureZ == -3))
	{
		Uint16 selectedLostFigure = bd.lostFigures[0][figureX + 8 * abs(figureZ + 2)];
		if (selectedLostFigure != 6)
		{
			bd.boardState[bd.pawnEndPosition.x][bd.pawnEndPosition.y] = selectedLostFigure;
			bd.lostFigures[0][figureX + 8 * abs(figureZ + 2)] = 6;
			bd.gameState.getBackLostFigure = 0;
			bd.gameState.nothing = 1;

			SetCurrentPlayer(cam);

			if (VictoryCondition())
				return true;
		}
	}
	else if (bd.gameState.getBackLostFigure && !bd.currentPlayer && //black lost selection
		figureX >= 0 && figureX <= 7 && figureZ >= 9 && figureZ <= 10 
		&& !(figureX == 0 && figureZ == 10))
	{
		int reversedFX = 7 - figureX;

		Uint16 selectedLostFigure = bd.lostFigures[1][reversedFX + 8 * abs(figureZ - 9)];
		if (selectedLostFigure != 6)
		{
			bd.boardState[bd.pawnEndPosition.x][bd.pawnEndPosition.y] = selectedLostFigure;
			bd.lostFigures[1][reversedFX + 8 * abs(figureZ - 9)] = 6;
			bd.gameState.getBackLostFigure = 0;
			bd.gameState.nothing = 1;

			SetCurrentPlayer(cam);

			if (VictoryCondition())
				return true;
		}
	}
	else
	{
		bd.currentSelectedVal = 6;
	}
	return false;
}

bool Board::MoveFigure(glm::vec3 chooseSpot, GLuint shaderIdGUI, Camera* cam)
{
	if (bd.currentSelectedVal != 6)
	{
		Uint16 spotX = (Uint16)floor(chooseSpot.x);
		Uint16 spotY = (Uint16)floor(chooseSpot.z);

		if (spotX >= 0 && spotX <= 7 && spotY >= 0 && spotY <= 7)
		{
			//Check if the move is possible
			if (MovePossible(bd.currentSelectedBoardPos, glm::u16vec2(spotX, spotY), bd.currentSelectedVal, bd.boardState, NORMAL_CHECK))
			{
				//If the pawn reaches the back of the field
				if ((bd.boardState[bd.currentSelectedBoardPos.x][bd.currentSelectedBoardPos.y] == 0 && spotX == 7) ||
					(bd.boardState[bd.currentSelectedBoardPos.x][bd.currentSelectedBoardPos.y] == 7 && spotX == 0))
						PawnChooseFigure(spotX, spotY);

				//If the replaced field is not empty, then store the replaced figure in the lostFigures
				Uint16 boardDest = bd.boardState[spotX][spotY];
				if (boardDest != 6)
				{
					Uint16 lFi = (boardDest < 6) ? 0 : 1;
					for (Uint8 i = 0; i < 15; i++)
					{
						if (bd.lostFigures[lFi][i] == 6)
						{
							bd.lostFigures[lFi][i] = boardDest;
							break;
						}
					}
				}

				bd.boardState[bd.currentSelectedBoardPos.x][bd.currentSelectedBoardPos.y] = 6;
				bd.boardState[spotX][spotY] = bd.currentSelectedVal;

				if (!bd.gameState.getBackLostFigure)
					SetCurrentPlayer(cam);

				if (bd.currentSelectedVal == 5) bd.kingPlayerOne.pos = { spotX, spotY }; //set the king position variable
				else if (bd.currentSelectedVal == 12) bd.kingPlayerTwo.pos = { spotX, spotY };
			}


			if (VictoryCondition())
				return true;
			
		}

		bd.currentSelectedVal = 6;
	}

	return false;
}

bool Board::GetCurrentPlayer()
{
	return bd.currentPlayer;
}

void Board::SetCurrentPlayer(Camera* cam)
{
	(bd.currentPlayer == PLAYER_ONE) ? bd.currentPlayer = PLAYER_TWO : bd.currentPlayer = PLAYER_ONE; //Switches player

	if (bd.currentPlayer == PLAYER_ONE)
	{
		cam->view = &cam->viewPOne;
		cam->camFront = &cam->camFrontPOne;
		cam->camPos = &cam->camPosPOne;
		cam->yaw = &cam->yawPOne;
		cam->pitch = &cam->pitchPOne;
	}
	else
	{
		cam->view = &cam->viewPTwo;
		cam->camFront = &cam->camFrontPTwo;
		cam->camPos = &cam->camPosPTwo;
		cam->yaw = &cam->yawPTwo;
		cam->pitch = &cam->pitchPTwo;
	}
}

// Private

bool Board::MovePossible(glm::u16vec2 src, glm::u16vec2 dest, Uint16 currVal, Uint16 (&boardStateP)[8][8], bool checkKing)
{
	//Check if there is a friendly figure on that field
	if (currVal < 6 && boardStateP[dest.x][dest.y] < 6)
		return false;
	else if (currVal > 6 && boardStateP[dest.x][dest.y] > 6)
		return false;
	//Check if the king is on that field
	else if ((boardStateP[dest.x][dest.y] == 5 ||
		boardStateP[dest.x][dest.y] == 12) && !checkKing)
		return false;

	if (currVal < 6)
	{
		if (!IsCheckAvoidable(bd.gameState.checkP1, src, dest))
			return false;
	}
	else if (currVal > 6)
	{
		if (!IsCheckAvoidable(bd.gameState.checkP2, src, dest))
			return false;
	}

	if (currVal == 0)
	{
		bool retVal = false;

		//White pawn
		if (src.x - dest.x == -1 && //for the normal movement
			src.y - dest.y == 0 &&
			boardStateP[dest.x][dest.y] == 6)
			return true;
		else if (src.x - dest.x == -1 && //for the diagonal attack
			abs(src.y - dest.y) == 1 &&
			boardStateP[dest.x][dest.y] != 6)
			return true;
		else if (src.x - dest.x == -2 && //for the first move; 2 fields possible
			src.y - dest.y == 0 &&
			boardStateP[dest.x][dest.y] == 6 &&
			src.x == 1 &&
			MoveObstructed(src.x, src.y, dest.x, dest.y, boardStateP))
			return true;
	}
	else if (currVal == 7)
	{
		//Black pawn
		if (src.x - dest.x == 1 &&
			src.y - dest.y == 0 && 
			boardStateP[dest.x][dest.y] == 6)
			return true;
		else if (src.x - dest.x == 1 &&
			abs(src.y - dest.y) == 1 &&
			boardStateP[dest.x][dest.y] != 6)
			return true;
		else if (src.x - dest.x == 2 && //for the first move; 2 fields possible
			src.y - dest.y == 0 &&
			boardStateP[dest.x][dest.y] == 6 &&
			src.x == 6 &&
			MoveObstructed(src.x, src.y, dest.x, dest.y, boardStateP))
			return true;
	}
	else if (currVal == 1 || currVal == 8)
	{
		//Rook
		if (src.y - dest.y == 0 || 
			src.x - dest.x == 0)
		{
			return MoveObstructed(src.x, src.y, dest.x, dest.y, boardStateP);
		}
	}
	else if (currVal == 2 || currVal == 9)
	{
		//Knight
		if ((abs(src.x - dest.x) == 2 &&
			abs(src.y - dest.y) == 1)
			||
			(abs(src.x - dest.x) == 1 &&
			abs(src.y - dest.y) == 2))
			return true;
	}
	else if (currVal == 3 || currVal == 10)
	{
		//Bishop
		
		//the diagonals can be defined as follows:
		//the x and y value is always the same (starting from the figure position)
		if (abs(src.x - dest.x) ==
			abs(src.y - dest.y))
		{
			return MoveObstructed(src.x, src.y, dest.x, dest.y, boardStateP);
		}
	}
	else if (currVal == 4 || currVal == 11)
	{
		//Queen
		if (abs(src.x - dest.x) == 0 ||
			abs(src.y - dest.y) == 0 ||
			abs(src.x - dest.x) == abs(src.y - dest.y))
		{
			return MoveObstructed(src.x, src.y, dest.x, dest.y, boardStateP);
		}
	}
	else if (currVal == 5 || currVal == 12)
	{
		//King
		if (abs(src.x - dest.x) <= 1 &&
			abs(src.y - dest.y) <= 1 && checkKing)
		{
			return true;
		}
		else if (abs(src.x - dest.x) <= 1 &&
			abs(src.y - dest.y) <= 1)
		{
			Uint16 tempBoardState[8][8];
			for (Uint8 i = 0; i < 8; i++)
				for (Uint8 j = 0; j < 8; j++)
					tempBoardState[i][j] = boardStateP[i][j];

			tempBoardState[dest.x][dest.y] = currVal;
			tempBoardState[src.x][src.y] = 6;

			glm::u16vec2 origBoardPos = src;
			Uint16 origBoardVal = currVal;

			
			for (Uint16 i = 0; i < 8; i++)
			{
				for (Uint16 j = 0; j < 8; j++)
				{
					src = glm::u16vec2(i, j);
					currVal = tempBoardState[i][j];

					if (MovePossible(src, dest, currVal, tempBoardState, CHECK_KING))
						return false;
				}
			}

			return true;
		}
	}

	return false;
}

bool Board::MoveObstructed(short sourceX, short sourceY, short spotX, short spotY, Uint16(&boardStateP)[8][8])
{
	short distX = spotX - sourceX;
	short distY = spotY - sourceY;

	while (1)
	{
		if (distX < 0)
			distX++;
		else if (distX > 0)
			distX--;

		if (distY < 0)
			distY++;
		else if (distY > 0)
			distY--;

		if (distX == 0 && distY == 0)
			break;

		if (boardStateP[sourceX + distX][sourceY + distY] != 6)
			return false;
	}

	return true;
}

bool Board::IsCheckAvoidable(bool check, glm::u16vec2 src, glm::u16vec2 dest)
{
	Uint16 newBoard[8][8];
	for (Uint8 i = 0; i < 8; i++)
		for (Uint8 j = 0; j < 8; j++)
			newBoard[i][j] = bd.boardState[i][j];

	newBoard[dest.x][dest.y] = bd.boardState[src.x][src.y];
	newBoard[src.x][src.y] = 6;
	
	glm::u16vec2 tempKPos;
	if (bd.currentPlayer)
		tempKPos = bd.kingPlayerOne.pos;
	else
		tempKPos = bd.kingPlayerTwo.pos;

	if (tempKPos == glm::u16vec2(src.x, src.y))
		tempKPos = glm::u16vec2(dest.x, dest.y);

	return !bd.kingPlayerOne.IsCheck(newBoard, tempKPos);
}

bool Board::VictoryCondition()
{
	bd.gameState.SetToNothing();
	if (bd.gameState.getBackLostFigure)
		bd.gameState.nothing = 0;
	if (bd.kingPlayerOne.IsCheck(bd.boardState, bd.kingPlayerOne.pos))
	{
		bd.gameState.checkP1 = 1; bd.gameState.nothing = 0;
		if (bd.kingPlayerOne.IsCheckMate(bd.boardState))
		{
			bd.currentSelectedVal = 6;
			bd.gameState.checkmateP1 = 1;
			return true;
		}
	}
	if (bd.kingPlayerTwo.IsCheck(bd.boardState, bd.kingPlayerTwo.pos))
	{
		bd.gameState.checkP2 = 1; bd.gameState.nothing = 0;
		if (bd.kingPlayerTwo.IsCheckMate(bd.boardState))
		{
			bd.currentSelectedVal = 6;
			bd.gameState.checkmateP2 = 1;
			return true;
		}
	}
	return false;
}

void Board::PawnChooseFigure(Uint16 spotX, Uint16 spotY)
{
	for (Uint8 i = 0; i < 15; i++)
	{
		if (bd.lostFigures[!bd.currentPlayer][i] != 6) //checks if there are any lost figures or if all fields are empty
		{
			bd.pawnEndPosition = glm::u16vec2(spotX, spotY);
			bd.gameState.getBackLostFigure = true;
			break;
		}
	}
}
