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
		v3 posOffset; // TODO(Ck): Do I need this?

		float zoom;
		float smoothness;
		float lerpSpeed;
	};

	void InitCamera(Camera2D* camera);
	void Update(Camera2D* camera, v2* target, float dt);
	mat4 Projection(Camera2D* camera, Rect viewPort);
	mat4 ViewMatrix(Camera2D* camera);
}
#endif