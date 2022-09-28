#include "mon_camera.h"
#include <iostream>

#include "mon_log.h"

namespace Mon
{
	void InitCamera(Camera* camera, CameraType type, const char* name, Rect viewPort)
	{
		camera->name = name;
		camera->type = type;
		switch (type)
		{
		case CameraType::Fly:
			InitFlyCamera(camera, viewPort);
			break;
		case CameraType::Follow:
			InitFollowCamera(camera, viewPort);
			break;
		case CameraType::Ortho:
			InitOrthoCamera(camera);
			break;
		default:
			Mon::Log::print("Camera type not specified. Creating fly camera");
			camera->type = CameraType::Fly;
			InitFlyCamera(camera, viewPort);
			break;
		}
	}

	// TODO(ck): Do not like switch for retrieving view matrix
	// IMPORTANT(ck): Can I use an update and viewmatrix function pointer? just set the func the 
	// camera is going to use?
	// that way can call camera->update(); and it will call the assigned update method
	mat4 ViewMatrix(Camera* camera)
	{
		switch (camera->type)
		{
		case CameraType::Fly:
			return FlyViewMatrix(camera);
		case CameraType::Follow:
			return FollowViewMatrix(camera);
		case CameraType::Ortho:
			return OrthoViewMatrix(camera);
		default:
			return FlyViewMatrix(camera);
		}
	}
	// TODO(ck): Do not like switch for update
	// IMPORTANT(ck): Can I use an update and viewmatrix function pointer? just set the func the 
	// camera is going to use?
	void Update(Camera* camera, double dt, Input* input, v3 pos, v3 orientation, bool constrainPitch)
	{
		switch (camera->type)
		{
		case CameraType::Fly:
			UpdateFlyCamera(camera, dt, input, constrainPitch);
			break;
		case CameraType::Follow:
			UpdateFollowCamera(camera, dt, input, pos, orientation, constrainPitch);
			break;
		case CameraType::Ortho:
			UpdateOrthoCamera(camera, dt, pos);
			break;
		default:
			UpdateFlyCamera(camera, dt, input, constrainPitch);
			break;
		}
	}

	/// 
	/// Standard Fly Camera
	///
	void InitFlyCamera(Camera* camera, Rect viewPort)
	{
		camera->worldUp = v3(0.0f, 1.0f, 0.0f);
		camera->pos = v3(-1.5f, 11.5f, 26.5f);
		camera->front = v3(0.0f, 0.0f, -1.0f);

		camera->speed = 30.0f;
		camera->yaw = -89.0f;
		camera->pitch = -41.0f;
		camera->zoom = 40.0f;
		camera->nearPlane = 0.1f;
		camera->farPlane = 1000.0f;
		camera->aspectRatio = viewPort.w / viewPort.h;

		//direction = glm::normalize(pos - target);
		//right = v3(1.0f);
		//up = glm::cross(direction, right);
		CalculateCameraVectors(camera);
	}

	mat4 FlyViewMatrix(Camera* camera)
	{
		return glm::lookAt(camera->pos, camera->pos + camera->front, camera->up);
	}

	mat4 Projection(Camera* camera)
	{
		return glm::perspective(glm::radians(camera->zoom), camera->aspectRatio, camera->nearPlane, camera->farPlane);
	}

	void UpdateFlyCamera(Camera* camera, double dt, Input* input, bool constrainPitch)
	{
		ProcessInput(camera, dt, input);
		if (input->lMouseBtn.endedDown)
		{
			CalculateYawPitch(camera, input->mouseOffset, 0.10f, constrainPitch);
			CalculateCameraVectors(camera);
		}
	}
	
	void ProcessInput(Camera* camera, double dt, Input* input)
	{
		if (input->shift.endedDown)
			camera->speed *= 2.0f;

		if (input->rMouseBtn.endedDown == false)
			ProcessScroll(camera, input->wheel.y);

		float velocity = camera->speed * (float)dt;
		if (input->lMouseBtn.endedDown && input->rMouseBtn.endedDown)
		{
			camera->pos += camera->front * velocity;
		}

		if (input->isAnalog)
		{
			if (input->stickAverageX >= 0.7f) camera->pos += camera->right * velocity;
			if (input->stickAverageX <= -0.7f) camera->pos -= camera->right * velocity;
			if (input->stickAverageY >= 0.7f) camera->pos -= camera->front * velocity;
			if (input->stickAverageY <= -0.7f) camera->pos += camera->front * velocity;
		}
		else
		{
			if (input->up.endedDown)	camera->pos += camera->front * velocity;
			if (input->down.endedDown)	camera->pos -= camera->front * velocity;
			if (input->left.endedDown)	camera->pos -= camera->right * velocity;
			if (input->right.endedDown)	camera->pos += camera->right * velocity;

			if (input->raise.endedDown || input->rBumper.endedDown)	
				camera->pos.y += 1.0f * velocity;
			if (input->lower.endedDown || input->lBumper.endedDown)
				camera->pos.y -= 1.0f * velocity;

			if (input->shift.endedDown)
				camera->speed /= 2.0f;
		}

		// Right stick
		if (input->stickAvgRX != 0.0f || input->stickAvgRY != 0.0f)
		{
			// NOTE(ck): Flip Y 
			CalculateYawPitch(camera, v2{ input->stickAvgRX, -input->stickAvgRY }, 0.90f, true);
			CalculateCameraVectors(camera);
		}

	}
	
