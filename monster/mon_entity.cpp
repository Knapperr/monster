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
	MonTexture::LoadTextureFile(&sprite.texture, fileLocation, isAlpha);

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

// This has to come from gl_render
void RenderData(CircleCollider* col)
{

}

void CreateCollider(CircleCollider* col, glm::vec2 pos, glm::vec2 center, float radius)
{
	col->pos = col->center = center;
	col->radius = radius;
}

// Real-Time Collision Detection Christer Ericson 2005. Chapter 4.3 Pg 88
int Intersects(CircleCollider a, CircleCollider b)
{
	glm::vec2 d = a.center - b.center;
	float dist2 = glm::dot(d, d);
	float radiusSum = a.radius + a.radius;
	return dist2 <= radiusSum * radiusSum;

}

void UpdateCollider(CircleCollider* col, glm::vec2 pos)
{
	col->pos = pos;
}