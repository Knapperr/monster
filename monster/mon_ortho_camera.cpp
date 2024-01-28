#include "mon_ortho_camera.h"

namespace Mon
{

	void InitCamera(Camera2D* camera, Rect viewPort)
	{
		camera->pos = v2(1.0f);
		camera->vel = v2(0.0f);
		camera->offset = v2(0.0f);

		camera->smoothness = 0.10f;
		camera->lerpSpeed = 7.0f;
		camera->zoom = 64.0f;
		
		// TODO(ck): Is this the right name? Should it be called camera size? camera viewport size?
		// I guess orthoprojectionsize is the right name
		camera->resolution.w = viewPort.w / 2.0f;
		camera->resolution.h = viewPort.h / 2.0f;
		// BASED OFF OF screen size
		//float metersToPixels = 16.0f / 1.4f;
		//float orthoSize = 480.0f / (2.0f * metersToPixels);
		//// screenSizeInPixels / (orthoSize * 2.0f)
		//camera->pixelsPerMeter = 480.0f / (orthoSize * 2.0f);
		// 16 pixels = 1 meter
		camera->pixelsPerMeter = 16.0f;
	}

	void Update(Camera2D* camera, v2 target, float dt)
	{
		// target position in world space and center of screen
		/*camera->pos.x = (target.x * 16.0f) - camera->resolution.w / 2.0f;
		camera->pos.y = (target.y * 16.0f) - camera->resolution.h / 2.0f;*/
		camera->pos.x = (target.x * 16.0f); // camera pos in world space (target*16)
		camera->pos.y = (target.y * 16.0f);
		// Smooth camera will not work with coordinate system 1:1 pixels and screen space
		//v2 targetPos = target;
		// 
		// IMPORTANT(ck): won't work with pixels and screen as 1:1 need to figure out meters to pixels
		//camera->pos.x = smoothDamp(camera->pos.x, (target.x * 16.0f - (480.0f / 2.0f)), camera->vel.x, camera->smoothness, dt);
		//camera->pos.y = smoothDamp(camera->pos.y, (target.y * 16.0f - (270.0f / 2.0f)), camera->vel.y, camera->smoothness, dt);
	}

	bool IsInBounds(Camera2D* camera, v2 pos)
	{
		bool result = false;


		return result;
	}

	mat4 Projection(Camera2D* camera)
	{
		mat4 projection = glm::ortho(0.0f, camera->resolution.w, 0.0f, camera->resolution.h, -1.0f, 1.0f);
		return projection;
	}

	mat4 ViewMatrix(Camera2D* camera)
	{
		mat4 view = mat4(1.0f);
		v3 cameraPos = v3(camera->pos, 0.0f);
		v3 cameraUp = v3(0.0f, 1.0f, 0.0f);
		//view = glm::translate(view, v3(-cameraPos.x, -cameraPos.y, 0.0f));
		view = glm::translate(view, v3(-(cameraPos.x - camera->resolution.w / 2.0f), -(cameraPos.y - camera->resolution.h / 2.0f), 1.0f));
		//view = glm::lookAt(cameraPos, cameraPos, cameraUp);
		return view;
	}
}