	void ProcessScroll(Camera* camera, int yOffset)
	{
		camera->speed += yOffset * 2;
		if (camera->speed <= 0)
			camera->speed = 0.2;
	}

	void CalculateCameraVectors(Camera* camera)
	{
		v3 frontResult = {};
		frontResult.x = cos(glm::radians(camera->yaw)) * cos(glm::radians(camera->pitch));
		frontResult.y = sin(glm::radians(camera->pitch));
		frontResult.z = sin(glm::radians(camera->yaw)) * cos(glm::radians(camera->pitch));
		camera->front = glm::normalize(frontResult);

		camera->right = glm::normalize(glm::cross(camera->front, camera->worldUp));
		camera->up = glm::normalize(glm::cross(camera->right, camera->front));
	}
	
	void CalculateYawPitch(Camera* camera, v2 offset, float sensitivity, bool constrainPitch)
	{
		offset.x *= sensitivity;
		offset.y *= sensitivity;

		camera->yaw += offset.x;
		camera->pitch += offset.y;

		if (constrainPitch)
		{
			if (camera->pitch > 89.0f)
				camera->pitch = 89.0f;
			if (camera->pitch < -89.0f)
				camera->pitch = -89.0f;
		}
	}

	/// 
	/// Follow 3D Camera 
	///

	void InitFollowCamera(Camera* camera, Rect viewPort)
	{
		InitFlyCamera(camera, viewPort);

		camera->yaw = 0.0f;
		camera->pitch = 50.0f;
		camera->front = v3(0.0f, 0.0f, -1.0f);
		camera->velocity = v3(0.0f);
		camera->distanceFromTarget = 10.0f;
		camera->angleAroundTarget = 180.0f;
		camera->smoothness = 0.15f;
		camera->lerpSpeed = 7.0f;
		camera->zoom = 30.0f;
	}

	mat4 FollowViewMatrix(Camera* camera)
	{
		glm::mat4 viewMatrix = glm::mat4(1.0f);
		viewMatrix = glm::rotate(viewMatrix, glm::radians(camera->pitch), glm::vec3(1, 0, 0));
		viewMatrix = glm::rotate(viewMatrix, glm::radians(camera->yaw), glm::vec3(0, 1, 0));
		glm::vec3 negativeCameraPos = glm::vec3(-camera->pos);
		glm::mat4 translate = glm::translate(glm::mat4(1.0f), negativeCameraPos);
		viewMatrix = viewMatrix * translate;

		return viewMatrix;
	}

	void UpdateFollowCamera(Camera* camera, double dt, Input* input, v3 pos, v3 orientation, bool constrainPitch)
	{
		// these are being done every frame.....
		// calczoom 
		// calcpitch
		// calculateanglearoundplayer
		// ---------------
		float horizontalDistance = (float)(camera->distanceFromTarget * cosf(glm::radians(camera->pitch)));
		float verticalDistance = (float)(camera->distanceFromTarget * sinf(glm::radians(camera->pitch)));
		float theta = glm::radians(orientation.y) + camera->angleAroundTarget;
		float offsetx = horizontalDistance * sinf(glm::radians(theta));
		float offsetz = horizontalDistance * cosf(glm::radians(theta));
		float offsety = 2.0f;

		// NOTE(ck): Smooth spring dampening
		camera->pos.x = smoothDamp(camera->pos.x, pos.x - offsetx, camera->velocity.x, camera->smoothness, dt);
		camera->pos.z = smoothDamp(camera->pos.z, pos.z - offsetz, camera->velocity.z, camera->smoothness, dt);
		camera->pos.y = smoothDamp(camera->pos.y, (pos.y + verticalDistance) + offsety, camera->velocity.y, camera->smoothness, dt);

		//pos.x = tPos.x - offsetx;
		//pos.z = tPos.z - offsetz;
		//pos.y = (tPos.y + verticalDistance) + offsety;

		camera->yaw = 180 - (glm::radians(orientation.y) + camera->angleAroundTarget);
	}

	void CalculateAngleAroundTarget(Camera* camera, v2 offset)
	{
		//if (g_Game->leftMousePressed)
		//{
			float angle = offset.x * 0.3f; // sensitivity
			camera->angleAroundTarget -= angle;
		//}
	}

	//
	// Orthographic Camera
	//
	void InitOrthoCamera(Camera* camera)
	{
		camera->lerpSpeed = 7.0f;
		camera->smoothness = 0.24f;
		camera->pos = v3(100.0f);
		camera->velocity = v3(1.0f);
		camera->zoom = 1.0f;
	}

	mat4 OrthoProjectionMatrix(Camera* camera)
	{
		// 640.0f = window.x
	// 360.0f = window.y
		float width = 1060.0f;
		float height = 1540.0f;
		float half = 6.0f;
#if 1
		float left = camera->pos.x - width / half;
		float right = camera->pos.x + width / half;
		float top = camera->pos.y - height / half;
		float bottom = camera->pos.y + height / half;
#else
		float left = 0.0f;
		float right = 960.0f;
		float top = 0.0f;
		float bottom = 540.0f;
#endif
		mat4 projection = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
		mat4 zoomMatrix = glm::scale(v3(camera->zoom));
		projection = projection * zoomMatrix;

		return projection;
	}

	mat4 OrthoViewMatrix(Camera* camera)
	{
		return mat4(1.0f);
	}

	void UpdateOrthoCamera(Camera* camera, float dt, v3 pos)
	{
		camera->pos = pos;
	}

}