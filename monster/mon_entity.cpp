#include "mon_entity.h"

#include <glad\glad.h>
#include "mon_gl_render.h"




Entity::Entity(const char* fileLocation,
					   bool isAlpha,
					   glm::vec2 pos,
					   glm::vec2 size,
					   glm::vec3 color,
					   float speed,
					   float rotation,
					   bool destroyed)
{
	Sprite sprite = {};
	// TODO(CK): Don't automatically make opengl data... figure out what renderer we are using
	MonGL::initRenderData(&sprite);
	MonTexture::LoadTextureFile(&sprite.texture, fileLocation, isAlpha, false);

	this->pos = pos;
	this->size = size;
	this->color = color;
	this->speed = speed;
	this->rotation = rotation;
	this->destroyed = destroyed;
	this->sprite = sprite;

	this->velocity = glm::vec2(1.0f);
}

Entity::~Entity()
{
	MonGL::cleanUp(&this->sprite);
}