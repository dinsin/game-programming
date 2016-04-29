/*

Dinesh Singh
CS3913 Intro to Game Programming
Prof. Ivan Safrin
Final Project (Towerfall: Ascension inspired local game)

GameApp

Since the amount of global variables was getting a little crazy,
I recreated the ClassDemoApp with this GameApp to organize better.
It handles most of the major functions, so its easier when variables from one function
have to be used by variable of another.

TO-DO LIST:

GAME
- Hit detection on bullets (vs. players and vs. walls)
	- Cap shots fired based on ammo capacity
- Change sprites to a more hi-res, cartoon-y type
- Change sprite when damaged (?)
- Animate the running and firing
- Change tiles to create a theme (modern city, lab, sewers, etc.)
- Create two more level arrangements
- Add special elements to some levels (teleporters, spikes, etc.)
- Add powerups (flipscreen, overshield, rapidfire, speed burst)
- Add pickup weapons (rockets, grenades, machine gun, etc.)
- Animate the detonation of explosive weapons 
- Add effects for getting shot
- Add spawn zones
- Add player attributes (health, armor)
- Add double jump, dash, crouch, heal, block abilities
	- Add crouce/prone shooting
- Add "Pause" option
- Add options to use a controller

MENU
- Change drawn on text into unique logo
- Render a unique background
- Add option for number of players selection, player appearance selection
- Add lighting as a decoration on menu

HUD
- Add health and armor
- Add ammo and current weapon
- Add picture of player

GAME OVER
- Add score after last round
- Add score to win
- Add field congratulating winner
- Add options to return to menu, or restart

*/

#include "GameApp.h"
using namespace std;

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

enum GameState { STATE_MAIN_MENU, STATE_GAME_LEVEL, STATE_GAME_OVER };
enum EntityType { PLAYER, PROJECTILE };

unsigned char level1Data[LEVEL_HEIGHT][LEVEL_WIDTH] = {					// Globally defining the tile data for "Level 1"
	{ 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90 },
	{ 90, 4, 4, 4, 4, 4, 4, 4, 90, 90, 90, 90, 90, 90, 4, 4, 4, 4, 4, 4, 4, 90 },
	{ 90, 4, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 4, 90 },
	{ 90, 4, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 4, 90 },
	{ 90, 4, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 4, 90 },
	{ 90, 4, 90, 90, 90, 90, 90, 6, 6, 6, 6, 6, 6, 6, 6, 90, 90, 90, 90, 90, 4, 90 },
	{ 90, 4, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 4, 90 },
	{ 90, 4, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 4, 90 },
	{ 90, 4, 6, 6, 6, 6, 6, 90, 90, 90, 90, 90, 90, 90, 90, 6, 6, 6, 6, 6, 4, 90 },
	{ 90, 4, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 4, 90 },
	{ 90, 4, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 4, 90 },
	{ 90, 4, 90, 90, 90, 90, 90, 6, 6, 6, 6, 6, 6, 6, 6, 90, 90, 90, 90, 90, 4, 90 },
	{ 90, 4, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 4, 90 },
	{ 90, 4, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 4, 90 },
	{ 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 90, 90, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2 },
	{ 32, 33, 33, 34, 32, 33, 33, 34, 33, 35, 100, 101, 35, 32, 33, 32, 34, 32, 33, 32, 33, 33 },
};

GLuint LoadTexture(const char *image_path) {			// Loading textures for blocks and player
	SDL_Surface *surface = IMG_Load(image_path);

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	SDL_FreeSurface(surface);

	return textureID;
}

