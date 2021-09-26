#include "mon_debug_camera.h"
#include <iostream>

namespace Mon
{
	Camera::Camera()
	{
		worldUp = v3(0.0f, 1.0f, 0.0f);

		pos = v3(29.0f, 17.0f, 46.0f);
		front = v3(0.0f, 0.0f, -1.0f);

		//direction = glm::normalize(pos - target);
		//right = v3(1.0f);
		//up = glm::cross(direction, right);
		speed = 30.0f;
		yaw = 0.0f;
		pitch = -34.0f;
		zoom = 40.0f;

		nearPlane = 0.1f;
		farPlane = 1000.0f;

		distanceFromTarget = 10.0f;
		angleAroundTarget = 180.0f;
		lerpSpeed = 7.0f;
		follow = false;

		calculateCameraVectors();
	}

	mat4 Camera::projection()
	{
		return glm::perspective(glm::radians(zoom), 960.0f / 540.0f, nearPlane, farPlane);
	}

	mat4 Camera::viewMatrix()
	{
		if (follow)
			return followViewMatrix();
		else
			return debugViewMatrix();
	}

	mat4 Camera::debugViewMatrix()
	{
		return glm::lookAt(pos, pos + front, up);
	}

	mat4 Camera::followViewMatrix()
	{
		glm::mat4 viewMatrix = glm::mat4(1.0f);
		viewMatrix = glm::rotate(viewMatrix, glm::radians(pitch), glm::vec3(1, 0, 0));
		viewMatrix = glm::rotate(viewMatrix, glm::radians(yaw), glm::vec3(0, 1, 0));
		glm::vec3 cameraPos = pos;
		glm::vec3 negativeCameraPos = glm::vec3(-cameraPos.x, -cameraPos.y, -cameraPos.z);
		glm::mat4 translate = glm::translate(glm::mat4(1.0f), negativeCameraPos);
		viewMatrix = viewMatrix * translate;

		return viewMatrix;
	}

	void Camera::move(v3 tPos, v3 tOrientation, float dt)
	{
		// these are being done every frame.....
		// calczoom 
		// calcpitch
		// calculateanglearoundplayer
		// ---------------

		// TODO(ck): Do not use lerp - figure out real camera following ease in ease out techniques

		float offsety = 2.0f;

		float horizontalDistance = (float)(distanceFromTarget * cosf(glm::radians(pitch)));
		float verticalDistance = (float)(distanceFromTarget * sinf(glm::radians(pitch)));
		float theta = glm::radians(tOrientation.y) + angleAroundTarget;
		float offsetx = horizontalDistance * sinf(glm::radians(theta));
		float offsetz = horizontalDistance * cosf(glm::radians(theta));
		
		// NOTE(ck): Lerp for now
		pos.x = lerp(pos.x, lerpSpeed * dt, tPos.x - offsetx);
		pos.z = lerp(pos.z, lerpSpeed * dt, tPos.z - offsetz);
		pos.y = lerp(pos.y, lerpSpeed * dt, (tPos.y + verticalDistance) + offsety); // need some kind of offset for the target so the camera doesn't point at the floor
		yaw = 180 - (glm::radians(tOrientation.y) + angleAroundTarget);
	}

	void Camera::update(double dt, Input* input, v3 pos, v3 orientation, bool constrainPitch)
	{
		if (follow)
		{
			move(pos, orientation, dt);
		}
		else
		{
			processInput(dt, input);
			if (input->lMouseBtn.endedDown)
			{
				calculateYawPitch(input->mouseOffset, 0.10f, constrainPitch);
				calculateCameraVectors();
			}
		}
	}

	void Camera::calculateAngleAroundTarget(v2 offset)
	{
		//if (g_Game->leftMousePressed)
		//{
		float angle = offset.x * 0.3f; // sensitivity
		this->angleAroundTarget -= angle;
		//}
	}

	void Camera::calculateCameraVectors()
	{
		v3 frontResult = {};
		frontResult.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		frontResult.y = sin(glm::radians(pitch));
		frontResult.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		front = glm::normalize(frontResult);

		right = glm::normalize(glm::cross(front, worldUp));
		up = glm::normalize(glm::cross(right, front));
	}

	void Camera::calculateYawPitch(v2 offset, float sensitivity, bool constrainPitch)
	{
		offset.x *= sensitivity;
		offset.y *= sensitivity;

		yaw += offset.x;
		pitch += offset.y;

		if (constrainPitch)
		{
			if (pitch > 89.0f)
				pitch = 89.0f;
			if (pitch < -89.0f)
				pitch = -89.0f;
		}
	}

	void Camera::processInput(double dt, Input* input)
	{
		if (input->shift.endedDown)
			speed *= 2.0f;

		processScroll(input->wheel.y);

		float velocity = speed * (float)dt;

		if (input->rMouseBtn.endedDown) {
			pos += front * velocity;
		}

		if (input->isAnalog)
		{
			if (input->stickAverageX >= 0.7f) pos += right * velocity;
			if (input->stickAverageX <= -0.7f) pos -= right * velocity;
			if (input->stickAverageY >= 0.7f) pos -= front * velocity;
			if (input->stickAverageY <= -0.7f) pos += front * velocity;
		}
		else
		{
			if (input->up.endedDown)	pos += front * velocity;
			if (input->down.endedDown)	pos -= front * velocity;
			if (input->left.endedDown)	pos -= right * velocity;
			if (input->right.endedDown)	pos += right * velocity;

			if (input->raise.endedDown)	pos.y += 1.0f * velocity;
			if (input->lower.endedDown)	pos.y -= 1.0f * velocity;

			if (input->shift.endedDown)
				speed /= 2.0f;
		}

		// Right stick
		if (input->stickAvgRX != 0.0f || input->stickAvgRY)
		{
			// NOTE(ck): Flip Y 
			calculateYawPitch(v2{ input->stickAvgRX, -input->stickAvgRY }, 0.90f, true);
			calculateCameraVectors();
		}
	}

	void Camera::processScroll(int yOffset)
	{
		if (speed >= 0)
			speed += yOffset * 2;
		else
			speed = 1;
	}

	void Camera::followOn()
	{
		follow = true;
		lastDebugPos = pos;
		lastDebugYaw = yaw;
		lastDebugPitch = pitch;
		lastDebugFront = front;

		yaw = 0.0f;
		pitch = 40.0f;
		front = v3(0.0f, 0.0f, -1.0f);

		
	}

	void Camera::followOff()
	{
		follow = false;
		yaw = lastDebugYaw;
		pitch = lastDebugPitch;
		front = lastDebugFront;
		pos = lastDebugPos;
	}
}