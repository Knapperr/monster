#include "mon_debug_camera.h"
#include <iostream>

namespace Mon
{

	Camera::Camera()
	{
		worldUp = v3(0.0f, 1.0f, 0.0f);

		pos = v3(1.0f);
		front = v3(0.0f, 0.0f, -1.0f);

		//direction = glm::normalize(pos - target);
		//right = v3(1.0f);
		//up = glm::cross(direction, right);
		speed = 30.0f;
		yaw = 0.0f;
		pitch = 60.0f;
		zoom = 45.0f;
		mouseSensitivity = 0.1f;

		nearPlane = 0.1f;
		farPlane = 1000.0f;

		distanceFromTarget = 10.0f;
		angleAroundTarget = 180.0f;
		follow = false;

		calculateCameraVectors();
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

	void Camera::move(v3 tPos, v3 tOrientation)
	{
		// these are being done every frame.....
		// calczoom 
		// calcpitch
		// calculateanglearoundplayer
		// ---------------

		float offsety = 2.0f;

		float horizontalDistance = (float)(distanceFromTarget * cosf(glm::radians(pitch)));
		float verticalDistance = (float)(distanceFromTarget * sinf(glm::radians(pitch)));
		float theta = glm::radians(tOrientation.y) + angleAroundTarget;
		float offsetx = horizontalDistance * sinf(glm::radians(theta));
		float offsetz = horizontalDistance * cosf(glm::radians(theta));
		pos.x = tPos.x - offsetx;
		pos.z = tPos.z - offsetz;
		pos.y = (tPos.y + verticalDistance) + offsety; // need some kind of offset for the target so the camera doesn't point at the floot
		yaw = 180 - (glm::radians(tOrientation.y) + angleAroundTarget);
	}

	void Camera::update(double dt, Input* input, v3 pos, v3 orientation, bool constrainPitch)
	{
		if (follow)
		{
			move(pos, orientation);
		}
		else
		{
			keyInput(dt, input);
			if (input->leftMouseButton.endedDown)
			{
				mouseInput(input->mouseOffset, constrainPitch);
				// TODO(ck): joystickInput
				//mouseInput(input->stickDir, constrainPitch;
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


	void Camera::mouseInput(v2 offset, bool constrainPitch)
	{
		offset.x *= mouseSensitivity;
		offset.y *= mouseSensitivity;


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

	void Camera::keyInput(double dt, Input* input)
	{
		if (input->shift.endedDown)
			speed *= 2.0f;

		float velocity = speed * (float)dt;

		if (input->up.endedDown)	pos += front * velocity;
		if (input->down.endedDown)	pos -= front * velocity;
		if (input->left.endedDown)	pos -= right * velocity;
		if (input->right.endedDown)	pos += right * velocity;

		if (input->raise.endedDown)	pos.y += 1.0f * velocity;
		if (input->lower.endedDown)	pos.y -= 1.0f * velocity;

		if (input->shift.endedDown)
			speed /= 2.0f;

	}

	void Camera::followOn()
	{
		follow = true;
		yaw = 0.0f;
		pitch = 60.0f;
		front = v3(0.0f, 0.0f, -1.0f);
	}

	void Camera::followOff()
	{
		follow = false;
	}
}