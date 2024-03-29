#ifndef MON_CAMERA_H
#define MON_CAMERA_H

#include "mon_input.h"

namespace Mon
{

	enum class CameraType
	{
		Fly,
		Follow,
		Ortho
	};

	struct Camera
	{
		v3 pos;
		v3 front;
		v3 right;
		v3 direction;
		v3 worldUp;
		v3 up;
		float speed;
		float FOV;
		float nearPlane;
		float farPlane;
		float yaw;
		float pitch;
		float aspectRatio;

		// Follow attributes
		v3 target;
		v3 velocity;
		float lerpSpeed;
		float smoothness;
		float distanceFromTarget;
		float angleAroundTarget;
		float offsetZ;

		const char* name;
		CameraType type;
	};
	
	void InitCamera(Camera* camera, CameraType type, const char* name, Rect viewPort);
	void SetCamera(Camera* camera, CameraType type, float FOV, float pitch, float yaw, float distanceFromTarget, float angleAroundTarget, float smoothness, float lerpSpeed, float offsetZ);
	mat4 ViewMatrix(Camera* camera);
	void Update(Camera* camera, double dt, Input* input, v3 pos, v3 orientation, bool constrainPitch = true);

	void InitFlyCamera(Camera* camera, Rect viewPort);
	void UpdateFlyCamera(Camera* camera, double dt, Input* input, bool constrainPitch = true);
	void ProcessInput(Camera* camera, double dt, Input* input);
	void ProcessScroll(Camera* camera, int yOffset);
	mat4 Projection(Camera* camera);
	void CalculateYawPitch(Camera* camera, v2 offset, float sensitivity, bool constrainPitch);
	void CalculateCameraVectors(Camera* camera);
	mat4 FlyViewMatrix(Camera* camera);
	
	void InitFollowCamera(Camera* camera, Rect viewPort);
	mat4 FollowViewMatrix(Camera* camera);
	void UpdateFollowCamera(Camera* camera, double dt, Input* input, v3 pos, v3 orientation, bool constrainPitch = true);
	void CalculateAngleAroundTarget(Camera* camera, v2 offset);
	
	void InitOrthoCamera(Camera* camera);
	mat4 OrthoProjectionMatrix(Camera* camera);
	mat4 OrthoViewMatrix(Camera* camera);
	void UpdateOrthoCamera(Camera* camera, float dt, v3 pos);
}

#endif // MON_CAMERA_H