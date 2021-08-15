#ifndef MON_ENTITY_H
#define MON_ENTITY_H

#include <glm/glm.hpp>
#include "mon_texture.h"
#include <vector>

// TODO(ck): Move to gl_render
struct Sprite
{
	MonTexture::Texture texture;
	unsigned int VAO;
	unsigned int VBO;
};

class Entity
{
public:
	glm::vec2 pos;
	glm::vec2 size;
	glm::vec2 velocity;
	glm::vec3 color;
	float speed;
	float maxSpeed;
	float weight;
	float rotation;
	bool destroyed;

	
	// Components  Sprite can't be part of Default Constructor 
	// default just sets this to nulls
	Sprite sprite;
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