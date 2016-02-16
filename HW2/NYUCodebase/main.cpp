/*

Dinesh Singh
CS3113 - Intro to Game Programming
Prof. Ivan Safrin

Assignment #2: Pong

*/

#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "ShaderProgram.h"
#include "Matrix.h"

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

// Entity class definition
class Entity{
public:

	Entity(ShaderProgram* program, Matrix& modelMatrix, float width, float height)
		: program(program), modelMatrix(modelMatrix), width(width), height(height)
	{
		x = 0.0f;
		y = 0.0f;
		rotation = 0.0f;
		speed = 0.0f;
		direction_x = 0.0f;
		direction_y = 0.0f;
	}

	void Draw(){
		modelMatrix.identity();

		modelMatrix.Translate(x, y, 0.0f);

		program->setModelMatrix(modelMatrix);

		float vertices[] = { -width / 2, -height / 2, width / 2, height / 2, -width / 2, height / 2,
			width / 2, height / 2, -width / 2, -height / 2, width / 2, -height / 2 };

		glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
		glEnableVertexAttribArray(program->positionAttribute);

		float texCoords[] = { 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f };

		glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
		glEnableVertexAttribArray(program->texCoordAttribute);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisableVertexAttribArray(program->positionAttribute);
		glDisableVertexAttribArray(program->texCoordAttribute);
	}

	float x;
	float y;
	float rotation;

	int textureID;

	float width;
	float height;

	float speed;
	float direction_x;
	float direction_y;

	ShaderProgram* program;
	Matrix& modelMatrix;

};

// Definition of varaibles to be used throughout main function (and subfunctions)
SDL_Window* displayWindow;
SDL_Event event;
Matrix projectionMatrix;
Matrix modelMatrix;
Matrix viewMatrix;
ShaderProgram* program;
Entity* topWall;
Entity* bottomWall;
Entity* midLine;
Entity* leftPaddle;
Entity* rightPaddle;
Entity* ball;
bool done;
int p1Wins;
int p2Wins;

GLuint LoadTexture(const char *image_path) {
	SDL_Surface *surface = IMG_Load(image_path);

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	SDL_FreeSurface(surface);

	return textureID;
}

void Setup(){
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Pong", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
	glewInit();
#endif

	glViewport(0, 0, 1280, 720);

	projectionMatrix.setOrthoProjection(-3.55, 3.55, -2.0, 2.0, -1.0, 1.0);

	program = new ShaderProgram(RESOURCE_FOLDER"vertex.glsl", RESOURCE_FOLDER"fragment.glsl");

	topWall = new Entity(program, modelMatrix, 7.1f, 0.04f);
	bottomWall = new Entity(program, modelMatrix, 7.1f, 0.04f);
	midLine = new Entity(program, modelMatrix, 0.04f, 4.0f);

	topWall->y = 2.0f - topWall->height / 2;
	bottomWall->y = -2.0f + topWall->height / 2;

	leftPaddle = new Entity(program, modelMatrix, 0.2f, 1.25f);
	rightPaddle = new Entity(program, modelMatrix, 0.2f, 1.25f);
	ball = new Entity(program, modelMatrix, 0.2f, 0.2f);

	leftPaddle->x = -3.55f + leftPaddle->width / 2;
	rightPaddle->x = 3.55f - rightPaddle->width / 2;

	leftPaddle->speed = 0.005f;
	rightPaddle->speed = 0.005f;

	ball->speed = .005f;

	ball->direction_x = .05f;
	ball->direction_y = .05f;
}

void ProcessEvents() {
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
			done = true;
		}
	}
}

