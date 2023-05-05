#include "GameEngine.h"
#include "Text.h"

static GameEngine* instance; // Singleton pattern for GameEngine class, static instance

Text* P1score;
Text* AIscore;

SDL_Renderer* GameEngine::renderer = nullptr;

GameEngine* GameEngine::Instance() {
	if (instance == nullptr)
		instance = new GameEngine();
	return instance;
}

bool GameEngine::InitGameEngine() {
	window = SDL_CreateWindow("Pong Game", SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0
	);

	if (!window) {
		cout << "Window creation failed... \n";
		return false;
	}

	renderer = SDL_CreateRenderer(window, -1, 0);

	if (!renderer) {
		cout << "Render initialization failed. \n";
		return false;
	}

	if (TTF_Init() < 0) { // check if TTF can be used and if it can't, then show a message
		cout << "TTF Font engine init failed.\n";
		return false;
	}
	font = TTF_OpenFont("Assets/Fonts/LTYPE.TTF", 30); // initial size of 30, LTYPE.TTF file placed in created Assets/Fonts folders

	resetPoint();

	P1score = new Text(s1.c_str(), 190, 30, true, renderer, 1);
	AIscore = new Text(s2.c_str(), 595, 30, true, renderer, 1);

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		cout << "Mixer could not be initialize!";
		return false;
	}

	return true;
}

bool GameEngine::CheckCollision(SDL_Rect A, SDL_Rect B) {
	return SDL_HasIntersection(&A, &B); // Check rect A and B
}

void GameEngine::PaddleHumanMoveMouse() { // Map paddleHuman's y position to the mouse's Y pos
	paddleHuman->spriteDestRect.y = mouseY - 72.5;
}

void GameEngine::UpdateBallPosition() { // Change destRect's x and y values to the x and y variables from the function's argument.
	ball->spriteDestRect.x += speed_x;
	ball->spriteDestRect.y += speed_y;
}

void GameEngine::ReverseBallPositionY() { // check ball's y position is above the top wall (y less than zero)
	if (ball->spriteDestRect.y < 0 || ball->spriteDestRect.y >(WINDOW_HEIGHT - ball->spriteDestRect.h)) { // or if y position is lower than the bottom wall (y greater than WINDOW_HEIGHT)
		speed_y = -speed_y;
	}
}

void GameEngine::ResetBallPositionX() { // If ball goes out on sides, left and right, reset to centre of screen
	if (ball->spriteDestRect.x < 0 || ball->spriteDestRect.x > WINDOW_WIDTH) {
		ball->spriteDestRect.x = WINDOW_WIDTH / 2;
		ball->spriteDestRect.y = WINDOW_HEIGHT / 2;
		speed_x = (rand() % 2 + 1) * direction[rand() % 2] * 2.0; // produces random numbers -1, -2, 1 and 2
		speed_y = (rand() % 2 + 1) * direction[rand() % 2] * 2.0;
	}
}

void GameEngine::PaddleAIMove() { // AI paddle moves with the ball (in the y-direction)
	paddleAI->spriteDestRect.y = ball->spriteDestRect.y - paddleAI->spriteDestRect.h / 2 + ball->spriteDestRect.h / 2;
}

void GameEngine::ResetPaddleAIBallNotAIArea() { // Reset paddleAI and stop it when ball is away from right side of divider
	if (NotAIArea(ball->spriteDestRect, divider)) {
		paddleAI->spriteDestRect.y = WINDOW_HEIGHT / 2 - paddleAI->spriteDestRect.h / 2 - 10;
	}
}

void GameEngine::BallInAIArea() { // If ball is on the right side of the divider, paddleAI moves with improved AI
	if (InAIArea(ball->spriteDestRect, divider)) {
		AI(); // Improve AI Paddle: making it challenging for player to win
	}
}

void GameEngine::CheckBallPaddleCollision() { // continually check if ball and player paddle or the ball and AI paddle collide
	if (CheckCollision(ball->spriteDestRect, paddleAI->spriteDestRect) || CheckCollision(ball->spriteDestRect, paddleHuman->spriteDestRect)) {
		Effect();
		speed_x = -speed_x; // if collision is detected in either, speed is reversed in the x-direction only
	}
}

void GameEngine::BallInPaddleHumanGoalArea() { 
	if (ball->spriteDestRect.x <= p1goal.x) {
		AddToAIScore();
		PlayerServe(); 
	}
}

