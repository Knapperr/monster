#include "mon_ortho_camera.h"

namespace Mon
{
	OrthoCamera::OrthoCamera()
	{
		lerpSpeed = 7.0f;
		pos = v2(0.0f, 0.0f);
		target = v2(0.0f,0.0f);
		zoom = 1.0f;
	}

	OrthoCamera::OrthoCamera(v2 position, Rect* viewPort)
	{
		lerpSpeed = 7.0f;
		pos = position;
		target = position;
		zoom = 1.0f;
		
	}

	void OrthoCamera::update(v2 *pos, float dt)
	{
#if 0
		target.x = lerp(target.x, lerpSpeed * dt, pos->x);
		target.y = lerp(target.y, lerpSpeed * dt, pos->y);
#else
		target.x = pos->x;
		target.y = pos->y;
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

	mat4 OrthoCamera::projectionMatrix()
	{
		// 640.0f = window.x
		// 360.0f = window.y
		// TODO(ck): my camera isn't deciding the projection that gets sent to the renderer
		// this might be a good thing because i can separate that off here
		float width = 960.0f;
		float height = 540.0f;
#if 1
		// TODO(ck): Remove half prefer zoom
		float half = 6.0f;
		float left = target.x - width / half;
		float right = target.x + width / half;
		float top = target.y - height / half;
		float bottom = target.y + height / half;
#else
		float left = 0.0f;
		float right = 960.0f;
		float top = 0.0f;
		float bottom = 540.0f;
#endif

		mat4 projection = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
		mat4 zoomMatrix = glm::scale(v3(zoom));
		projection *= zoomMatrix;

		return projection;
	}
}