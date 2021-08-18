#ifndef MON_DEBUG_CAMERA_H
#define MON_DEBUG_CAMERA_H

#include "mon_input.h"

namespace Mon
{
	class Camera
	{
	public:
		Camera();
		void update(double dt, Input* input, v3 pos, v3 orientation, bool constrainPitch = true);
		void move(v3 tPos, v3 tOrientation);
		void getKeyboardInput();
		void followOn();
		void followOff();
		mat4 viewMatrix();
		mat4 debugViewMatrix();
		mat4 followViewMatrix();
		v3 pos;
		v3 front;
		v3 right;
		float speed;
		float zoom;
		bool follow;

		float nearPlane;
		float farPlane;

		float yaw;
		float pitch;

	private:
		v3 target;
		v3 direction;
		v3 worldUp;
		v3 up;

		float mouseSensitivity;
		float distanceFromTarget;
		float angleAroundTarget;

		void keyInput(double dt, Input* input);
		void mouseInput(v2 offset, bool constrainPitch);
		void calculateCameraVectors();
		void calculateAngleAroundTarget(v2 offset);
	};
}

#endif 