void GameApp::DrawText(int fontTexture, string text, float size, float spacing, float x, float y) {
	float texture_size = 1.0 / 16.0f;
	vector<float> vertexData;
	vector<float> texCoordData;

	modelMatrix.identity();

	modelMatrix.Translate(x, y, 0.0f);
	program->setModelMatrix(modelMatrix);

	for (int i = 0; i < text.size(); i++) {
		float texture_x = (float)(((int)text[i]) % 16) / 16.0f;
		float texture_y = (float)(((int)text[i]) / 16) / 16.0f;
		vertexData.insert(vertexData.end(), {
			((size + spacing) * i) + (-0.5f * size), 0.5f * size,
			((size + spacing) * i) + (-0.5f * size), -0.5f * size,
			((size + spacing) * i) + (0.5f * size), 0.5f * size,
			((size + spacing) * i) + (0.5f * size), -0.5f * size,
			((size + spacing) * i) + (0.5f * size), 0.5f * size,
			((size + spacing) * i) + (-0.5f * size), -0.5f * size,
		});
		texCoordData.insert(texCoordData.end(), {
			texture_x, texture_y,
			texture_x, texture_y + texture_size,
			texture_x + texture_size, texture_y,
			texture_x + texture_size, texture_y + texture_size,
			texture_x + texture_size, texture_y,
			texture_x, texture_y + texture_size,
		});
	}
	glUseProgram(program->programID);

	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertexData.data());
	glEnableVertexAttribArray(program->positionAttribute);
	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData.data());
	glEnableVertexAttribArray(program->texCoordAttribute);
	glBindTexture(GL_TEXTURE_2D, fontTexture);
	glDrawArrays(GL_TRIANGLES, 0, text.size() * 6);

	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
}

bool GameApp::isDone() {
	return done;
}

GameApp::GameApp() :
lastFrameTicks(0.0f), timeLeftOver(0.0f), done(false)
{
	Setup();
}

void GameApp::Setup() {
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Dinesh's Platformer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
	glewInit();
#endif

	glViewport(0, 0, 1280, 720);

	projectionMatrix.setOrthoProjection(-11.0, 11.0, -11.0, 5.0, -1.0, 1.0);

	program = new ShaderProgram(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	blocksTexture = LoadTexture(RESOURCE_FOLDER"demo_sprites.png");
	playerTexture = LoadTexture(RESOURCE_FOLDER"space_cartoon.png");
	fontTexture = LoadTexture(RESOURCE_FOLDER"pixel_font.png");
	bulletTexture = LoadTexture(RESOURCE_FOLDER"space_cartoon.png");

	RenderMainMenu();
}

GameApp::~GameApp() {
	delete program;
	SDL_Quit();
}

bool GameApp::ProcessUpdateAndRender() {
	float ticks = (float)SDL_GetTicks() / 1000.0f;
	float elapsed = ticks - lastFrameTicks;
	lastFrameTicks = ticks;

	ProcessEvents();

	float fixedElapsed = elapsed + timeLeftOver;
	if (fixedElapsed > FIXED_TIMESTEP * MAX_TIMESTEPS) {
		fixedElapsed = FIXED_TIMESTEP * MAX_TIMESTEPS;
	}
	while (fixedElapsed >= FIXED_TIMESTEP) {
		fixedElapsed -= FIXED_TIMESTEP;
		Update(FIXED_TIMESTEP);
	}
	timeLeftOver = fixedElapsed;

	return done;
}

void GameApp::ProcessEvents() {

	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
			done = true;
		}
		else if (event.type == SDL_KEYDOWN) {
			if (event.key.keysym.scancode == SDL_SCANCODE_F && state == STATE_GAME_LEVEL){
				shootBullet(player1);
			}
			if (event.key.keysym.scancode == SDL_SCANCODE_RSHIFT && state == STATE_GAME_LEVEL){
				shootBullet(player2);
			}
			else if (event.key.keysym.scancode == SDL_SCANCODE_SPACE && state == STATE_MAIN_MENU) {
				RenderGameLevel();
			}
		}
	}

	glClearColor(0.3f, 0.3f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	program->setModelMatrix(modelMatrix);
	program->setViewMatrix(viewMatrix);
	program->setProjectionMatrix(projectionMatrix);

	glUseProgram(program->programID);

	switch (state) {
	case STATE_MAIN_MENU:
		DrawText(fontTexture, "FINAL PROJECT", 0.75, 0, -4.05, 0.0f);
		DrawText(fontTexture, "PRESS SPACE TO START!", 0.4, 0, -3.6f, -2.5f);
		break;

	case STATE_GAME_LEVEL:
		modelMatrix.identity();
		buildLevel();
		renderTilemap();
		player1->Render(program);
		player2->Render(program);
		for (Entity& projectile : projectiles){
			projectile.Render(program);
		}
		break;
	}
	SDL_GL_SwapWindow(displayWindow);
}

void GameApp::worldToTileCoordinates(float worldX, float worldY, int *gridX, int *gridY) {
	*gridX = (int)(worldX / TILE_SIZE);
	*gridY = (int)(-worldY / TILE_SIZE);
}

