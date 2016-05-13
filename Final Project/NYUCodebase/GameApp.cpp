/*

Dinesh Singh
CS3913 Intro to Game Programming
Prof. Ivan Safrin
Final Project (local multiplayer game)

GameApp

Since the amount of global variables was getting a little crazy,
I recreated the ClassDemoApp with this GameApp to organize better.
It handles most of the major functions, so its easier when variables from one function
have to be used by variable of another.

TO-DO LIST:

GAME
- Hit detection on bullets (vs. players and vs. walls)					DONE
	- Cap shots fired based on ammo capacity
- Change sprites to a more hi-res, cartoon-y type
- Change sprite when damaged (?)
- Animate the running and firing
- Change tiles to create a theme (modern city, lab, sewers, etc.)		DONE
- Create two more level arrangements									DONE
- Add special elements to some levels (teleporters, spikes, etc.)
- Add powerups (flipscreen, overshield, rapidfire, speed burst)
- Add pickup weapons (rockets, grenades, machine gun, etc.)
- Animate the detonation of explosive weapons 
- Add effects for getting shot
- Add spawn zones
- Add player attributes (health, armor)									DONE
- Add double jump, dash, crouch, heal, block abilities
	- Add crouce/prone shooting
- Add "Pause" option
- Add options to use a controller

MENU
- Add option for number of players selection
- Add lighting as a decoration on menu

HUD
- Add health and armor									DONE
- Add ammo and current weapon
- Add picture of player

GAME OVER
- Add score after last round							DONE
- Add score to win										DONE
- Add field congratulating winner						DONE
- Add options to return to menu, or restart				DONE

*/

#include "GameApp.h";
#include <SDL_mixer.h>;
#include <string>;
using namespace std;

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif 

enum GameState { STATE_MAIN_MENU, STATE_LEVEL_SELECT, STATE_GAME_LEVEL, STATE_ROUND_OVER, STATE_GAME_OVER };
enum Level { FOREST, CAVE, SEWERS };

