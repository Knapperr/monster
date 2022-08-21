#include "mon_ortho_camera.h"

namespace Mon
{
	OrthoCamera::OrthoCamera()
	{
		lerpSpeed = 7.0f;
		smoothness = 0.24f;
		pos = v2(0.0f);
		offset = v2(0.0f);
		//target = v2(0.0f);
		vel = v2(0.0f);
		zoom = 64.0f;
	}

	OrthoCamera::OrthoCamera(v2 position, Rect* viewPort)
	{
		lerpSpeed = 7.0f;
		smoothness = 0.24f;
		pos = position;
		offset = v2(0.0f);
		vel = v2(1.0f);
		zoom = 64.0f;
		
	}

	void OrthoCamera::update(v2 *target, float dt)
	{
#if 0
		pos.x = (target->x + offset.x) * 0.5f;
		pos.y = (target->y + offset.y) * 0.5f;
#else 
		pos.x = smoothDamp(pos.x, (target->x + offset.x)*0.5f, vel.x, smoothness, dt);
		pos.y = smoothDamp(pos.y, (target->y + offset.y)*0.5f, vel.y, smoothness, dt);
#endif

		//float mapOffset = 40.0f / 2.0f;
		//if (pos.x < -((mapOffset / 2.0f)) + 2.0f)
		//	pos.x = -(mapOffset / 2.0f) + 2.0f;
		//if (pos.x > (mapOffset / 2.0f) - 2.0f)
		//	pos.x = (mapOffset / 2.0f) - 2.0f;

		//if (pos.y < -((mapOffset / 2.0f)) + 2.0f)
		//	pos.y = -(mapOffset / 2.0f) + 2.0f;
		//if (pos.y > (mapOffset / 2.0f) - 2.0f)
		//	pos.y = (mapOffset / 2.0f) - 2.0f;
	}

	mat4 OrthoCamera::projectionMatrix()
	{
		float width = 960.0f;
		float height = 540.0f;
		float aspect = width / height;
		float half_height = height / 2.0f; // ortho size
		float half_width = half_height * aspect;

		float left = -half_width;
		float right = half_width;
		float top = half_height;
		float bottom = -half_height;
		
		mat4 projection = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);		
		mat4 zoomMatrix = glm::scale(v3(zoom));
		projection = projection * zoomMatrix;

		return projection;
	}

	mat4 OrthoCamera::viewMatrix()
	{
		mat4 view = mat4(1.0f);		
		v3 cameraFront = v3(0.0f, 0.0f, -1.0f);
		v3 cameraUp = v3(0.0f, 1.0f, 0.0f);
		v3 targetPos = v3(pos, 0.0f);

		view = glm::lookAt(targetPos, cameraFront + targetPos, cameraUp);
		return view;
	}
}