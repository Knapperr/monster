#ifndef MON_DEBUG_CAMERA_H
#define MON_DEBUG_CAMERA_H

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "mon_input.h"

namespace Mon
{

	class Camera
	{
	public:
		Camera();
		void update(double dt, Input* input, bool constrainPitch = true);
		void getKeyboardInput();
		glm::mat4 viewMatrix();
		glm::vec3 pos;
		glm::vec3 front;
		glm::vec3 right;
		float speed;
		float zoom;
		bool disabled;

		float nearPlane;
		float farPlane;

		float yaw;
		float pitch;

	private:
		glm::vec3 target;
		glm::vec3 direction;
		glm::vec3 worldUp;
		glm::vec3 up;

		float mouseSensitivity;


		void keyInput(double dt, Input* input);
		void mouseInput(glm::vec2 offset, bool constrainPitch);
		void calculateCameraVectors();
	};
}

#endif 