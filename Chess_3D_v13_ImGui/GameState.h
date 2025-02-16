#pragma once

class GameState
{
public:
	GameState()
	{
		SetToNothing();
		getBackLostFigure = 0;
	}

	void SetAllToFalse()
	{
		pause = 0; checkP1 = 0; checkP2 = 0; checkmateP1 = 0; checkmateP2 = 0; getBackLostFigure = 0; nothing = 0;
	}

	void SetToNothing()
	{
		pause = 0; checkP1 = 0; checkP2 = 0; checkmateP1 = 0; checkmateP2 = 0; nothing = 1;
	}

	bool pause;
	bool checkP1;
	bool checkP2;
	bool checkmateP1;
	bool checkmateP2;
	bool getBackLostFigure;
	bool nothing;
};
