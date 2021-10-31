#ifndef MON_DEBUG_CAMERA_H
#define MON_DEBUG_CAMERA_H

#include "mon_input.h"

namespace Mon
{
	class Camera
	{
	public:
		Camera();
		Camera(Rect viewPort);
		void update(double dt, Input* input, v3 pos, v3 orientation, bool constrainPitch = true);
		void move(v3 tPos, v3 tOrientation, float dt);
		void followOn();
		void followOff();
		mat4 projection();
		mat4 viewMatrix();
		mat4 debugViewMatrix();
		mat4 followViewMatrix();
		v3 pos;
		v3 vel;
		v3 front;
		v3 right;
		float speed;
		float zoom;

		float nearPlane;
		float farPlane;

		float yaw;
		float pitch;
		float angleAroundTarget;
		float lerpSpeed;
		float smoothness;
		float aspectRatio;

		v3 lastDebugPos;
		v3 lastDebugFront;
		float lastDebugPitch;
		float lastDebugYaw;

		bool follow;

	private:
		v3 target;
		v3 direction;
		v3 worldUp;
		v3 up;

		float distanceFromTarget;

		void processInput(double dt, Input* input);
		void processScroll(int yOffset);
		void calculateYawPitch(v2 offset, float sensitivity, bool constrainPitch);
		void calculateCameraVectors();
		void calculateAngleAroundTarget(v2 offset);
	};
}

#endif 