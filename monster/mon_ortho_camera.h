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

		float zoom;
		float smoothness;
		float lerpSpeed;
		float pixelsPerMeter;
	};

	void InitCamera(Camera2D* camera);
	void Update(Camera2D* camera, v2 target, float dt);
	bool IsInBounds(Camera2D* camera, v2 pos);
	mat4 Projection(Camera2D* camera, Rect viewPort);
	mat4 ViewMatrix(Camera2D* camera);
}
#endif