#ifndef MON_MATH_H
#define MON_MATH_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

namespace Mon
{
	// TODO(ck): Slowly remove glm
	typedef glm::vec2 v2;
	typedef glm::vec3 v3;
	typedef glm::vec4 v4;

	typedef glm::mat4 mat4;

	struct Rect
	{
		float x, y;
		float w, h;

		float left()	{ return x; }
		float right()	{ return x + w; }
		float top()		{ return y; }
		float bottom()	{ return y + h; }
	};

	// pull out to own file
	struct Point
	{
		int x, y;

	};

	inline float square(float a)
	{
		float result = a * a;
		return result;
	}

	// TODO(ck): Intrinsic
	inline float squareRoot(float real)
	{
		float result = sqrtf(real);
		return result;
	}
	
	inline float inner(v2 a, v2 b)
	{
		float result = a.x * b.x + a.y * b.y;
		return result;
	}

	inline float lengthSq(v2 a)
	{
		float result = inner(a, a);
		return result;
	}



	inline float lerp(float a, float time, float b)
	{
		float result = (1.0f - time) * a + time * b;
		return result;
	}
	
	// TODO(ck): Figure this out is it ease in
	inline float approach(float t, float target, float delta)
	{
		return 1;
		//return t > target ?
	}

	inline int roundReal32ToInt32(float real)
	{
		int result = (int)roundf(real);
		return result;
	}
}

#endif