bool GameApp::checkIfSolid(int gridX, int gridY, unsigned char levelData[LEVEL_HEIGHT][LEVEL_WIDTH]) {
	if (gridX >= 0 && gridY >= 0 && (levelData[gridY][gridX] == 4 ||
		levelData[gridY][gridX] == 6 ||
		levelData[gridY][gridX] == 2 ||
		levelData[gridY][gridX] == 3 ||
		levelData[gridY][gridX] == 32 ||
		levelData[gridY][gridX] == 33 ||
		levelData[gridY][gridX] == 34 ||
		levelData[gridY][gridX] == 35 ||
		levelData[gridY][gridX] == 100 ||
		levelData[gridY][gridX] == 101)) {
		return true;
	}
	return false;
}

void GameApp::Update(float elapsed) {		// A switch between the games two states
	switch (state) {
	case STATE_MAIN_MENU:
		break;

	case STATE_GAME_LEVEL:
		UpdateGameLevel(elapsed);
		break;
	}
}

void GameApp::RenderMainMenu() {
	state = STATE_MAIN_MENU;
}

void GameApp::RenderGameLevel() {

	state = STATE_GAME_LEVEL;

	glClearColor(0.2f, 0.4f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	player1 = new Entity();
	player1->position_x = 4.5f;
	player1->position_y = -7.0f;
	player1->acceleration_y = -10.0f;
	player1->sprite = SheetSprite(playerTexture, 425.0f / 1024.0f, 404.0f / 512.0f, 40.0f / 1024.0f, 60.0f / 512.0f, 1.0f);
	player1->friction_x = 2.0f;
	player1->friction_y = 2.0f;
	player1->isStatic = false;
	player1->active = true;
	player1->entityType = PLAYER;
	player1->facing = 1.0f;
	players.push_back(player1);

	player2 = new Entity();
	player2->position_x = 15.5f;
	player2->position_y = -7.0f;
	player2->acceleration_y = -10.0f;
	player2->sprite = SheetSprite(playerTexture, 420.0f / 1024.0f, 0.0f / 512.0f, 40.0f / 1024.0f, 66.0f / 512.0f, 1.0f);
	player2->friction_x = 2.0f;
	player2->friction_y = 2.0f;
	player2->isStatic = false;
	player2->active = true;
	player2->entityType = PLAYER;
	player2->facing = -1.0f;
	players.push_back(player2);

	viewMatrix.Translate(-11.0, 5.0, 0);
}

void GameApp::UpdateGameLevel(float elapsed) {

	const Uint8 *keys = SDL_GetKeyboardState(NULL);

	if (keys[SDL_SCANCODE_A]) {							// Player 1 movement updates
		player1->acceleration_x = -10.0f;
		player1->facing = -1.0f;
	}
	else if (keys[SDL_SCANCODE_D]) {
		player1->acceleration_x = 10.0f;
		player1->facing = 1.0f;
	}
	else {
		player1->acceleration_x = 0.0f;
	}
	if (keys[SDL_SCANCODE_W] && player1->collidedBottom) {
		player1->velocity_y = 15.0f;
		player1->collidedBottom = false;
	}

	if (keys[SDL_SCANCODE_LEFT]) {						// Player 2 movement updates
		player2->acceleration_x = -10.0f;
		player2->facing = -1.0f;
	}
	else if (keys[SDL_SCANCODE_RIGHT]) {
		player2->acceleration_x = 10.0f;
		player2->facing = 1.0f;
	}
	else {
		player2->acceleration_x = 0.0f;
	}
	if (keys[SDL_SCANCODE_UP] && player2->collidedBottom) {
		player2->velocity_y = 15.0f;
		player2->collidedBottom = false;
	}

	for (size_t i = 0; i < projectiles.size(); i++){
		if (!projectiles[i].isStatic && projectiles[i].active) {
			projectiles[i].Update(elapsed);			
		}
	}

	for (size_t i = 0; i < players.size(); i++)				// Collision checking and updating for each player
		if (!players[i]->isStatic && players[i]->active) {

			players[i]->Update(elapsed);

			int gridX = 0, gridY = 0;

			worldToTileCoordinates(players[i]->position_x - players[i]->width / 2, players[i]->position_y, &gridX, &gridY);
			if (checkIfSolid(gridX, gridY, levelData)) {
				players[i]->position_x = (gridX + 1) * TILE_SIZE + players[i]->width / 2 + 0.001f;
				players[i]->velocity_x = 0;
				players[i]->collidedLeft = true;
			}

			worldToTileCoordinates(players[i]->position_x + players[i]->width / 2, players[i]->position_y, &gridX, &gridY);
			if (checkIfSolid(gridX, gridY, levelData)) {
				players[i]->position_x = gridX * TILE_SIZE - players[i]->width / 2 - 0.001f;
				players[i]->velocity_x = 0;
				players[i]->collidedRight = true;
			}

			worldToTileCoordinates(players[i]->position_x, players[i]->position_y + players[i]->height / 2, &gridX, &gridY);
			if (checkIfSolid(gridX, gridY, levelData)) {
				players[i]->position_y = (-gridY - 1) * TILE_SIZE - players[i]->height / 2 - 0.001f;
				players[i]->velocity_y = 0;
				players[i]->collidedTop = true;
			}

			worldToTileCoordinates(players[i]->position_x, players[i]->position_y - players[i]->height / 2, &gridX, &gridY);
			if (checkIfSolid(gridX, gridY, levelData)) {
				players[i]->position_y = -gridY * TILE_SIZE + players[i]->height / 2 + 0.001f;
				players[i]->velocity_y = 0;
				players[i]->collidedBottom = true;
			}
		}
}

void GameApp::buildLevel() {
	memcpy(levelData, level1Data, LEVEL_HEIGHT*LEVEL_WIDTH);
}

void GameApp::renderTilemap() {
	vector<float> vertexData;
	vector<float> texCoordData;

	for (int y = 0; y < LEVEL_HEIGHT; y++) {
		for (int x = 0; x < LEVEL_WIDTH; x++) {

			float u = (float)(((int)levelData[y][x]) % SPRITE_COUNT_X) / (float)SPRITE_COUNT_X;
			float v = (float)(((int)levelData[y][x]) / SPRITE_COUNT_X) / (float)SPRITE_COUNT_Y;

			float spriteWidth = 1.0f / (float)SPRITE_COUNT_X;
			float spriteHeight = 1.0f / (float)SPRITE_COUNT_Y;

			vertexData.insert(vertexData.end(), {

				TILE_SIZE * x, -TILE_SIZE * y,
				TILE_SIZE * x, (-TILE_SIZE * y) - TILE_SIZE,
				(TILE_SIZE * x) + TILE_SIZE, (-TILE_SIZE * y) - TILE_SIZE,

				TILE_SIZE * x, -TILE_SIZE * y,
				(TILE_SIZE * x) + TILE_SIZE, (-TILE_SIZE * y) - TILE_SIZE,
				(TILE_SIZE * x) + TILE_SIZE, -TILE_SIZE * y
			});

			texCoordData.insert(texCoordData.end(), {
				u, v,
				u, v + spriteHeight,
				u + spriteWidth, v + spriteHeight,

				u, v,
				u + spriteWidth, v + spriteHeight,
				u + spriteWidth, v
			});
		}
	}

	glUseProgram(program->programID);

	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertexData.data());
	glEnableVertexAttribArray(program->positionAttribute);

	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData.data());
	glEnableVertexAttribArray(program->texCoordAttribute);

	glBindTexture(GL_TEXTURE_2D, blocksTexture);
	glDrawArrays(GL_TRIANGLES, 0, 6 * LEVEL_HEIGHT * LEVEL_WIDTH);

	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
}

void GameApp::shootBullet(Entity* player) {				// Function that hires the firing of a bullet by the player
	Entity* bullet = new  Entity();
	bullet->sprite = SheetSprite(bulletTexture, 392.0f / 1024.0f, 478.0f / 512.0f, 29.0f / 1024.0f, 33.0f / 512.0f, .25f);
	bullet->position_x = player->position_x + (player->width/2 * player->facing);
	bullet->position_y = player->position_y;
	bullet->velocity_x = 25.0f * player->facing;
	bullet->velocity_y = 0.0f;
	bullet->entityType = PROJECTILE;
	bullet->isStatic = false;
	bullet->active = true;
	projectiles.push_back(*bullet);
}