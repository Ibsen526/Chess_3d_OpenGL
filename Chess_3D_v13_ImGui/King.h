#pragma once

class King
{
#define PLAYER_ONE true
#define PLAYER_TWO false
#define CHECK_KING true
#define NORMAL_CHECK false
#define CHECKMATE true


public:
	glm::u16vec2 pos;
	//bool player;

	King()
	{
		pos = glm::u16vec2(0, 0);
	}

	King(Uint16 x, Uint16 y)
	{
		pos = glm::u16vec2(x, y);
	}

	bool IsCheck(Uint16(&boardState)[8][8], glm::u16vec2 kingPos)
	{
		for (Uint16 i = 0; i < 8; i++) // check if the king is in check
		{
			for (Uint16 j = 0; j < 8; j++)
			{
				if (boardState[i][j] != 5 && boardState[i][j] != 12) //the king is in check
				{
					if (MovePossible(glm::u16vec2(i, j), kingPos,
						boardState, NORMAL_CHECK, NORMAL_CHECK))
					{
						return true;
					}
				}
			}
		}

		return false;
	}

	bool IsCheckMate(Uint16(&boardState)[8][8]) //here sth is wrong because in one certain position the checkCounter = 6 when it should be 7
	{
		glm::i16vec2 kingMoves[8] = { {-1,1},{0,1},{1,1}, {-1,0},{1,0}, {-1,-1},{0,-1},{1,-1} };
		Uint16 checkCounter = 0;

		for (Uint16 k = 0; k < 8; k++)
		{
			for (Uint16 i = 0; i < 8; i++)
			{
				for (Uint16 j = 0; j < 8; j++)
				{
					if (pos.x + kingMoves[k].x < 0 || pos.x + kingMoves[k].x > 7 ||
						pos.y + kingMoves[k].y < 0 || pos.y + kingMoves[k].y > 7)
					{
						checkCounter++;
						i = 8; j = 8;
					}
					else if (!(pos.x == i && pos.y == j) && 
						MovePossible(glm::u16vec2(i, j), glm::u16vec2(pos.x + kingMoves[k].x, pos.y + kingMoves[k].y),
							boardState, CHECKMATE, NORMAL_CHECK))
					{
						checkCounter++;
						i = 8; j = 8;
					}
				}
			}
		}

		std::cout << checkCounter << std::endl;
		if (checkCounter >= 7)
		{
			std::cout << "Pos " << pos.x << " " << pos.y << std::endl;

			//check if another move can set you free
			std::cout << "Saveable: " << SavedByOtherMove(boardState) << std::endl;
			if (!SavedByOtherMove(boardState))
				return true;
		}

		return false;
	}

private:
	bool SavedByOtherMove(Uint16(&boardState)[8][8])
	{
		//if the king is in check, then only moves are allowed, for which the king check afterwards is false
		Uint16 tempBoardState[8][8];
		for (Uint8 i = 0; i < 8; i++)
			for (Uint8 j = 0; j < 8; j++)
				tempBoardState[i][j] = boardState[i][j];

		for (Uint16 sx = 0; sx < 8; sx++) //source
		{
			for (Uint16 sy = 0; sy < 8; sy++)
			{
				for (Uint16 dx = 0; dx < 8; dx++) //dest
				{
					for (Uint16 dy = 0; dy < 8; dy++)
					{
						if (!(sx == dx && sy == dy) &&
							MovePossible(glm::u16vec2(sx, sy), glm::u16vec2(dx, dy), boardState, NORMAL_CHECK, CHECK_KING))
						{
							Uint16 originalState = tempBoardState[sx][sy];
							Uint16 overwrittenState = tempBoardState[dx][dy];
							tempBoardState[dx][dy] = originalState;
							tempBoardState[sx][sy] = 6;

							glm::u16vec2 tempPos = pos;
							if (sx == pos.x && sy == pos.y) //change the king position if the start is equal to the king
								tempPos = glm::u16vec2(dx, dy);

							if (!IsCheck(tempBoardState, tempPos)) //pos is a class variable, maybe it should also be a parameter
							{
								return true;
							}

							tempBoardState[sx][sy] = originalState;
							tempBoardState[dx][dy] = overwrittenState;
							if (sx == pos.x && sy == pos.y) 
								tempPos = glm::u16vec2(sx, sy);
							
						}
					}
				}
			}
		}

		return false;
	}

	bool MovePossible(glm::u16vec2 src, glm::u16vec2 dest, Uint16(&boardStateP)[8][8], bool checkMate, bool checkKing)
	{
		Uint16 currVal = boardStateP[src.x][src.y];

		if (checkMate) //Checks the surrounding fields, so dest is always 6
		{
			if (currVal < 6 && boardStateP[pos.x][pos.y] < 6)
				return false;
			else if (currVal > 6 && boardStateP[pos.x][pos.y] > 6)
				return false;
		}
		else //Check if there is a friendly figure on that field
		{
			if (currVal < 6 && boardStateP[dest.x][dest.y] < 6)
				return false;
			else if (currVal > 6 && boardStateP[dest.x][dest.y] > 6)
				return false;
		}

		if (checkKing) //Checks if a king is on the dest field
		{
			if (boardStateP[dest.x][dest.y] == 5 ||
				boardStateP[dest.x][dest.y] == 12)
				return false;
			else if (currVal > 6 && boardStateP[pos.x][pos.y] < 6)
				return false;
			else if (currVal < 6 && boardStateP[pos.x][pos.y] > 6)
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
				abs(src.y - dest.y) <= 1)
			{
				return true;
			}
		}

		return false;
	}

	bool MoveObstructed(short sourceX, short sourceY, short spotX, short spotY, Uint16(&boardStateP)[8][8])
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
};
