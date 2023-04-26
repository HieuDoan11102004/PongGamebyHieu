#pragma once
#include <iostream>
#include <string>
#include <algorithm>
#include <SDL.h>
#include <SDL_ttf.h> // to render ttf fonts
#include <SDL_mixer.h>
#include "Sprite.h"
#include "Text.h"
using namespace std;
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

class GameEngine
{
private:
	SDL_Event event; // to handle events
	SDL_Event eventMenu; // to handle events menu
	SDL_Event eventPause; // to handle events pause
	SDL_Event eventOver; // to handle events game over

	int mouseX, mouseY; // for mouse coordinates

	SDL_Window* window; // moved from InitGameEngine
	Mix_Music* music = NULL;
	Mix_Chunk* effect = NULL;

	bool isRunning = true;
	bool isMenuRunning = true;
	bool isPause = false;
	bool isOver = false;

	SDL_Texture* menuTexture;
	SDL_Texture* pauseTexture;
	SDL_Texture* gameOverTexture;
	

	int p1score = 0; // for demo score is hardcoded, this score will be tracked and made in a different function
	int aiscore = 0; // AI paddle score

	string s1 = to_string(p1score);
	string s2 = to_string(aiscore);

	Sprite* background; // Sprite Actors
	Sprite* paddleHuman;
	Sprite* paddleAI;
	Sprite* ball;

	Text* P1score;
	Text* AIscore;

	void InitializeSpriteBackground(const char* loadPath, int cellX, int cellY, int cellWidth, int cellHeight,
		int destX, int destY, int destW, int destH);
	void InitializeSpritepaddleHuman(const char* loadPath, int cellX, int cellY, int cellWidth, int cellHeight,
		int destX, int destY, int destW, int destH);
	void InitializeSpritepaddleAI(const char* loadPath, int cellX, int cellY, int cellWidth, int cellHeight,
		int destX, int destY, int destW, int destH);
	void InitializeSpriteBall(const char* loadPath, int cellX, int cellY, int cellWidth, int cellHeight,
		int destX, int destY, int destW, int destH);

	SDL_Rect divider; // Visual Improvements: center divider
	SDL_Rect p1goal;
	SDL_Rect aigoal;

public:
	static SDL_Renderer* renderer;
	static TTF_Font* font; // font declarations

	int speed_x, speed_y; // x and y speeds of the ball
	int direction[2] = { -1, 1 }; // x and y array directions

	static GameEngine* Instance(); // function returns

	bool InitGameEngine();
	bool CheckCollision(SDL_Rect A, SDL_Rect B);
	void PaddleHumanMoveMouse(); // Sprite, to:
	void UpdateBallPosition();
	void ReverseBallPositionY();
	void ResetBallPositionX();
	void PaddleAIMove();
	void ResetPaddleAIBallNotAIArea();
	void BallInAIArea();
	void CheckBallPaddleCollision();
	void BallInPaddleHumanGoalArea();
	void BallInPaddleAIGoalArea(); // Sprite, from
	bool NotAIArea(SDL_Rect BALL, SDL_Rect AIAREA);
	bool InAIArea(SDL_Rect BALL, SDL_Rect AIAREA);
	void PlayerServe();
	void AIServe();
	void AI();
	void AddToPlayerScore();
	void AddToAIScore();
	void keepScore();
	void endGame();
	void InitGameWorld();
	void Render();
	void Input();
	void Quit();
	void Update();
	bool IsRunning() { return isRunning; }
	bool IsMenuRunning() { return isMenuRunning; }
	bool IsPause() { return isPause; }
	bool IsOver() { return isOver; }
	bool checkOver() {
		return (aiscore == 5 || p1score == 5);
	}


	void Music();
	void Effect();
	void setGameOver();
	void setMenu();
	void pauseGame();
};

