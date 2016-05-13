#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>

#include "ShaderProgram.h"
#include "Matrix.h"
#include "SheetSprite.h"

class Entity {
public:

	Entity();
	Entity(float x, float y, float height, float width, bool static_entity);

	void Update(float elapsed);
	void Render(ShaderProgram* program);
	bool collidesWith(Entity* object);

	SheetSprite sprite;

	int textureID;

	float position_x;
	float position_y;
	float width;
	float height;
	float velocity_x;
	float velocity_y;
	float acceleration_x;
	float acceleration_y;
	float friction_x;
	float friction_y;
	float direction_x;
	float direction_y;

	bool isStatic;
	float health;
	int jumps;

	bool active;

	float hitboxWidth;
	float hitboxHeight;

	bool collidedTop;
	bool collidedBottom;
	bool collidedLeft;
	bool collidedRight;

	float facing;

	int entityType;

	void fixPenetration(float width2, float height2);
};