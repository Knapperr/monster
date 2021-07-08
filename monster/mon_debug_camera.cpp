#include "mon_debug_camera.h"
#include <iostream>

namespace Mon
{

	Camera::Camera()
	{
		worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

		pos = glm::vec3(13.0f, 13.0f, 40.0f);
		front = glm::vec3(0.0f, 0.0f, -1.0f);

		//direction = glm::normalize(pos - target);
		//right = glm::vec3(1.0f);
		//up = glm::cross(direction, right);
		speed = 30.0f;
		disabled = false;
		yaw = -90.0f;
		pitch = -18.0f;
		zoom = 45.0f;
		mouseSensitivity = 0.1f;

		nearPlane = 0.1f;
		farPlane = 1000.0f;

		calculateCameraVectors();
	}

	glm::mat4 Camera::viewMatrix()
	{
		return glm::lookAt(pos, pos + front, up);
	}

	void Camera::update(double dt, Input* input, bool constrainPitch)
	{
		if (disabled == false)
		{
			keyInput(dt, input);
		}

		if (input->leftMouseButton.endedDown)
		{
			mouseInput(input->mouseXOffset, input->mouseYOffset, constrainPitch);
			calculateCameraVectors();

		}
	}

	void Camera::calculateCameraVectors()
	{
		glm::vec3 frontResult;
		frontResult.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		frontResult.y = sin(glm::radians(pitch));
		frontResult.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		front = glm::normalize(frontResult);

		right = glm::normalize(glm::cross(front, worldUp));
		up = glm::normalize(glm::cross(right, front));
	}


	void Camera::mouseInput(float xOffset, float yOffset, bool constrainPitch)
	{
		xOffset *= mouseSensitivity;
		yOffset *= mouseSensitivity;

		yaw += xOffset;
		pitch += yOffset;

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
}