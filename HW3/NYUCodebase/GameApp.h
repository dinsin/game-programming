/*

Dinesh Singh
CS3913 Intro to Game Programming
Prof. Ivan Safrin

GameApp

Since the amount of global variables was getting a little crazy,
I recreated the ClassDemoApp with this GameApp to organize better.

It handles most of the major functions, so its easier when variables from one function
have to be used by variable of another.

The sprites I used are inspired by my sad Knicks team.

CONTROLS: SPACE to start game, LEFT & RIGHT to move, UP to shoot

*/

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
using namespace std;

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

class GameApp {
public:
    
    GameApp();					// Constructor and Destructor of GameApp, which handle the Setup() and Cleanup() functions
    ~GameApp();
    
    void Setup();

	void ProcessEvents();

	bool ProcessUpdateAndRender();				// Combined function that handles the ProcessEvents(), Update(elapsed), and the Render() portions
    
    void Update(float elapsed);
	void UpdateGameLevel(float elapsed);		// Only the Game Level requires active updating
    
    void RenderMainMenu();			// Different functions to render each game state
    void RenderGameLevel();
        
    void shootBullet();							// Short fuction to handle the bullet
        
    void DrawText(int fontTexture, string text, float size, float spacing, float x, float y);		// DrawText() function taken from the slides
            
private:
    
    ShaderProgram* program;    
    SDL_Event event;    
    SDL_Window* displayWindow;    
    Matrix projectionMatrix;
    Matrix modelMatrix;
    Matrix viewMatrix;
    
    float lastFrameTicks;
    
    Entity* player;    
    vector<Entity> enemies;
    Entity bullet;
    
    GLuint spritesTexture;
    GLuint fontTexture;
    
    int state;    
    int score;
    
    int enemyDirection;
    int numEnemies;
	float timeCounter;    
    bool enemyHitWall;
    bool moveDown;
    
    bool done = false;
    
};