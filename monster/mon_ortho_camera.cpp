#include "mon_ortho_camera.h"

namespace Mon
{

	void InitCamera(Camera2D* camera, Rect viewPort)
	{
		camera->pos = v2(1.0f);
		camera->vel = v2(0.0f);
	
		camera->smoothness = 0.10f;
		camera->zoom = 6.0f;		
		camera->resolution.w = viewPort.w;
		camera->resolution.h = viewPort.h;
		camera->aspect = camera->resolution.w / camera->resolution.h;
	}

	void Update(Camera2D* camera, v2 target, float dt)
	{
		// IMPORTANT(ck): won't work with pixels and screen as 1:1 need to figure out meters to pixels
		
		camera->pos = target;
		// NOTE(ck): Smooth damp can be turned back on once pixel art shader problems are solved.
		//			 I wonder if I can't have a smooth damp camera in pixel art? I guess once the smoothing is figured out that little jitter of 0.10 won't matter.
		//camera->pos.x = smoothDamp(camera->pos.x, (target.x), camera->vel.x, camera->smoothness, dt);
		//camera->pos.y = smoothDamp(camera->pos.y, (target.y), camera->vel.y, camera->smoothness, dt);

		// need the map bounds in world space
		// camera position 
		// 40x40.. 
	/*	if (camera->pos.x > 40.0f)
			camera->pos.x = 40.0f;
		if (camera->pos.y > 40.0f)
			camera->pos.y = 40.0f;

		if (camera->pos.x < 0.0f)
			camera->pos.x = 0.0f;
		if (camera->pos.y < 0.0f)
			camera->pos.y = 0.0f;*/
	}

	bool IsInBounds(Camera2D* camera, v2 pos)
	{
		bool result = false;

		// camera bounds
		// get the width of the camera in world units
		// we know this from 
		// we need to do the width, height and then figure out the position??? ... position is the center
		float cameraW = camera->resolution.w / 16.0f;
		float cameraH = camera->resolution.h / 16.0f;

		return result;
	}

	mat4 Projection(Camera2D* camera)
	{
		return glm::ortho(-camera->aspect * camera->zoom, camera->aspect * camera->zoom, -1.0f * camera->zoom, 1.0f * camera->zoom, -1.0f, 1.0f);
		//return glm::ortho(0.0f, camera->resolution.w, 0.0f, camera->resolution.h, -1.0f, 1.0f);
	}

	mat4 ViewMatrix(Camera2D* camera)
	{
		mat4 view = glm::lookAt(v3(camera->pos, 1.0f), v3(camera->pos, 0.0f), v3(0.0f, 1.0f, 0.0f));
		return view;
	}
}