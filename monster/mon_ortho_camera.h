#ifndef MON_ORTHO_CAMERA
#define MON_ORTHO_CAMERA

#include "mon_math.h"

namespace Mon
{
	struct Camera2D
	{
		v2 vel;
		v2 pos;
		Rect resolution;

		float zoom;
		float aspect;
		float smoothness;
	};

	void InitCamera(Camera2D* camera, Rect viewPort);
	void Update(Camera2D* camera, v2 target, float dt);
	bool IsInBounds(Camera2D* camera, v2 pos);
	mat4 Projection(Camera2D* camera);
	mat4 ViewMatrix(Camera2D* camera);
}
#endif