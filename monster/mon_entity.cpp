#include "mon_entity.h"

namespace Mon {

#define real_pow powf
	void RigidBody::integrate(float duration)
	{
		if (inverseMass <= 0.0f)
			return;

		// assert(duration > 0.0);
		/*
			x += vector.x * scale;
			y += vector.y * scale;
			z += vector.z * scale;
		*/
		pos += velocity * duration;

		// update linear pos
		v3 resultingAcc = acceleration;

		// work out the acceleration from the force
		velocity += resultingAcc * duration;

		// impose drag
		velocity *= real_pow(damping, duration);

		// TODO(ck): TEMP FOR TESTING
		if (velocity.x < -15.0f)
			velocity.x = -15.0f;
		if (velocity.x > 15.0f)
			velocity.x = 15.0f;

		if (velocity.z < -15.0f)
			velocity.z = -15.0f;
		if (velocity.z > 15.0f)
			velocity.z = 15.0f;

		clearAccumulator();
	}

	void RigidBody::clearAccumulator()
	{
		return;
	}

}