void GameEngine::BallInPaddleAIGoalArea() { 
	if (ball->spriteDestRect.x >= aigoal.x) {
		AddToPlayerScore();
		AIServe(); 
	}
}

bool GameEngine::NotAIArea(SDL_Rect BALL, SDL_Rect AIAREA) {
	int leftBALL, leftAIAREA; // sides of rectangles
	int rightBALL, rightAIAREA;
	int topBALL, topAIAREA;
	int bottomBALL, bottomAIAREA;

	leftBALL = BALL.x; // calculate sides of Ball
	rightBALL = BALL.x + BALL.w;
	topBALL = BALL.y;
	bottomBALL = BALL.y + BALL.h;

	leftAIAREA = AIAREA.x; // calculate sides of AIArea
	rightAIAREA = AIAREA.x + AIAREA.w;
	topAIAREA = AIAREA.y;
	bottomAIAREA = AIAREA.y + AIAREA.h;

	if (bottomBALL <= topAIAREA) { // if any of the sides from Ball are outside of AIArea
		return false;
	}
	if (topBALL >= bottomAIAREA) {
		return false;
	}
	if (leftBALL >= rightAIAREA) {
		return false;
	}

	return true; // if none of the sides from Ball are outside AIArea
}

bool GameEngine::InAIArea(SDL_Rect BALL, SDL_Rect AIAREA) {
	int leftBALL, leftAIAREA; // sides of rectangles
	int rightBALL, rightAIAREA;
	int topBALL, topAIAREA;
	int bottomBALL, bottomAIAREA;

	leftBALL = BALL.x; // calculate sides of Ball
	rightBALL = BALL.x + BALL.w;
	topBALL = BALL.y;
	bottomBALL = BALL.y + BALL.h;

	leftAIAREA = AIAREA.x; // calculate sides of AIArea
	rightAIAREA = AIAREA.x + AIAREA.w;
	topAIAREA = AIAREA.y;
	bottomAIAREA = AIAREA.y + AIAREA.h;

	if (bottomBALL <= topAIAREA) { // if any of the sides from Ball are outside of AIArea
		return false;
	}
	if (topBALL >= bottomAIAREA) {
		return false;
	}
	if (rightBALL <= leftAIAREA) {
		return false;
	}

	return true; // if none of the sides from Ball are outside AIArea
}

void GameEngine::PlayerServe() {
	ball->spriteDestRect.x = WINDOW_WIDTH / 2;
	ball->spriteDestRect.y = WINDOW_HEIGHT / 2;
	speed_x = (rand() % 2 + 1) * direction[rand() % 1] * 2.0; // serve to playerPaddle
	speed_y = (rand() % 2 + 1) * direction[rand() % 2] * 2.0; // produces random numbers -1, -2, 1 and 2
}

void GameEngine::AIServe() {
	ball->spriteDestRect.x = WINDOW_WIDTH / 2;
	ball->spriteDestRect.y = WINDOW_HEIGHT / 2;
	speed_x = (rand() % 2 + 1) * (1) * 2.0; // serve to aiPaddle
	speed_y = (rand() % 2 + 1) * direction[rand() % 2] * 2.0; // produces random numbers -1, -2, 1 and 2
}

void GameEngine::AI() { // Improve AI Paddle: making it challenging for player to win
	if (ball->spriteDestRect.y < WINDOW_HEIGHT / 2 + 2) { // top: aiPaddle moves upward in direction + 2
		paddleAI->spriteDestRect.y = ball->spriteDestRect.y / 0.9209; // aiPaddle's upward movement is divided/slowed by 0.921, 0.9208 
	}
	if (ball->spriteDestRect.y < WINDOW_HEIGHT / 2 + 1) { // middle-top: aiPaddle moves upward in direction + 1
		paddleAI->spriteDestRect.y = ball->spriteDestRect.y / 0.8580; // aiPaddle's upward movement is divided/slowed by 0.780, 0.8070 
	}
	if (ball->spriteDestRect.y >= WINDOW_HEIGHT / 2 - 1) { // middle-bottom: aiPaddle moves downward in direction - 1
		paddleAI->spriteDestRect.y = ball->spriteDestRect.y * 0.8105; // aiPaddle's downward movement is multiplied/slowed by 0.660, 0.66729 
	}
	if (ball->spriteDestRect.y >= WINDOW_HEIGHT / 2 - 2) { // bottom: aiPaddle moves downward in direction - 2
		paddleAI->spriteDestRect.y = ball->spriteDestRect.y * 0.7610; // aiPaddle's downward movement is multiplied/slowed by 0.660, 0.8208
	}
}

