/*

Dinesh Singh
CS3913 Intro to Game Programming
Prof. Ivan Safrin
Homework #4 (Platformer)

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

#define TILE_SIZE 1.0f

#define SPRITE_COUNT_X 16
#define SPRITE_COUNT_Y 8

class GameApp {
public:

	GameApp();
	~GameApp();

	void DrawText(int fontTexture, string text, float size, float spacing, float x, float y);

	void Setup();

	bool ProcessUpdateAndRender();
	void ProcessEvents();
	void Render();
	void Update(float elapsed);

	void worldToTileCoordinates(float worldX, float worldY, int *gridX, int *gridY);
	bool checkIfSolid(int gridX, int gridY, unsigned char levelData[LEVEL_HEIGHT][LEVEL_WIDTH]);
	void buildLevel();
	void renderTilemap();

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

	Entity *player;
	vector<Entity*> entities;

	GLuint blocksTexture;
	GLuint playerTexture;

	float timeCount;

	unsigned char levelData[LEVEL_HEIGHT][LEVEL_WIDTH];

	bool done = false;

};