unsigned char forestData[LEVEL_HEIGHT][LEVEL_WIDTH] = {						// Globally defining the tile data for "FOREST"
	{ 90, 4, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 4, 90 },
	{ 90, 4, 90, 6, 6, 6, 6, 6, 90, 90, 90, 90, 90, 90, 6, 6, 6, 6, 6, 90, 4, 90 },
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

unsigned char sewersData[LEVEL_HEIGHT][LEVEL_WIDTH] = {					// Globally defining the tile data for "SEWERS"
	{ 102, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 100},
	{ 102, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 100 },
	{ 102, 17, 17, 17, 17, 41, 17, 17, 41, 17, 17, 41, 17, 17, 17, 41, 41, 17, 17, 41, 17, 100},
	{ 102, 17, 17, 41, 17, 17, 17, 41, 41, 41, 17, 41, 17, 41, 41, 17, 41, 41, 17, 17, 41, 100},
	{ 102, 17, 41, 17, 17, 41, 17, 41, 17, 1, 1, 1, 1, 17, 17, 41, 17, 41, 41, 17, 41, 100},
	{ 102, 24, 17, 17, 17, 41, 41, 17, 41, 41, 17, 17, 17, 17, 17, 17, 41, 17, 17, 17, 24, 100},
	{ 102, 1, 1, 17, 17, 41, 17, 17, 41, 17, 41, 17, 17, 17, 17, 41, 41, 17, 17, 1, 1, 100 },
	{ 102, 17, 17, 17, 17, 41, 17, 17, 41, 17, 17, 17, 17, 17, 17, 41, 17, 17, 17, 17, 17, 100 },
	{ 102, 17, 17, 17, 17, 0, 1, 2, 41, 17, 41, 17, 17, 17, 0, 1, 2, 17, 17, 17, 17, 100 },
	{ 102, 17, 17, 17, 17, 16, 17, 18, 41, 17, 17, 17, 17, 17, 16, 17, 18, 17, 17, 17, 17, 100 },
	{ 102, 1, 17, 17, 17, 16, 17, 18, 41, 17, 1, 1, 17, 17, 16, 17, 18, 17, 17, 17, 1, 100 },
	{ 102, 17, 17, 17, 17, 16, 17, 18, 41, 17, 17, 17, 17, 17, 16, 17, 18, 17, 17, 17, 17, 100 },
	{ 102, 33, 33, 33, 33, 16, 17, 18, 33, 33, 33, 33, 33, 33, 16, 17, 18, 33, 33, 33, 33, 100},
	{ 134, 160, 161, 162, 163, 160, 160, 160, 155, 156, 156, 156, 156, 157, 160, 160, 160, 161, 162, 163, 160, 132},
	{ 150, 176, 177, 187, 179, 176, 176, 176, 171, 172, 172, 172, 172, 173, 176, 176, 176, 177, 188, 179, 176, 148},	
	{ 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192,},
};

unsigned char caveData[LEVEL_HEIGHT][LEVEL_WIDTH] = {					// Globally defining the tile data for "CAVE"
	{ 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, },
	{ 7, 8, 9, 7, 7, 8, 7, 8, 9, 9, 7, 8, 7, 9, 7, 9, 7, 8, 8, 8, 7, 7},
	{ 52, 49, 25, 25, 25, 49, 49, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 54 },
	{ 52, 25, 25, 25, 49, 49, 49, 25, 25, 49, 25, 25, 25, 25, 49, 49, 25, 25, 25, 25, 25, 54 },
	{ 39, 39, 25, 25, 25, 25, 25, 25, 49, 49, 25, 25, 25, 25, 49, 49, 25, 25, 25, 25, 39, 39 },
	{ 52, 25, 49, 25, 25, 25, 25, 25, 49, 49, 25, 25, 49, 25, 25, 25, 25, 49, 49, 25, 25, 54 },
	{ 52, 25, 25, 49, 25, 25, 49, 49, 25, 25, 25, 25, 49, 25, 25, 25, 25, 49, 25, 25, 25, 54 },
	{ 52, 25, 25, 49, 25, 25, 25, 25, 49, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 54 },
	{ 39, 39, 40, 25, 25, 25, 25, 49, 49, 25, 25, 25, 25, 49, 49, 25, 25, 25, 25, 38, 39, 39 },
	{ 25, 25, 54, 25, 25, 25, 25, 49, 49, 25, 25, 25, 25, 25, 25, 25, 49, 25, 25, 52, 25, 25, },
	{ 25, 25, 54, 25, 25, 25, 25, 49, 25, 41, 25, 25, 41, 49, 25, 25, 25, 25, 25, 52, 25, 25, },
	{ 25, 25, 25, 39, 39, 40, 25, 25, 25, 55, 25, 25, 55, 25, 25, 25, 38, 39, 39, 25, 25, 25, },
	{ 49, 25, 25, 25, 25, 54, 25, 25, 25, 55, 25, 25, 55, 25, 25, 25, 52, 25, 25, 25, 25, 49, },
	{ 49, 49, 25, 25, 25, 54, 25, 25, 25, 55, 25, 25, 55, 25, 25, 25, 52, 25, 25, 25, 49, 25, },
	{ 50, 35, 36, 36, 35, 37, 37, 35, 36, 36, 37, 37, 37, 35, 36, 37, 35, 36, 35, 36, 37, 51 },
	{ 52, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 54 },
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

void GameApp::DrawHUD(int fontTexture, string text, float size, float spacing, float x, float y) {
	float texture_size = 1.0 / 16.0f;
	vector<float> vertexData;
	vector<float> texCoordData;

	Matrix modelMatrix;
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
	displayWindow = SDL_CreateWindow("Final Project", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL);
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

	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);					// Setting bgm and sound effects
	menuBGM = Mix_LoadMUS("menu.wav");
	shootSFX = Mix_LoadWAV("shoot.wav");
	jumpSFX = Mix_LoadWAV("jump.wav");
	gameoverSFX = Mix_LoadWAV("gameover.wav");
	sewersBGM = Mix_LoadMUS("sewers.wav");
	forestBGM = Mix_LoadMUS("forest.wav");
	caveBGM = Mix_LoadMUS("caves.mp3");

	sewersTexture = LoadTexture(RESOURCE_FOLDER"sewer_1.png");
	forestTexture = LoadTexture(RESOURCE_FOLDER"demo_sprites.png");
	caveTexture = LoadTexture(RESOURCE_FOLDER"sheet_4.png");
	playerTexture = LoadTexture(RESOURCE_FOLDER"space_cartoon.png");
	fontTexture = LoadTexture(RESOURCE_FOLDER"modern_font.png");
	bulletTexture = LoadTexture(RESOURCE_FOLDER"space_cartoon.png");

	player1 = new Entity();
	players.push_back(player1);
	player2 = new Entity();
	players.push_back(player2);

	RenderMainMenu();
}

GameApp::~GameApp() {

	Mix_FreeChunk(shootSFX);
	Mix_FreeMusic(menuBGM);
	Mix_FreeMusic(forestBGM);
	Mix_FreeMusic(sewersBGM);

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
			if (event.key.keysym.scancode == SDL_SCANCODE_LSHIFT && state == STATE_GAME_LEVEL && player1->active){		// Game level events
				shootBullet(player1);
			}
			if (event.key.keysym.scancode == SDL_SCANCODE_RSHIFT && state == STATE_GAME_LEVEL && player2->active){
				shootBullet(player2);
			}
			if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE && state == STATE_GAME_LEVEL) {
				viewMatrix.identity();
				Mix_HaltMusic();
				RenderMainMenu();
				p1kills = 0;
				p2kills = 0;
			}

			else if (event.key.keysym.scancode == SDL_SCANCODE_SPACE && state == STATE_MAIN_MENU) {					// Main menu events
				RenderLevelSelect();
			}

			else if (state == STATE_LEVEL_SELECT && event.key.keysym.scancode == SDL_SCANCODE_SPACE){		// Level select events
				Mix_HaltMusic();
				RenderGameLevel();
			}

			if (event.key.keysym.scancode == SDL_SCANCODE_SPACE && state == STATE_ROUND_OVER) {				// Round over events
				RenderGameLevel();
			}
			else if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE && state == STATE_ROUND_OVER) {	
				done = true;
			}

			if (event.key.keysym.scancode == SDL_SCANCODE_SPACE && state == STATE_GAME_OVER) {				// Game over events
				Mix_HaltMusic();
				RenderMainMenu();
			}
			else if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE && state == STATE_GAME_OVER) {
				done = true;
			}

		}
	}

	glClearColor(0.0f, 0.0f, 0.15f, .5f);
	glClear(GL_COLOR_BUFFER_BIT);

	program->setModelMatrix(modelMatrix);
	program->setViewMatrix(viewMatrix);
	program->setProjectionMatrix(projectionMatrix);

	glUseProgram(program->programID);

	if (state == STATE_MAIN_MENU && Mix_PlayingMusic() == 0){				// Handling currently playing music
		Mix_PlayMusic(menuBGM, -1);
	}
	else if (level == SEWERS && Mix_PlayingMusic() == 0){
		Mix_PlayMusic(sewersBGM, -1);
	}
	else if (level == FOREST && Mix_PlayingMusic() == 0){
		Mix_PlayMusic(forestBGM, -1);
	}
	else if (level == CAVE && Mix_PlayingMusic() == 0){
		Mix_PlayMusic(caveBGM, -1);
	}

	switch (state) {
	case STATE_MAIN_MENU:
		DrawText(fontTexture, "FINAL PROJECT", 1.5f, -.35f, -7.2f, 2.0f);
		DrawText(fontTexture, "PRESS [SPACE] TO BEGIN!", 0.7f, -.35f, -4.0f, -2.5f);
		break;

	case STATE_LEVEL_SELECT:
		viewMatrix.identity();
		DrawText(fontTexture, "LEVEL SELECT", 1.0f, -.35f, -3.75f, 3.0f);
		DrawText(fontTexture, "(1) FOREST", 0.5f, -.25f, -7.50f, -1.0f);
		DrawText(fontTexture, "(2) SEWERS", 0.5f, -.25f, -1.50f, -1.0f);
		DrawText(fontTexture, "(3) CAVE", 0.5f, -.25f, 5.50f, -1.0f);

		if (event.key.keysym.scancode == SDL_SCANCODE_1){
			level = FOREST;
			DrawText(fontTexture, "FOREST SELECTED", 0.5f, -.2f, -2.50f, -8.0f);
		}
		else if (event.key.keysym.scancode == SDL_SCANCODE_2){
			level = SEWERS;
			DrawText(fontTexture, "SEWERS SELECTED", 0.5f, -.2f, -2.50f, -8.0f);
		}
		else if (event.key.keysym.scancode == SDL_SCANCODE_3){
			level = CAVE;
			DrawText(fontTexture, "CAVE SELECTED", 0.5f, -.2f, -2.25f, -8.0f);
		}

		break;

	case STATE_GAME_LEVEL:
		modelMatrix.identity();
		buildLevel();
		if (level == FOREST){
			renderForestTilemap();
		}
		else if (level == SEWERS){
			renderSewersTilemap();
		}
		else if (level == CAVE){
			renderCaveTilemap();
		}
		player1->Render(program);
		player2->Render(program);
		for (Entity& projectile : projectiles){
			projectile.Render(program);
		}
		DrawHUD(fontTexture, "PLAYER 1", .5f, -.2f, 0.25f, -0.25f);
		DrawHUD(fontTexture, "HEALTH:" + to_string(int(player1->health)), .5f, -.2f, 0.25f, -0.75f);
		DrawHUD(fontTexture, "PLAYER 2", .5f, -.2f, 19.00f, -0.25f);
		DrawHUD(fontTexture, "HEALTH:" + to_string(int(player2->health)), .5f, -.2f, 19.00f, -0.75f);
		DrawHUD(fontTexture, to_string(p1kills) + "-" + to_string(p2kills), .75f, -.2f, 10.45f, -0.55f);
		break;

	case STATE_ROUND_OVER:
		viewMatrix.identity();
		DrawText(fontTexture, "ROUND OVER", 1.5f, -.35f, -5.50f, 2.0f);
		if (p1kills == 1){
			DrawText(fontTexture, "PLAYER 1: " + to_string(p1kills) + " KILL", 0.7f, -.35f, -3.1f, -0.5f);
		}
		else{
			DrawText(fontTexture, "PLAYER 1: " + to_string(p1kills) + " KILLS", 0.7f, -.35f, -3.125f, -0.5f);
		}
		if (p2kills == 1){
			DrawText(fontTexture, "PLAYER 2: " + to_string(p2kills) + " KILL", 0.7f, -.35f, -3.1f, -1.5f);
		}
		else{
			DrawText(fontTexture, "PLAYER 2: " + to_string(p2kills) + " KILLS", 0.7f, -.35f, -3.125f, -1.5f);
		}
		DrawText(fontTexture, "3 KILLS TO WIN!", 0.7f, -.35f, -2.75f, -3.5f);
		DrawText(fontTexture, "PRESS [SPACE] TO BEGIN THE NEXT ROUND", 0.6f, -.25f, -6.5f, -8.0f);
		DrawText(fontTexture, "PRESS [ESC] TO QUIT", 0.6f, -.25f, -3.5f, -9.0f);
		break;

	case STATE_GAME_OVER:
		viewMatrix.identity();
		DrawText(fontTexture, "GAME OVER", 1.5f, -.35f, -5.25f, 2.0f);
		if (winner == player1){
			DrawText(fontTexture, "PLAYER 1 WINS!!!", 0.7f, -.35f, -3.125f, -1.5f);
		}
		else if (winner == player2){
			DrawText(fontTexture, "PLAYER 2 WINS!!!", 0.7f, -.35f, -3.125f, -0.5f);
		}
		DrawText(fontTexture, "PRESS [SPACE] TO RETURN TO THE MAIN MENU", 0.6f, -.25f, -7.5f, -8.0f);
		DrawText(fontTexture, "PRESS [ESC] TO QUIT", 0.6f, -.25f, -3.5f, -9.0f);
		level = 0;
		p1kills = 0;
		p2kills = 0;
		break;
	}
	SDL_GL_SwapWindow(displayWindow);
}

