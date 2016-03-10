/*

Dinesh Singh
CS3913 Intro to Game Programming
Prof. Ivan Safrin

GameApp

Since the amount of global variables was getting a little crazy,
I recreated the ClassDemoApp with this GameApp to organize better.

It handles most of the major functions, so its easier when variables from one function
have to be used by variable of another.

The sprites I used are inspired by my sad Knicks team. I left comments throughout the code to explain stuff

CONTROLS: SPACE to start game, LEFT & RIGHT to move, UP to shoot

*/

#include "GameApp.h"
using namespace std;

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

enum GameState { STATE_MAIN_MENU, STATE_GAME_LEVEL };
int state;

GLuint LoadTexture(const char* image_path) {
    
	SDL_Surface* surface = IMG_Load(image_path);
    
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //GL_NEAREST = clearer
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

GameApp::GameApp():lastFrameTicks(0.0f), done(false){
    Setup();
}

GameApp::~GameApp() {
	delete program;

	SDL_Quit();
}

void GameApp::Setup() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Space Invaders", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 1280, 720);

	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
	bgm = Mix_LoadMUS("hiphopBGM.wav");
	Mix_PlayMusic(bgm, -1);
	shootSound = Mix_LoadWAV("shootball.wav");
	killSound = Mix_LoadWAV("swish.wav");
    
    projectionMatrix.setOrthoProjection(-3.55, 3.55, -2.0, 2.0, -1.0, 1.0);
    
    program = new ShaderProgram(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    spritesTexture = LoadTexture(RESOURCE_FOLDER"sprites.png");
    fontTexture = LoadTexture(RESOURCE_FOLDER"pixel_font.png");
        
    RenderMainMenu();
}

void GameApp::ProcessEvents() {
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            done = true;
        }
        else if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.scancode == SDL_SCANCODE_UP && state == STATE_GAME_LEVEL) {
                shootBullet();
            }
            else if (event.key.keysym.scancode == SDL_SCANCODE_SPACE) {
                if (state == STATE_MAIN_MENU) {
                    RenderGameLevel();
                }
            }
        }
    }
    
    glClearColor(0.3f, 0.3f, 0.8f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    program->setModelMatrix(modelMatrix);
    program->setViewMatrix(viewMatrix);
    program->setProjectionMatrix(projectionMatrix);
    
    glUseProgram(program->programID);
    
    switch (state) {
        case STATE_MAIN_MENU:
            DrawText(fontTexture, "SPACE INVADERS", 0.3, 0, -1.9f, 0.7f);
            DrawText(fontTexture, "PRESS SPACE TO START!", 0.1, 0, -1.0f, -0.5f);
            break;
            
        case STATE_GAME_LEVEL:
            player->Draw(program);
            bullet.Draw(program);
            DrawText(fontTexture, "SCORE: " + to_string(score), 0.1, 0, -0.37f, 1.8f);
            for (Entity& enemy : enemies) {
                enemy.Draw(program);
            }
            break;
    }    
    SDL_GL_SwapWindow(displayWindow);
}

bool GameApp::ProcessUpdateAndRender() {
	float ticks = (float)SDL_GetTicks() / 1000.0f;
	float elapsed = ticks - lastFrameTicks;
	lastFrameTicks = ticks;

	ProcessEvents();
	Update(elapsed);

	return done;
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

void GameApp::UpdateGameLevel(float elapsed) {

	const Uint8* keys = SDL_GetKeyboardState(NULL);

	if (keys[SDL_SCANCODE_LEFT]) {
		player->x -= elapsed * 6.0f;
		if (player->x < -3.55f + player->width / 2.0f) {
			player->x = -3.55f + player->width / 2.0f;
		}
	}
	if (keys[SDL_SCANCODE_RIGHT]) {
		player->x += elapsed * 6.0f;
		if (player->x > 3.55f - player->width / 2.0f) {
			player->x = 3.55f - player->width / 2.0f;
		}
	}

	timeCounter += elapsed;
	if (timeCounter > 0.15f) {										// Big branch monitoring the enemies movement's relatie to the player,
		for (Entity& enemy : enemies) {								// bullet, and walls. Restarts the game from the main menu if player loses.
			if (enemy.active) {
				if (player->collidesWith(enemy)) {
					RenderMainMenu();
				}
				if (moveDown) {
					enemy.y -= 0.25f;
				}
				else {
					enemy.x += 0.15f * enemyDirection;
					if (enemy.x < -3.1f || enemy.x > 3.1f) {
						enemyHitWall = true;
					}
				}
			}
		}
		moveDown = false;
		if (enemyHitWall) {
			moveDown = true;
			enemyDirection = -enemyDirection;
			enemyHitWall = false;
		}
		timeCounter -= 0.15f;
	}

	if (bullet.active) {
		bullet.y += 5.0f * elapsed;
		if (bullet.y > 2.0f) {
			bullet.active = false;
		}
		else {
			for (Entity& enemy : enemies) {
				if (bullet.collidesWith(enemy)) {
					enemy.active = false;
					bullet.active = false;
					Mix_PlayChannel(-1, killSound, 0);
					score += 100;
					numEnemies -= 1;
					if (numEnemies <= 0) {
						RenderMainMenu();
					}
					break;
				}
			}
		}
	}

}

void GameApp::RenderMainMenu() {
    state = STATE_MAIN_MENU;
}

void GameApp::RenderGameLevel() {

    state = STATE_GAME_LEVEL;
    
    score = 0;    
    timeCounter = 0;
    enemyDirection = 1.0f;
    moveDown = false;
    enemyHitWall = false;
    
    player = new Entity();								// Create player, bullet, and enemy entities from the sprite sheet. Setting the intial positions]
    player->x = 0.0f;									// and determining the hitboxes for each entity
    player->y = -2.0f + (player->height * 0.4f) / 2.0f;
    player->sprite = SheetSprite(spritesTexture, 0.0f/128.0f, 57.0f/128.0f, 54.0f/128.0f, 55.0f/128.0f, .4f);
    player->hitboxHeight = player->height * 0.4f;
    player->hitboxWidth = player->width * 0.4f;
    
	bullet = Entity();
	bullet.active = false;
	bullet.sprite = SheetSprite(spritesTexture, 56.0f / 128.0f, 57.0f / 128.0f, 50.0f / 128.0f, 49.0f / 128.0f, .25f);
	bullet.hitboxHeight = bullet.height * 0.25f;
	bullet.hitboxWidth = bullet.width * 0.25f;
    
	enemies.clear();
    for (int row = 0; row < 3; row++) {
        for (int column = 0; column < 6; column++) {
            Entity enemy;
            enemy.x = -3.0f + (column * 0.75f);
            enemy.y = 1.4f - (row * 0.5f);
			enemy.sprite = SheetSprite(spritesTexture, 0.0f / 128.0f, 0.0f / 128.0f, 64.0f / 128.0f, 55.0f / 128.0f, .35f);
            enemy.hitboxHeight = enemy.height * 0.35f;
            enemy.hitboxWidth = enemy.width * 0.35f;
            enemies.push_back(enemy);
            numEnemies++;
        }
    }    
}

void GameApp::shootBullet() {				// Function that hires the firing of a bullet by the player
	if (!bullet.active) {
        bullet.active = true;
		Mix_PlayChannel(-1, shootSound, 0);
        bullet.x = player->x;
        bullet.y = player->y;
    }
}