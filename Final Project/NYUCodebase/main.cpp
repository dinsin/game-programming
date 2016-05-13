/*

Dinesh Singh
CS3113 - Intro to Game Programming
Prof. Ivan Safrin

Final Project

*/

#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "GameApp.h"

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

int main(int argc, char *argv[]) {

	GameApp newGame;										// GameApp's constructor calls the Setup() function outside of the loop
	while (!newGame.ProcessUpdateAndRender()) {				// The while loop's condition is the function from the GameApp class that
	}														// constantly runs the body of the loop (ProcessEvents(), Update(), and Render())
	return 0;												// Thus, it runs until done returns "false" and breaks the loop. Then, the destructor
}															// for the GameApp class handles the cleanup.