void GameApp::worldToTileCoordinates(float worldX, float worldY, int *gridX, int *gridY) {
	if (level == FOREST){
		*gridX = (int)(worldX / FOREST_TILE_SIZE);
		*gridY = (int)(-worldY / FOREST_TILE_SIZE);
	}
	else if (level == SEWERS){
		*gridX = (int)(worldX / SEWERS_TILE_SIZE);
		*gridY = (int)(-worldY / SEWERS_TILE_SIZE);
	}
	else if (level == CAVE){
		*gridX = (int)(worldX / CAVE_TILE_SIZE);
		*gridY = (int)(-worldY / CAVE_TILE_SIZE);
	}
}

bool GameApp::checkIfSolid(int gridX, int gridY, unsigned char levelData[LEVEL_HEIGHT][LEVEL_WIDTH]) {

	if (level == FOREST){
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
	}
	else if (level == SEWERS){
		if (gridX >= 0 && gridY >= 0 && (levelData[gridY][gridX] == 0 ||
			levelData[gridY][gridX] == 1 ||
			levelData[gridY][gridX] == 2 ||
			levelData[gridY][gridX] == 16 ||
			levelData[gridY][gridX] == 18 ||
			levelData[gridY][gridX] == 160 ||
			levelData[gridY][gridX] == 161 ||
			levelData[gridY][gridX] == 162 ||
			levelData[gridY][gridX] == 163 ||
			levelData[gridY][gridX] == 100 ||
			levelData[gridY][gridX] == 156 ||
			levelData[gridY][gridX] == 155 ||
			levelData[gridY][gridX] == 157 ||
			levelData[gridY][gridX] == 102)) {
			return true;
		}
	}
	else if (level == CAVE){
		if (gridX >= 0 && gridY >= 0 && (levelData[gridY][gridX] == 39 ||
			levelData[gridY][gridX] == 67 ||
			levelData[gridY][gridX] == 68 ||
			levelData[gridY][gridX] == 69 ||
			levelData[gridY][gridX] == 81 ||
			levelData[gridY][gridX] == 83 ||
			levelData[gridY][gridX] == 40 ||
			levelData[gridY][gridX] == 35 ||
			levelData[gridY][gridX] == 36 ||
			levelData[gridY][gridX] == 37 ||
			levelData[gridY][gridX] == 52 ||
			levelData[gridY][gridX] == 54 ||
			levelData[gridY][gridX] == 41 ||
			levelData[gridY][gridX] == 38 ||
			levelData[gridY][gridX] == 95 ||
			levelData[gridY][gridX] == 96 ||
			levelData[gridY][gridX] == 97 ||
			levelData[gridY][gridX] == 55)) {
			return true;
		}
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

void GameApp::RenderLevelSelect(){
	state = STATE_LEVEL_SELECT;
}

void GameApp::RenderRoundOver(){
	state = STATE_ROUND_OVER;
}

void GameApp::RenderGameLevel() {

	state = STATE_GAME_LEVEL;

	glClearColor(0.2f, 0.4f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	projectiles.clear();
	
	if (level == FOREST){
		player1->position_x = 4.5f;
		player1->position_y = -7.5f;
	}	
	else if (level == SEWERS){
		player1->position_x = 2.5f;
		player1->position_y = -5.5f;
	}
	else if (level == CAVE){
		player1->position_x = 2.5f;
		player1->position_y = -6.5f;
	}
	player1->acceleration_y = lerp(0.0f, -20.0f, 2);
	player1->sprite = SheetSprite(playerTexture, 425.0f / 1024.0f, 404.0f / 512.0f, 40.0f / 1024.0f, 60.0f / 512.0f, 1.0f);
	player1->friction_x = 2.0f;
	player1->friction_y = 2.0f;
	player1->hitboxHeight = player1->height * .4f;
	player1->hitboxWidth = player1->width * .4f;
	player1->health = 100.0f;
	player1->isStatic = false;
	player1->active = true;
	player1->facing = 1.0f;
	
	if (level == FOREST){
		player2->position_x = 17.5f;
		player2->position_y = -7.5f;
	}	
	else if (level == SEWERS){
		player2->position_x = 19.5f;
		player2->position_y = -5.5f;
	}
	else if (level == CAVE){
		player2->position_x = 19.5f;
		player2->position_y = -6.5f;
	}
	player2->acceleration_y = lerp(0.0f, -20.0f, 2);
	player2->sprite = SheetSprite(playerTexture, 420.0f / 1024.0f, 0.0f / 512.0f, 40.0f / 1024.0f, 66.0f / 512.0f, 1.0f);
	player2->friction_x = 2.0f;
	player2->friction_y = 2.0f;
	player2->hitboxHeight = player2->height * .4f;
	player2->hitboxWidth = player2->width * .4f;
	player2->health = 100.0f;
	player2->isStatic = false;
	player2->active = true;
	player2->facing = -1.0f;
	
	viewMatrix.Translate(-11.0, 5.0, 0);

}

void GameApp::UpdateGameLevel(float elapsed) {

	const Uint8 *keys = SDL_GetKeyboardState(NULL);

	if (keys[SDL_SCANCODE_A]) {							// Player 1 movement updates
		player1->acceleration_x = lerp(player1->acceleration_x, -25.0f, 2);
		player1->facing = -1.0f;
	}
	else if (keys[SDL_SCANCODE_D]) {
		player1->acceleration_x = lerp(player1->acceleration_x, 25.0f, 2);
		player1->facing = 1.0f;
	}
	else {
		player1->acceleration_x = lerp(player1->acceleration_x, 0.0f, 1.5);
	}
	if (keys[SDL_SCANCODE_W] && player1->collidedBottom) {
		player1->velocity_y = 25.0f;
		player1->collidedBottom = false;
		Mix_PlayChannel(-1, jumpSFX, 0);
	}

	if (keys[SDL_SCANCODE_LEFT]) {						// Player 2 movement updates
		player2->acceleration_x = lerp(player2->acceleration_x, -25.0f, 2);
		player2->facing = -1.0f;
	}
	else if (keys[SDL_SCANCODE_RIGHT]) {
		player2->acceleration_x = lerp(player2->acceleration_x, 25.0f, 2);
		player2->facing = 1.0f;
	}
	else {
		player2->acceleration_x = lerp(player2->acceleration_x, 0.0f, 1.5);
	}
	if (keys[SDL_SCANCODE_UP] && player2->collidedBottom) {
		player2->velocity_y = 25.0f;
		player2->collidedBottom = false;
		Mix_PlayChannel(-1, jumpSFX, 0);
	}

	for (size_t i = 0; i < projectiles.size(); i++){					// Projectile updating and collision checking
		if (!projectiles[i].isStatic && projectiles[i].active) {		// Handling players after collision
			projectiles[i].Update(elapsed);
			for (size_t j = 0; j < players.size(); j++){
				if (players[j]->active && projectiles[i].collidesWith(players[j])){
					players[j]->health -= 5;
					projectiles[i].active = false;
					if (players[j]->health == 0){							// Health checking each player
						players[j]->active = false;
						if (players[j] == player1){							// Checking and handling winner of each round
							p2kills += 1;									// and checking if someone won the game
							if (p2kills == 3){
								winner = player2;
								state = STATE_GAME_OVER;
								Mix_PlayChannel(-1, gameoverSFX, 0);
								break;
							}
						}
						else if (players[j] == player2){
							p1kills += 1;
							if (p1kills == 3){
								winner = player1;
								state = STATE_GAME_OVER;
								Mix_PlayChannel(-1, gameoverSFX, 0);
								break;
							}
						}
						state = STATE_ROUND_OVER;
						break;
					}
				}
			}
		}
	}

	for (size_t i = 0; i < players.size(); i++)						// Collision checking and updating for each player
		if (!players[i]->isStatic && players[i]->active && level == FOREST) {

			players[i]->Update(elapsed);

			int gridX = 0, gridY = 0;

			worldToTileCoordinates(players[i]->position_x - players[i]->width / 3, players[i]->position_y, &gridX, &gridY);
			if (checkIfSolid(gridX, gridY, levelData)) {
				players[i]->position_x = (gridX + 1) * FOREST_TILE_SIZE + (players[i]->width / 3) + 0.005f;
				players[i]->velocity_x = 0;
				players[i]->collidedLeft = true;
			}

			worldToTileCoordinates(players[i]->position_x + players[i]->width / 3, players[i]->position_y, &gridX, &gridY);
			if (checkIfSolid(gridX, gridY, levelData)) {
				players[i]->position_x = gridX * FOREST_TILE_SIZE - (players[i]->width / 3) - 0.005f;
				players[i]->velocity_x = 0;
				players[i]->collidedRight = true;
			}

			worldToTileCoordinates(players[i]->position_x, players[i]->position_y + players[i]->height / 2, &gridX, &gridY);
			if (checkIfSolid(gridX, gridY, levelData)) {
				players[i]->position_y = (-gridY - 1) * FOREST_TILE_SIZE - players[i]->height / 2 - 0.005f;
				players[i]->velocity_y = 0;
				players[i]->collidedTop = true;
			}

			worldToTileCoordinates(players[i]->position_x, players[i]->position_y - players[i]->height / 2, &gridX, &gridY);
			if (checkIfSolid(gridX, gridY, levelData)) {
				players[i]->position_y = -gridY * FOREST_TILE_SIZE + players[i]->height / 2 + 0.005f;
				players[i]->velocity_y = 0;
				players[i]->collidedBottom = true;
			}
		}
		
	else if (!players[i]->isStatic && players[i]->active && level == SEWERS) {

		players[i]->Update(elapsed);

		int gridX = 0, gridY = 0;

		worldToTileCoordinates(players[i]->position_x - players[i]->width / 3, players[i]->position_y, &gridX, &gridY);
		if (checkIfSolid(gridX, gridY, levelData)) {
			players[i]->position_x = (gridX + 1) * SEWERS_TILE_SIZE + (players[i]->width / 3) + 0.005f;
			players[i]->velocity_x = 0;
			players[i]->collidedLeft = true;
		}

		worldToTileCoordinates(players[i]->position_x + players[i]->width / 3, players[i]->position_y, &gridX, &gridY);
		if (checkIfSolid(gridX, gridY, levelData)) {
			players[i]->position_x = gridX * SEWERS_TILE_SIZE - (players[i]->width / 3) - 0.005f;
			players[i]->velocity_x = 0;
			players[i]->collidedRight = true;
		}

		worldToTileCoordinates(players[i]->position_x, players[i]->position_y + players[i]->height / 2, &gridX, &gridY);
		if (checkIfSolid(gridX, gridY, levelData)) {
			players[i]->position_y = (-gridY - 1) * SEWERS_TILE_SIZE - players[i]->height / 2 - 0.005f;
			players[i]->velocity_y = 0;
			players[i]->collidedTop = true;
		}

		worldToTileCoordinates(players[i]->position_x, players[i]->position_y - players[i]->height / 2, &gridX, &gridY);
		if (checkIfSolid(gridX, gridY, levelData)) {
			players[i]->position_y = -gridY * SEWERS_TILE_SIZE + players[i]->height / 2 + 0.005f;
			players[i]->velocity_y = 0;
			players[i]->collidedBottom = true;
		}
	}

	else if (!players[i]->isStatic && players[i]->active && level == CAVE) {

		players[i]->Update(elapsed);

		int gridX = 0, gridY = 0;

		worldToTileCoordinates(players[i]->position_x - players[i]->width / 3, players[i]->position_y, &gridX, &gridY);
		if (checkIfSolid(gridX, gridY, levelData)) {
			players[i]->position_x = (gridX + 1) * CAVE_TILE_SIZE + (players[i]->width / 3) + 0.005f;
			players[i]->velocity_x = 0;
			players[i]->collidedLeft = true;
		}

		worldToTileCoordinates(players[i]->position_x + players[i]->width / 3, players[i]->position_y, &gridX, &gridY);
		if (checkIfSolid(gridX, gridY, levelData)) {
			players[i]->position_x = gridX * CAVE_TILE_SIZE - (players[i]->width / 3) - 0.005f;
			players[i]->velocity_x = 0;
			players[i]->collidedRight = true;
		}

		worldToTileCoordinates(players[i]->position_x, players[i]->position_y + players[i]->height / 2, &gridX, &gridY);
		if (checkIfSolid(gridX, gridY, levelData)) {
			players[i]->position_y = (-gridY - 1) * CAVE_TILE_SIZE - players[i]->height / 2 - 0.005f;
			players[i]->velocity_y = 0;
			players[i]->collidedTop = true;
		}

		worldToTileCoordinates(players[i]->position_x, players[i]->position_y - players[i]->height / 2, &gridX, &gridY);
		if (checkIfSolid(gridX, gridY, levelData)) {
			players[i]->position_y = -gridY * CAVE_TILE_SIZE + players[i]->height / 2 + 0.005f;
			players[i]->velocity_y = 0;
			players[i]->collidedBottom = true;
		}
	}

	for (size_t i = 0; i < projectiles.size(); i++)						// Collision checking and updating for each projectile against the world
		if (!projectiles[i].isStatic && projectiles[i].active && level == FOREST) {

			projectiles[i].Update(elapsed);

			int gridX = 0, gridY = 0;

			worldToTileCoordinates(projectiles[i].position_x - projectiles[i].width / 2, projectiles[i].position_y, &gridX, &gridY);
			if (checkIfSolid(gridX, gridY, levelData)) {
				projectiles[i].active = false;
			}

			worldToTileCoordinates(projectiles[i].position_x + projectiles[i].width / 2, projectiles[i].position_y, &gridX, &gridY);
			if (checkIfSolid(gridX, gridY, levelData)) {
				projectiles[i].active = false;
			}
		}

		else if (!projectiles[i].isStatic && projectiles[i].active && level == SEWERS) {

			projectiles[i].Update(elapsed);

			int gridX = 0, gridY = 0;

			worldToTileCoordinates(projectiles[i].position_x - projectiles[i].width / 2, projectiles[i].position_y, &gridX, &gridY);
			if (checkIfSolid(gridX, gridY, levelData)) {
				projectiles[i].active = false;
			}

			worldToTileCoordinates(projectiles[i].position_x + projectiles[i].width / 2, projectiles[i].position_y, &gridX, &gridY);
			if (checkIfSolid(gridX, gridY, levelData)) {
				projectiles[i].active = false;
			}

		}

		else if (!projectiles[i].isStatic && projectiles[i].active && level == CAVE) {

			projectiles[i].Update(elapsed);

			int gridX = 0, gridY = 0;

			worldToTileCoordinates(projectiles[i].position_x - projectiles[i].width / 2, projectiles[i].position_y, &gridX, &gridY);
			if (checkIfSolid(gridX, gridY, levelData)) {
				projectiles[i].active = false;
			}

			worldToTileCoordinates(projectiles[i].position_x + projectiles[i].width / 2, projectiles[i].position_y, &gridX, &gridY);
			if (checkIfSolid(gridX, gridY, levelData)) {
				projectiles[i].active = false;
			}
		}

}

void GameApp::buildLevel() {
	if (level == FOREST){
		memcpy(levelData, forestData, LEVEL_HEIGHT*LEVEL_WIDTH);
	}
	else if (level == SEWERS){
		memcpy(levelData, sewersData, LEVEL_HEIGHT*LEVEL_WIDTH);
	}
	else if (level == CAVE){
		memcpy(levelData, caveData, LEVEL_HEIGHT*LEVEL_WIDTH);
	}
}

void GameApp::renderForestTilemap() {
	vector<float> vertexData;
	vector<float> texCoordData;

	for (int y = 0; y < LEVEL_HEIGHT; y++) {
		for (int x = 0; x < LEVEL_WIDTH; x++) {

			float u = (float)(((int)levelData[y][x]) % FOREST_SPRITE_COUNT_X) / (float)FOREST_SPRITE_COUNT_X;
			float v = (float)(((int)levelData[y][x]) / FOREST_SPRITE_COUNT_X) / (float)FOREST_SPRITE_COUNT_Y;

			float spriteWidth = 1.0f / (float)FOREST_SPRITE_COUNT_X;
			float spriteHeight = 1.0f / (float)FOREST_SPRITE_COUNT_Y;

			vertexData.insert(vertexData.end(), {

				FOREST_TILE_SIZE * x, -FOREST_TILE_SIZE * y,
				FOREST_TILE_SIZE * x, (-FOREST_TILE_SIZE * y) - FOREST_TILE_SIZE,
				(FOREST_TILE_SIZE * x) + FOREST_TILE_SIZE, (-FOREST_TILE_SIZE * y) - FOREST_TILE_SIZE,

				FOREST_TILE_SIZE * x, -FOREST_TILE_SIZE * y,
				(FOREST_TILE_SIZE * x) + FOREST_TILE_SIZE, (-FOREST_TILE_SIZE * y) - FOREST_TILE_SIZE,
				(FOREST_TILE_SIZE * x) + FOREST_TILE_SIZE, -FOREST_TILE_SIZE * y
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

	glBindTexture(GL_TEXTURE_2D, forestTexture);
	glDrawArrays(GL_TRIANGLES, 0, 6 * LEVEL_HEIGHT * LEVEL_WIDTH);

	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
}

void GameApp::renderSewersTilemap() {

	vector<float> vertexData;
	vector<float> texCoordData;

	for (int y = 0; y < LEVEL_HEIGHT; y++) {
		for (int x = 0; x < LEVEL_WIDTH; x++) {

			float u = (float)(((int)levelData[y][x]) % SEWERS_SPRITE_COUNT_X) / (float)SEWERS_SPRITE_COUNT_X;
			float v = (float)(((int)levelData[y][x]) / SEWERS_SPRITE_COUNT_X) / (float)SEWERS_SPRITE_COUNT_Y;

			float spriteWidth = 1.0f / (float)SEWERS_SPRITE_COUNT_X;
			float spriteHeight = 1.0f / (float)SEWERS_SPRITE_COUNT_Y;

			vertexData.insert(vertexData.end(), {

				SEWERS_TILE_SIZE * x, -SEWERS_TILE_SIZE * y,
				SEWERS_TILE_SIZE * x, (-SEWERS_TILE_SIZE * y) - SEWERS_TILE_SIZE,
				(SEWERS_TILE_SIZE * x) + SEWERS_TILE_SIZE, (-SEWERS_TILE_SIZE * y) - SEWERS_TILE_SIZE,

				SEWERS_TILE_SIZE * x, -SEWERS_TILE_SIZE * y,
				(SEWERS_TILE_SIZE * x) + SEWERS_TILE_SIZE, (-SEWERS_TILE_SIZE * y) - SEWERS_TILE_SIZE,
				(SEWERS_TILE_SIZE * x) + SEWERS_TILE_SIZE, -SEWERS_TILE_SIZE * y
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

	glBindTexture(GL_TEXTURE_2D, sewersTexture);
	glDrawArrays(GL_TRIANGLES, 0, 6 * LEVEL_HEIGHT * LEVEL_WIDTH);

	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
}

void GameApp::renderCaveTilemap() {

	vector<float> vertexData;
	vector<float> texCoordData;

	for (int y = 0; y < LEVEL_HEIGHT; y++) {
		for (int x = 0; x < LEVEL_WIDTH; x++) {

			float u = (float)(((int)levelData[y][x]) % CAVE_SPRITE_COUNT_X) / (float)CAVE_SPRITE_COUNT_X;
			float v = (float)(((int)levelData[y][x]) / CAVE_SPRITE_COUNT_X) / (float)CAVE_SPRITE_COUNT_Y;

			float spriteWidth = 1.0f / (float)CAVE_SPRITE_COUNT_X;
			float spriteHeight = 1.0f / (float)CAVE_SPRITE_COUNT_Y;

			vertexData.insert(vertexData.end(), {

				CAVE_TILE_SIZE * x, -CAVE_TILE_SIZE * y,
				CAVE_TILE_SIZE * x, (-CAVE_TILE_SIZE * y) - CAVE_TILE_SIZE,
				(CAVE_TILE_SIZE * x) + CAVE_TILE_SIZE, (-CAVE_TILE_SIZE * y) - CAVE_TILE_SIZE,

				CAVE_TILE_SIZE * x, -CAVE_TILE_SIZE * y,
				(CAVE_TILE_SIZE * x) + CAVE_TILE_SIZE, (-CAVE_TILE_SIZE * y) - CAVE_TILE_SIZE,
				(CAVE_TILE_SIZE * x) + CAVE_TILE_SIZE, -CAVE_TILE_SIZE * y
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

	glBindTexture(GL_TEXTURE_2D, caveTexture);
	glDrawArrays(GL_TRIANGLES, 0, 6 * LEVEL_HEIGHT * LEVEL_WIDTH);

	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
}

void GameApp::shootBullet(Entity* player) {				// Function that hires the firing of a bullet by the player
	Entity* bullet = new  Entity();
	bullet->sprite = SheetSprite(bulletTexture, 392.0f / 1024.0f, 478.0f / 512.0f, 29.0f / 1024.0f, 33.0f / 512.0f, .25f);
	bullet->position_x = player->position_x + (player->width/2 * player->facing);
	bullet->position_y = player->position_y;
	bullet->velocity_x = 20.0f * player->facing;
	bullet->velocity_y = 0.0f;
	bullet->hitboxHeight = bullet->height;
	bullet->hitboxWidth = bullet->width;
	bullet->isStatic = false;
	bullet->active = true;
	Mix_PlayChannel(-1, shootSFX, 0);
	projectiles.push_back(*bullet);
}

float GameApp::lerp(float from, float to, float time) {
	return (1.0 - time)*from + time*to;
}