void GameEngine::AddToPlayerScore() { // Scoring System: Player's Score
	p1score++;
	if (p1score > 5) p1score = 5; 
	s1 = to_string(p1score);
	P1score = new Text(s1.c_str(), 190, 30, true, renderer, 1);
}

void GameEngine::AddToAIScore() { // Scoring System: AI's Score
	aiscore++;
	if (aiscore > 5) aiscore = 5; 
	s2 = to_string(aiscore);
	AIscore = new Text(s2.c_str(), 595, 30, true, renderer, 1);
}

void GameEngine::keepScore() { // Game-End Condition: if AI scores 5 points
	if (aiscore == 5 || p1score == 5) {
		isRunning = false;
		isOver = true;
		endGame();
	}
}

void GameEngine::endGame() { // Game-End Condition
	ball->spriteDestRect.x = -20;
	ball->spriteDestRect.y = -20;
	paddleAI->spriteDestRect.y = WINDOW_HEIGHT / 2 - paddleAI->spriteDestRect.h / 2 - 10;
	paddleHuman->spriteDestRect.y = WINDOW_HEIGHT / 2 - paddleHuman->spriteDestRect.h / 2 - 10;
}


void GameEngine::InitGameWorld() {
	speed_x = -3.5; // speed variables
	speed_y = -3.5;
	int destW = 800; // Sprites:
	int destH = 600;
	int destW2 = 128;
	int destH2 = 128;
	int destW3 = 128;
	int destH3 = 128;
	int destW4 = 32;
	int destH4 = 32;

	float destX = WINDOW_WIDTH * 0.5f - destW * 0.5f;
	float destY = WINDOW_HEIGHT * 0.5f - destH * 0.5f;
	float destX2 = WINDOW_WIDTH * 0.1f - destW2 * 0.1f - 152.0;
	float destY2 = WINDOW_HEIGHT * 0.4816f - destH2 * 0.4816f;
	float destX3 = WINDOW_WIDTH * 1.0f - destW3 * 1.0 + 84.0f;
	float destY3 = WINDOW_HEIGHT * 0.4816f - destH3 * 0.4816f;
	float destX4 = WINDOW_WIDTH * 0.5030f - destW4 * 0.5030f;
	float destY4 = WINDOW_HEIGHT * 0.4925f - destH4 * 0.4925f;

	InitializeSpriteBackground("Assets/Sprites/stadium.png", 0, 0, 800, 600, destX, destY, destW, destH);
	InitializeSpritepaddleHuman("Assets/Sprites/image.png", 0, 0, 256, 256, destX2, destY2, destW2, destH2);
	InitializeSpritepaddleAI("Assets/Sprites/image.png", 256, 0, 256, 256, destX3, destY3, destW3, destH3);
	InitializeSpriteBall("Assets/Sprites/ball.png", 0, 0, 640, 640, destX4, destY4, destW4, destH4);

	divider = { WINDOW_WIDTH / 2,0,1,WINDOW_HEIGHT }; // Center divider
	p1goal = { 0,0,1,WINDOW_HEIGHT }; //goal
	aigoal = { WINDOW_WIDTH - 1,0,1,WINDOW_HEIGHT }; //goal
}

void GameEngine::Render() {
	SDL_RenderClear(renderer); // Clears the previously drawn frame, Draw current frame:

	background->Render(renderer); // background Sprite
	paddleHuman->Render(renderer); // paddleHmuan Sprite
	paddleAI->Render(renderer); // paddleAI Sprite
	ball->Render(renderer); // ball Sprite

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // center divider
	SDL_RenderFillRect(renderer, &divider);

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // player's goal area
	SDL_RenderFillRect(renderer, &p1goal);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // ai's goal area
	SDL_RenderFillRect(renderer, &aigoal);

	AIscore->RenderFont(); // render ai's score 
	P1score->RenderFont(); // render player's score 

	SDL_RenderPresent(renderer); 
}

