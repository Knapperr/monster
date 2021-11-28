#ifndef MON_CAMERA_H
#define MON_CAMERA_H

#include "mon_input.h"

namespace Mon
{

	struct Camera
	{
		v3 pos;
		v3 front;
		v3 right;
		v3 direction;
		v3 worldUp;
		v3 up;
		float speed;
		float zoom;
		float nearPlane;
		float farPlane;
		float yaw;
		float pitch;
		float aspectRatio;
	};
	
	/*
	v3 lastDebugPos;
	v3 lastDebugFront;
	float lastDebugPitch;
	float lastDebugYaw;
	*/

	struct FollowCamera : Camera
	{
		v3 target;
		v3 velocity;
		float lerpSpeed;
		float smoothness;
		float distanceFromTarget;
		float angleAroundTarget;
	};

	void InitCamera(Camera* camera, Rect viewPort);
	void Update(Camera* camera, double dt, Input* input, v3 pos, v3 orientation, bool constrainPitch = true);
	void ProcessInput(Camera* camera, double dt, Input* input);
	void ProcessScroll(Camera* camera, int yOffset);
	mat4 Projection(Camera* camera);
	void CalculateYawPitch(Camera* camera, v2 offset, float sensitivity, bool constrainPitch);
	void CalculateCameraVectors(Camera* camera);
	mat4 ViewMatrix(Camera* camera);
	
	void InitFollowCamera(FollowCamera* camera, Rect viewPort);
	mat4 FollowViewMatrix(FollowCamera* camera);
	void Update(FollowCamera* camera, double dt, Input* input, v3 pos, v3 orientation, bool constrainPitch = true);
	void CalculateAngleAroundTarget(FollowCamera* camera, v2 offset);

	void FollowOn(Camera* camera);
	void FollowOff(Camera* camera);
}

#endif // MON_CAMERA_H