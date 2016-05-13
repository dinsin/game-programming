/*

Dinesh Singh
CS3913 Intro to Game Programming
Prof. Ivan Safrin
Final Project

GameApp

Since the amount of global variables was getting a little crazy,
I recreated the ClassDemoApp with this GameApp to organize better.
It handles most of the major functions, so its easier when variables from one function
have to be used by variable of another.

*/

#pragma once

#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

#include "ShaderProgram.h"
#include "Matrix.h"
#include "Entity.h"
#include "SheetSprite.h"

#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

#define FIXED_TIMESTEP 0.0166666f
#define MAX_TIMESTEPS 6

#define LEVEL_HEIGHT 16
#define LEVEL_WIDTH 22

#define FOREST_TILE_SIZE 1.0f
#define SEWERS_TILE_SIZE 1.0f
#define CAVE_TILE_SIZE 1.0f

#define FOREST_SPRITE_COUNT_X 16
#define FOREST_SPRITE_COUNT_Y 8

#define SEWERS_SPRITE_COUNT_X 16
#define SEWERS_SPRITE_COUNT_Y 22

#define CAVE_SPRITE_COUNT_X 14
#define CAVE_SPRITE_COUNT_Y 8

class GameApp {
public:

	GameApp();
	~GameApp();

	void DrawText(int fontTexture, string text, float size, float spacing, float x, float y);

	void DrawHUD(int fontTexture, string text, float size, float spacing, float x, float y);

	void Setup();

	bool ProcessUpdateAndRender();
	void ProcessEvents();

	void Update(float elapsed);

	void RenderMainMenu();
	void RenderLevelSelect();
	void RenderRoundOver();

	void UpdateGameLevel(float elapsed);
	void RenderGameLevel();

	void worldToTileCoordinates(float worldX, float worldY, int *gridX, int *gridY);
	bool checkIfSolid(int gridX, int gridY, unsigned char levelData[LEVEL_HEIGHT][LEVEL_WIDTH]);
	void buildLevel();
	void renderForestTilemap();
	void renderSewersTilemap();
	void renderCaveTilemap();

	void shootBullet(Entity* player);
	float lerp(float from, float to, float time);

	bool isDone();

private:

	ShaderProgram *program;
	SDL_Event event;
	SDL_Window* displayWindow;

	Matrix projectionMatrix;
	Matrix modelMatrix;
	Matrix viewMatrix;

	float lastFrameTicks;
	float timeLeftOver;

	Entity* player1;
	int p1kills = 0;
	Entity* player2;
	int p2kills = 0;
	vector<Entity*> players;
	vector<Entity> projectiles;
	Entity* winner;

	GLuint forestTexture;
	GLuint sewersTexture;
	GLuint caveTexture;
	GLuint playerTexture;
	GLuint fontTexture;
	GLuint bulletTexture;

	Mix_Music* menuBGM;
	Mix_Music* forestBGM;
	Mix_Music* sewersBGM;
	Mix_Music* caveBGM;
	Mix_Chunk* shootSFX;
	Mix_Chunk* jumpSFX;
	Mix_Chunk* gameoverSFX;

	float timeCount;

	int state;
	int level;
	int musicState;

	unsigned char levelData[LEVEL_HEIGHT][LEVEL_WIDTH];

	bool done = false;

};