void GameEngine::Input() {
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_MOUSEMOTION) {
			SDL_GetMouseState(&mouseX, &mouseY); 
		}
		if (event.type == SDL_QUIT) { 
			isRunning = false;
		}
		if (event.type == SDL_KEYDOWN) {
			switch (event.key.keysym.sym) {
			case SDLK_ESCAPE:
				isRunning = false;
				break;
			case SDLK_SPACE:
				isPause = true;
				break;
			}
		}
	}
}

void GameEngine::Quit() {
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	TTF_CloseFont(font);
	Mix_FreeMusic(music);
	Mix_Quit();
	TTF_Quit();
	SDL_Quit(); 
}

void GameEngine::Update() {
	UpdateBallPosition(); // Continually update ball's x and y position by the speed amount
	PaddleHumanMoveMouse(); // Map paddleHuman's y position to the mouse's Y pos

	BallInPaddleHumanGoalArea(); // Add score for AI if ball collides with paddleHuman's goal area behind paddleHuman
	BallInPaddleAIGoalArea(); // Add score for Player if ball collides with paddleAI's goal area behind paddleAI
	
	PaddleAIMove(); // Move paddleAI with the ball (in the y-direction)
	BallInAIArea(); // If ball is on the right side of the divider, paddleAI moves with improved AI
	
	CheckBallPaddleCollision(); // Continually check if ball and player paddle or the ball and AI paddle collide


	keepScore(); // Game end condition

	ResetBallPositionX(); // If ball goes out on sides, left and right, reset to centre of screen
	ReverseBallPositionY(); // Reverse the ball's y direction
}


void GameEngine::InitializeSpriteBackground(const char* loadPath, int cellX, int cellY, int cellWidth, int cellHeight,
	int destX, int destY, int destW, int destH) {
	SDL_Rect src;
	src = { cellX, cellY, cellWidth, cellHeight };
	SDL_Rect dest;
	dest = { destX, destY, destW, destH };
	background = new Object(loadPath, src, dest, renderer);
}

void GameEngine::InitializeSpritepaddleHuman(const char* loadPath2, int cellX2, int cellY2, int cellWidth2, int cellHeight2,
	int destX2, int destY2, int destW2, int destH2) {
	SDL_Rect src2;
	src2 = { cellX2, cellY2, cellWidth2, cellHeight2 };
	SDL_Rect dest2;
	dest2 = { destX2, destY2, destW2, destH2 };
	paddleHuman = new Object(loadPath2, src2, dest2, renderer);
}

void GameEngine::InitializeSpritepaddleAI(const char* loadPath3, int cellX3, int cellY3, int cellWidth3, int cellHeight3,
	int destX3, int destY3, int destW3, int destH3) {
	SDL_Rect src3;
	src3 = { cellX3, cellY3, cellWidth3, cellHeight3 };
	SDL_Rect dest3;
	dest3 = { destX3, destY3, destW3, destH3 };
	paddleAI = new Object(loadPath3, src3, dest3, renderer);
}

void GameEngine::InitializeSpriteBall(const char* loadPath4, int cellX4, int cellY4, int cellWidth4, int cellHeight4,
	int destX4, int destY4, int destW4, int destH4) {
	SDL_Rect src4;
	src4 = { cellX4, cellY4, cellWidth4, cellHeight4 };
	SDL_Rect dest4;
	dest4 = { destX4, destY4, destW4, destH4 };
	ball = new Object(loadPath4, src4, dest4, renderer);
}

void GameEngine::Music()
{
	music = Mix_LoadMUS("c1song.wav");
	if (isMenuRunning)
		Mix_PlayMusic(music, -1);
}

void GameEngine::Effect()
{
	effect = Mix_LoadWAV("medium.wav");
	Mix_PlayChannel(-1, effect, 0);
}


bool MouseInRect(SDL_Rect& a) {
	int x, y;
	SDL_GetMouseState(&x, &y);
	if ((x >= a.x and x <= a.x + a.w) and (y >= a.y and y <= a.y + a.h)) return true;
	return false;
}

