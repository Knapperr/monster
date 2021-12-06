#include "mon_entity.h"

namespace Mon {

	v3 GetSize(Collider* c)
	{
		return v3(c->size.max.x - c->size.min.x, c->size.max.y - c->size.min.y, c->size.max.z - c->size.min.z);
	}

	v3 GetCenter(Collider* c)
	{
		return v3((c->size.min.x + c->size.max.x) / 2, (c->size.min.y + c->size.max.y) / 2, (c->size.min.z + c->size.max.z) / 2);
	}

	// TODO(ck): need an offset from the entity so that the bounding box will properly
	// go around the object 
	mat4 GetTransform(Collider* c, v3 entityPos, v3 entityScale)
	{
		mat4 translateMatrix = translate(mat4(1.0f), entityPos);		
		//mat4 scaleMatrix = scale(mat4(1.0f), entityScale);
		
		return translate(translateMatrix, GetCenter(c)) * scale(mat4(1.0f), GetSize(c));
	}

	void SetTransform(Collider* c, v3 entityPos, v3 entityScale)
	{
		c->data.worldMatrix = GetTransform(c, entityPos, entityScale);
		c->worldPos = c->data.worldMatrix[3];
	}


#define real_pow powf
	void Particle::integrate(float duration)
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

	void Particle::clearAccumulator()
	{
		return;
	}




}