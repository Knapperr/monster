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
	
		// target pos in world space
		camera->pos.x = target->x * 16.0f;
		camera->pos.y = target->y * 16.0f;
		// screen space
		// TODO(ck): Width, Height variables
		camera->pos.x = camera->pos.x - (480.0f / 2.0f);
		camera->pos.y = camera->pos.y - (270.0f / 2.0f);

		// Smooth camera will not work with coordinate system 1:1 pixels and screen space
		//v2 targetPos = *target;
		//camera->pos.x = smoothDamp(camera->pos.x, (targetPos.x), camera->vel.x, camera->smoothness, dt);
		//camera->pos.y = smoothDamp(camera->pos.y, (targetPos.y), camera->vel.y, camera->smoothness, dt);

	}

	mat4 Projection(Camera2D* camera, Rect viewPort)
	{
		// TODO(ck): Width, Height variables
		// NOTE(ck): half of the view port width and height 
		mat4 projection = glm::ortho(0.0f, 480.0f, 0.0f, 270.0f, -1.0f, 1.0f);
		return projection;
	}

	mat4 ViewMatrix(Camera2D* camera)
	{
		mat4 view = mat4(1.0f);
		v3 cameraPos = v3(camera->pos, 0.0f);
		view = glm::translate(view, v3(-cameraPos.x, -cameraPos.y, 0.0f));
		//view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		return view;
	}
}