void GameEngine::setMenu()
{
	static int a = 1, b = 1; 

	SDL_Surface* menuSurface = IMG_Load("Assets/Sprites/menu.png");

	menuTexture = SDL_CreateTextureFromSurface(renderer, menuSurface);

	SDL_FreeSurface(menuSurface);

	SDL_RenderClear(renderer);

	SDL_RenderCopy(renderer, menuTexture, NULL, NULL);

	const int menuItem = 2;
	Text* textMenu[menuItem];

	textMenu[0] = new Text("START", 350, 355, true, renderer, a); 
	textMenu[1] = new Text("EXIT", 350, 400, true, renderer, b); 

	textMenu[0]->RenderFont();
	textMenu[1]->RenderFont();

	SDL_RenderPresent(renderer);

	int x, y;

	SDL_Color white = { 255, 255, 255, 255 };
	SDL_Color red = { 255, 0, 0 };
	SDL_Color tmpColor = { 255, 255, 255, 255 };

	SDL_Surface* surface[2];

	SDL_Rect fontRect[2];

	const char* text[menuItem] = { "START", "EXIT" };


	surface[0] = TTF_RenderText_Blended(font, text[0], white);

	fontRect[0] = { 350, 355, surface[0]->w, surface[0]->h };

	surface[1] = TTF_RenderText_Blended(font, text[1], white);

	fontRect[1] = { 350, 400, surface[1]->w, surface[1]->h };

	SDL_Rect total = { fontRect[0].x, fontRect[0].y, fontRect[0].w, fontRect[0].h * menuItem }; 

	while (SDL_PollEvent(&eventMenu))
	{
		if (eventMenu.type == SDL_QUIT)
		{
			isMenuRunning = false;
			isRunning = false;
			break;
		}

		if (eventMenu.type == SDL_MOUSEMOTION)
		{
			x = eventMenu.motion.x;
			y = eventMenu.motion.y;

			for (int i = 0; i < menuItem; i++)
			{
				if (MouseInRect(fontRect[i]))
				{
					a = i;
					b = 1 - i;
				}
				if (!MouseInRect(total)) {
					a = 1; b = 1;
				}
			}
		}


		if (eventMenu.type == SDL_MOUSEBUTTONDOWN)
		{
			x = eventMenu.button.x;
			y = eventMenu.button.y;

			if (x >= fontRect[0].x and x <= fontRect[0].x + fontRect[0].w and y >= fontRect[0].y and y <= fontRect[0].y + fontRect[0].h)
			{
				isMenuRunning = false;
			}

			if (x >= fontRect[1].x and x <= fontRect[1].x + fontRect[1].w and y >= fontRect[1].y and y <= fontRect[1].y + fontRect[1].h)
			{
				isMenuRunning = false;
				isRunning = false;
			}
		}
	}
	SDL_DestroyTexture(menuTexture);
	for (auto i : textMenu) i->clear();
}

void GameEngine::pauseGame()
{
	static int a = 1, b = 1;

	SDL_Surface* pauseSurface = IMG_Load("Assets/Sprites/pause.png");

	pauseTexture = SDL_CreateTextureFromSurface(renderer, pauseSurface);

	SDL_FreeSurface(pauseSurface);

	SDL_RenderClear(renderer);

	SDL_RenderCopy(renderer, pauseTexture, NULL, NULL);

	const int pauseItem = 2;
	Text* textPause[pauseItem];

	textPause[0] = new Text("CONTINUE", 350, 355, true, renderer, a);
	textPause[1] = new Text("EXIT", 350, 400, true, renderer, b);

	textPause[0]->RenderFont();
	textPause[1]->RenderFont();


	SDL_RenderPresent(renderer);

	int x, y;

	SDL_Color white = { 255, 255, 255, 255 };
	SDL_Color red = { 255, 0, 0 };

	SDL_Surface* surface[2];

	SDL_Rect fontRect[2];

	const char* text[pauseItem] = { "CONTINUE", "EXIT" };


	surface[0] = TTF_RenderText_Blended(font, text[0], white);

	fontRect[0] = { 350, 355, surface[0]->w, surface[0]->h };

	surface[1] = TTF_RenderText_Blended(font, text[1], white);

	fontRect[1] = { 350, 400, surface[1]->w, surface[1]->h };

	SDL_Rect total = { fontRect[0].x, fontRect[0].y, fontRect[0].w, fontRect[0].h * pauseItem };

	while (SDL_PollEvent(&eventPause))
	{
		if (eventPause.type == SDL_QUIT)
		{
			isPause = false;
			isRunning = false;
			break;
		}

		if (eventPause.type == SDL_MOUSEMOTION)
		{
			x = eventPause.motion.x;
			y = eventPause.motion.y;

			for (int i = 0; i < pauseItem; i++)
			{
				if (MouseInRect(fontRect[i]))
				{
					a = i;
					b = 1 - i;
				}
				if (!MouseInRect(total))
				{
					a = 1;
					b = 1;
				}
			}
		}


		if (eventPause.type == SDL_MOUSEBUTTONDOWN)
		{
			x = eventPause.button.x;
			y = eventPause.button.y;

			if (x >= fontRect[0].x and x <= fontRect[0].x + fontRect[0].w and y >= fontRect[0].y and y <= fontRect[0].y + fontRect[0].h)
			{
				isPause = false;
			}

			if (x >= fontRect[1].x and x <= fontRect[1].x + fontRect[1].w and y >= fontRect[1].y and y <= fontRect[1].y + fontRect[1].h)
			{
				isPause = false;
				isRunning = false;
			}
		}
	}
	SDL_DestroyTexture(pauseTexture);
	for (auto i : textPause) i->clear();
}

