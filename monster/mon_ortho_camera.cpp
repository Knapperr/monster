#include "mon_ortho_camera.h"

namespace Mon
{
	OrthoCamera::OrthoCamera()
	{
		lerpSpeed = 7.0f;
		pos = v2(0.0f, 0.0f);
		target = v2(0.0f,0.0f);
	}

	OrthoCamera::OrthoCamera(v2 position)
	{
		lerpSpeed = 7.0f;
		target = position;
	}


	void OrthoCamera::update(v2 *pos, float dt)
	{
#if 1
		target.x = lerp(target.x, lerpSpeed * dt, pos->x);
		target.y = lerp(target.y, lerpSpeed * dt, pos->y);
#else
		//target.x = pos->x;
		//target.y = pos->y;
#endif
	}

	void OrthoCamera::update(Point* pos, float dt)
	{
#if 0
		target.x = lerp(target.x, lerpSpeed * dt, pos->x);
		target.y = lerp(target.y, lerpSpeed * dt, pos->y);
#else
		//target.x = pos->x;
		//target.y = pos->y;
#endif
	}

	/*
	void update_camera(Camera* c) {
		c->x += (c->target_x - c->x) * 0.12;
		c->y += (c->target_y - c->y) * 0.12;
	}*/

	void OrthoCamera::createOrtho()
	{

	}
}