void Update() {

	float lastFrameTicks = 0.0f;
	float ticks = (float)SDL_GetTicks()/ 1000.0f;
	float elapsed = ticks - lastFrameTicks;
	lastFrameTicks = ticks;

	// Check for key presses, and adjust the paddles based on the keys
	const Uint8* keys = SDL_GetKeyboardState(NULL);

	if (keys[SDL_SCANCODE_W]) {
		leftPaddle->y += leftPaddle->speed;
	}
	if (keys[SDL_SCANCODE_S]) {
		leftPaddle->y -= leftPaddle->speed;
	}
	if (keys[SDL_SCANCODE_UP]) {
		rightPaddle->y += rightPaddle->speed;
	}
	if (keys[SDL_SCANCODE_DOWN]) {
		rightPaddle->y -= rightPaddle->speed;
	}	

	// Adjusting the position of the ball based on elapsed time
	ball->x += elapsed * ball->direction_x * ball->speed;
	ball->y += elapsed * ball->direction_y * ball->speed;

	// Collisions between the ball and the two paddles
	if (!(leftPaddle->y - leftPaddle->height / 2 > ball->y + ball->height / 2 ||
		leftPaddle->y + leftPaddle->height / 2 < ball->y - ball->height / 2 ||
		leftPaddle->x - leftPaddle->width / 2 > ball->x + ball->width / 2 ||
		leftPaddle->x + leftPaddle->width / 2 < ball->x - ball->width / 2)) {
		ball->direction_x = .05f;
	}
	if (!(rightPaddle->y - rightPaddle->height / 2 > ball->y + ball->height / 2 ||
		rightPaddle->y + rightPaddle->height / 2 < ball->y - ball->height / 2 ||
		rightPaddle->x - rightPaddle->width / 2 > ball->x + ball->width / 2 ||
		rightPaddle->x + rightPaddle->width / 2 < ball->x - ball->width / 2)) {
		ball->direction_x = -.05f;
	}	
	
	// Collisions between the paddle and the top or bottom of the screen
	if (leftPaddle->y > 2.0f - leftPaddle->height / 2) {
		leftPaddle->y = 2.0f - leftPaddle->height / 2;
	}
	else if (leftPaddle->y < -2.0f + leftPaddle->height / 2) {
		leftPaddle->y = -2.0f + leftPaddle->height / 2;
	}
	if (rightPaddle->y > 2.0f - rightPaddle->height / 2) {
		rightPaddle->y = 2.0f - rightPaddle->height / 2;
	}
	else if (rightPaddle->y < -2.0f + rightPaddle->height / 2) {
		rightPaddle->y = -2.0f + rightPaddle->height / 2;
	}

	// Collisions between ball and left/right edges of the screen
	if (ball->x > 3.55f - ball->width / 2) {
		ball->direction_x = -.05;
		ball->x = 0.0f;
		ball->y = 0.0f;
		p1Wins += 1;			// Variable tracks each player's wins and keeps track. Doesn't display them yet, though
	}
	else if (ball->x < -3.55f + ball->width / 2) {
		ball->direction_x = .05;
		ball->x = 0.0f;
		ball->y = 0.0f;
		p2Wins += 1;
	}

	// Collisions between the ball and the top or bottom of the screen
	if (ball->y > 2.0f - ball->height / 2) {
		ball->direction_y = -.05f;
	}
	else if (ball->y < -2.0f + ball->height / 2) {
		ball->direction_y = .05f;
	}	
}

void Render() {
	glClearColor(0.2f, 0.5f, 0.9f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	modelMatrix.identity();

	program->setModelMatrix(modelMatrix);
	program->setViewMatrix(viewMatrix);
	program->setProjectionMatrix(projectionMatrix);

	glUseProgram(program->programID);

	topWall->Draw();
	bottomWall->Draw();
	midLine->Draw();

	leftPaddle->Draw();
	rightPaddle->Draw();
	ball->Draw();

	SDL_GL_SwapWindow(displayWindow);
}

void Cleanup(){
	delete program;
	SDL_Quit();
}

int main(int argc, char *argv[]) {	

	Setup();

	while (!done){
		ProcessEvents();
		Update();
		Render();
	}

	Cleanup();

	return 0;
}