void GameEngine::setGameOver()
{
	static int a = 1, b = 1;

	SDL_Surface* overSurface = IMG_Load("Assets/Sprites/playAgain.png");

	gameOverTexture = SDL_CreateTextureFromSurface(renderer, overSurface);

	SDL_FreeSurface(overSurface);

	SDL_RenderClear(renderer);

	SDL_RenderCopy(renderer, gameOverTexture, NULL, NULL);

	const int overItem = 2;
	Text* textOver[overItem];

	textOver[0] = new Text("PLAY AGAIN", 320, 355, true, renderer, a);
	textOver[1] = new Text("EXIT", 320, 400, true, renderer, b);

	textOver[0]->RenderFont();
	textOver[1]->RenderFont();

	Text* textWin = new Text("YOU WIN", 320, 100, true, renderer, 1);
	Text* textLose = new Text("YOU LOSE", 320, 100, true, renderer, 1);

	if (aiscore == 5) textLose->RenderFont();
	if (p1score == 5) textWin->RenderFont();

	SDL_RenderPresent(renderer);

	int x, y;

	SDL_Color white = { 255, 255, 255, 255 };
	SDL_Color red = { 255, 0, 0 };

	SDL_Surface* surface[2];

	SDL_Rect fontRect[2];

	const char* text[overItem] = { "PLAY AGAIN", "EXIT" };


	surface[0] = TTF_RenderText_Blended(font, text[0], white);

	fontRect[0] = { 320, 355, surface[0]->w, surface[0]->h };

	surface[1] = TTF_RenderText_Blended(font, text[1], white);

	fontRect[1] = { 320, 400, surface[1]->w, surface[1]->h };

	SDL_Rect total = { fontRect[0].x, fontRect[0].y, fontRect[0].w, fontRect[0].h * overItem };

	while (SDL_PollEvent(&eventOver))
	{
		if (eventOver.type == SDL_QUIT)
		{
			isOver = false;
			isRunning = false;
			break;
		}

		if (eventOver.type == SDL_MOUSEMOTION)
		{
			x = eventOver.motion.x;
			y = eventOver.motion.y;

			for (int i = 0; i < overItem; i++)
			{
				if (MouseInRect(fontRect[i]))
				{
					a = i;
					b = 1 - i;
				}
				if (!MouseInRect(total))
				{
					a = 1; b = 1;
				}
			}
		}


		if (eventOver.type == SDL_MOUSEBUTTONDOWN)
		{
			x = eventOver.button.x;
			y = eventOver.button.y;

			if (x >= fontRect[0].x and x <= fontRect[0].x + fontRect[0].w and y >= fontRect[0].y and y <= fontRect[0].y + fontRect[0].h)
			{
				isRunning = true;
				isOver = false;

				resetPoint();

				P1score = new Text("0", 190, 30, true, renderer, 1);
				AIscore = new Text("0", 595, 30, true, renderer, 1);
			}

			if (x >= fontRect[1].x and x <= fontRect[1].x + fontRect[1].w and y >= fontRect[1].y and y <= fontRect[1].y + fontRect[1].h)
			{
				isOver = false;
				isRunning = false;
			}
		}
	}

	SDL_DestroyTexture(gameOverTexture);
	for (auto i : textOver) i->clear();
}
