#ifndef MON_ENTITY_H
#define MON_ENTITY_H

#include <glm/glm.hpp>
#include "mon_texture.h"
#include <vector>


// TODO(CK): Figure out where to put this
struct Sprite
{
	Texture2D texture;
	unsigned int VAO;
	unsigned int VBO;
};

struct CircleCollider
{
	Sprite sprite;

	glm::vec2 pos;
	glm::vec2 center;
	float radius;
};

void CreateCollider(CircleCollider* col, glm::vec2 pos, glm::vec2 center, float radius);
void UpdateCollider(CircleCollider* col, glm::vec2 pos);
int Intersects(CircleCollider a, CircleCollider b);


// Pass the collider for the x,y,z in pos and the radius ...! :)
void DrawCollider(float x, float y, float z, float radius, int numberOfSides);

class Entity
{
public:
	glm::vec2 pos;
	glm::vec2 size;
	glm::vec2 velocity;
	glm::vec3 color;
	float speed;
	float rotation;
	bool destroyed;

	
	// Components  Sprite can't be part of Default Constructor 
	// default just sets this to nulls
	Sprite sprite;
	CircleCollider col;
	// Texture2D sprite

	// TODO(CK): Maybe worry about this later (remember we jsut want to make a game)
	// IComponent 
	//  - RigidBody 
	//  - Sprite 
	Entity(const char* fileLocation,
			   bool isAlpha,
			   glm::vec2 pos,
			   glm::vec2 size,
			   glm::vec3 color,
			   float speed,
			   float rotation,
			   bool destroyed);
	~Entity();
};

#endif