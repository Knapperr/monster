#include "mon_entity.h"

#include <glad\glad.h>


namespace Mon
{

	Entity2D::Entity2D(const char* fileLocation,
					   bool isAlpha,
					   v2 position)
	{
		sprite = {};
		// TODO(CK): Don't automatically make opengl data... figure out what renderer we are using
		MonGL::initRenderData2D(&sprite);
		MonTexture::LoadTextureFile(&sprite.texture, fileLocation, isAlpha, false);

		pos = position;
		// TODO(ck): Deal with speed
		speed = 100.0f;
		maxSpeed = 250.0f;
		
		rotation = 0.0f;
		destroyed = false;

		velocity = v2(1.0f);
		weight = 44.0f;
	}

	Entity2D::~Entity2D()
	{
		//MonGL::cleanUp(&this->sprite);
	}
}