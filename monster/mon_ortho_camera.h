#ifndef MON_ORTHO_CAMERA
#define MON_ORTHO_CAMERA

#include "mon_math.h"

namespace Mon
{
	class OrthoCamera
	{
	public:
		
		v2 vel;
		v2 pos;
		v2 offset;
		//v2 target;
		float zoom;
		float smoothness;
		float lerpSpeed;
		v3 positionOffset; // TODO(ck): IMPORTANT(ck): IS this what this is?

		OrthoCamera();
		OrthoCamera(v2 position, Rect* viewPort);
		void update(v2* target, float dt);
		mat4 projectionMatrix();
		mat4 viewMatrix();
	};
}
#endif