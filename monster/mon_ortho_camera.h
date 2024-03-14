#ifndef MON_ORTHO_CAMERA
#define MON_ORTHO_CAMERA

#include "mon_math.h"

namespace Mon
{
	struct Camera2D
	{
		v2 vel;
		v2 pos;
		v2 offset;
		Rect screen; // not sure what this is
		Rect resolution;

		float zoom;
		float smoothness;
		float lerpSpeed;
		float pixelsPerMeter;
	};

	void InitCamera(Camera2D* camera, Rect viewPort);
	void Update(Camera2D* camera, v2 target, float dt);
	bool IsInBounds(Camera2D* camera, v2 pos);
	mat4 Projection(Camera2D* camera);
	mat4 ViewMatrix(Camera2D* camera);
	v3 CameraTranslation(Camera2D* camera);
}
#endif