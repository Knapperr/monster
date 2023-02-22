#include "mon_ortho_camera.h"

namespace Mon
{

	void InitCamera(Camera2D* camera)
	{
		camera->pos = v2(1.0f);
		camera->vel = v2(0.0f);
		camera->offset = v2(0.0f);

		camera->smoothness = 0.10f;
		camera->lerpSpeed = 7.0f;
		camera->zoom = 64.0f;
	}

	void Update(Camera2D* camera, v2* target, float dt)
	{
#if 0
		camera->pos.x = (camera->pos.x - target->x);
		camera->pos.y = (camera->pos.y - target->y);
#else 
		// need offset targetPos - offset
		v2 targetPos = *target;
		camera->pos.x = smoothDamp(camera->pos.x, (targetPos.x), camera->vel.x, camera->smoothness, dt);
		camera->pos.y = smoothDamp(camera->pos.y, (targetPos.y), camera->vel.y, camera->smoothness, dt);
#endif
	}

	mat4 Projection(Camera2D* camera, Rect viewPort)
	{
		float aspect = viewPort.w / viewPort.h;
		float half_height = viewPort.h / 2.0f; // ortho size
		float half_width = half_height * aspect;

		float left = -half_width;
		float right = half_width;
		float top = half_height;
		float bottom = -half_height;

		mat4 projection = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
		return projection;
	}


	mat4 ViewMatrix(Camera2D* camera)
	{
		mat4 view = mat4(1.0f);
		v3 cameraFront = v3(0.0f, 0.0f, -1.0f);
		v3 cameraUp = v3(0.0f, 1.0f, 0.0f);
		v3 targetPos = v3(camera->pos, 0.0f);

		view = glm::lookAt(targetPos, cameraFront + targetPos, cameraUp);
		return view;
	}
}