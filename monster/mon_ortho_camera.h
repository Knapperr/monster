#ifndef MON_ORTHO_CAMERA
#define MON_ORTHO_CAMERA

#include "mon_math.h"

namespace Mon
{
	class OrthoCamera
	{
	public:
		v2 pos;
		v2 target;
		float zoom;
		float lerpSpeed;

		OrthoCamera();
		OrthoCamera(v2 position, Rect* viewPort);
		void update(v2* pos, float dt);
		void update(Point* pos, float dt);
		void createOrtho();
		mat4 projectionMatrix();
	};
}
#endif