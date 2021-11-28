#include "mon_ortho_camera.h"

namespace Mon
{
	OrthoCamera::OrthoCamera()
	{
		lerpSpeed = 7.0f;
		smoothness = 0.24f;
		pos = v2(0.0f);
		//target = v2(0.0f);
		vel = v2(0.0f);
		zoom = 1.0f;
	}

	OrthoCamera::OrthoCamera(v2 position, Rect* viewPort)
	{
		lerpSpeed = 7.0f;
		smoothness = 0.24f;
		pos = position;
		//target = position;
		vel = v2(1.0f);
		zoom = 1.0f;
		
	}

	void OrthoCamera::update(v2 *target, float dt)
	{
#if 0
		pos.x = target->x;
		pos.y = target->y;
#else 
		pos.x = smoothDamp(pos.x, target->x, vel.x, smoothness, dt);
		pos.y = smoothDamp(pos.y, target->y, vel.y, smoothness, dt);

		if (pos.x < 0.0f)
			pos.x = 0.0f;
		if (pos.y < 0.0f)
			pos.y = 0.0f;
		if (pos.y > 960.0f)
			pos.y = 0.0f;
		if (pos.y < 0.0f)
			pos.y = 0.0f;
#endif
	}

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
		float half = 6.0f;
#if 1
		// TODO(ck): compute in update?
		// TODO(ck): Remove half prefer zoom
		float left = pos.x - width / half;
		float right = pos.x + width / half;
		float top = pos.y - height / half;
		float bottom = pos.y + height / half;
#else
		float left = 0.0f;
		float right = 960.0f;
		float top = 0.0f;
		float bottom = 540.0f;
#endif
		mat4 projection = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
		mat4 zoomMatrix = glm::scale(v3(zoom));
		projection = projection * zoomMatrix;

		return projection;
	}
}