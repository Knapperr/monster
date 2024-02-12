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
		// clamp to world -- tilemap size
		// viewport / 2 --- is how wide and tall our screen is
		// s
		// 16pixels/1 = 16pixels... so maybe our calculations are fine???
		// so i am doing the same thing as handmade by using 16pixels i just dont have to do the conversion because 1=16... 
		//  im skipping it and using the value directly
		//  the only difference is he takes the screen center into account when drawing characters... believe we already do this by
		// using the view matrix against our world position
		
		// so 16 tiles per unit?  39seems to be the end in world space 39*16=624  

		/*
		I think I see what people do now. They write to the buffer in like pixels or whatever so that they dont have to
		multiply the vertices then when they do the read in their second pass they scale it up and make it bigger
		*/

		// IMPORTANT(ck): won't work with pixels and screen as 1:1 need to figure out meters to pixels
		camera->pos.x = smoothDamp(camera->pos.x, (target.x), camera->vel.x, camera->smoothness, dt);
		camera->pos.y = smoothDamp(camera->pos.y, (target.y), camera->vel.y, camera->smoothness, dt);
	}

	bool IsInBounds(Camera2D* camera, v2 pos)
	{
		bool result = false;


		return result;
	}

	mat4 Projection(Camera2D* camera)
	{
		//mat4 projection = glm::ortho(0.0f, camera->resolution.w, 0.0f, camera->resolution.h, -1.0f, 1.0f);

		mat4 projection = glm::ortho(0.0f, camera->resolution.w, 0.0f, camera->resolution.h, -1.0f, 1.0f);
		return projection;
	}

	mat4 ViewMatrix(Camera2D* camera)
	{
		mat4 view = mat4(1.0f);
		float screenCenterX = camera->resolution.w / 2.0f;
		float screenCenterY = camera->resolution.h / 2.0f;
		view = glm::translate(view, v3(screenCenterX - 16.0f*camera->pos.x, screenCenterY - 16.0f*camera->pos.y, 1.0f));
		return view;
	}
}