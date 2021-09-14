#include "mon_entity.h"

namespace Mon
{

	void initEntity(Entity2D* e, const char* fileLocation, bool isAlpha, v2 position)
	{
		e->sprite = {};
		// TODO(ck): Don't automatically make opengl data... figure out what renderer we are using
		// TODO(ck): Texture load in renderer we need the sprite size from the texture
		MonGL::initRenderData2D(&e->sprite);
		MonTexture::LoadTextureFile(&e->sprite.texture, fileLocation, isAlpha, false, true);

		e->pos = position;
		e->sprite.pos = e->pos;
		// TODO(ck): set in renderer use texture size
		e->sprite.size.x = e->sprite.texture.width;
		e->sprite.size.y = e->sprite.texture.height;

		// TODO(ck): Deal with speed
		e->speed = 500.0f;
		e->maxSpeed = 250.0f;
		
		e->rotation = 0.0f;
		e->destroyed = false;

		e->velocity = v2(1.0f);
		e->weight = 44.0f;
	}

	// TODO(ck): Should this param be pointer?
	// should velocity be acceleratio?
	void movePlayer(Entity2D* p, v2* velocity, float deltaTime)
	{
		if ((velocity->x != 0.0f) && (velocity->y != 0.0f))
		{
			*velocity *= 0.707106781187f;
		}

		*velocity *= p->speed;

		*velocity += -2.5f * p->velocity;

		v2 oldPos = p->pos;
		v2 newPos = oldPos;
		float deltaX = (0.5f * velocity->x * square(deltaTime) + p->velocity.x * deltaTime);
		float deltaY = (0.5f * velocity->y * square(deltaTime) + p->velocity.y * deltaTime);
		v2 delta = { deltaX, deltaY };

		// TODO(ck): need to set an offest like casey does
		newPos += delta;


		p->velocity.x = velocity->x * deltaTime + p->velocity.x;
		p->velocity.y = velocity->y * deltaTime + p->velocity.y;

		p->pos = newPos;

	}



}