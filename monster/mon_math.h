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
#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))

	// TODO(ck): Should be in platform layer
	typedef int32_t int32;

	typedef uint32_t uint32;

	// TODO(ck): Slowly remove glm
	typedef glm::vec2 v2;
	typedef glm::vec3 v3;
	typedef glm::vec4 v4;

	typedef glm::mat3 mat3;
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

	inline float lerp(float a, float b, float t)
	{
		float result = (1.0f - t) * a + b * t;
		return result;
	}

	inline float inverse_lerp(float a, float b, float v)
	{
		float result = (v - a) / (b - a);
		return result;
	}

	// smooth damping from Game Engine Gems 4. Critically Damped Ease-In/Ease Out Smoothing - [Thomas Lowe, Krome Studios][chapter 1.10 pg.99]
	// critically damped
	inline float smoothDamp(float from, float to, float &vel, float smoothTime, float time)
	{
		float omega = 2.0f / smoothTime;
		float x = omega * time;
		float exp = 1.0f / (1.0f + x + 0.48f*x*x + 0.235f*x*x*x);

		// adding a maximum smooth speed
		// float maxChange = maxSpeed*smoothTime;
		// float change = min(max(-maxChange, change), maxChange);
		float change = from - to;
		float temp = (vel + omega * change)*time;
		vel = (vel - omega * temp)*exp; // equation 5
		return to + (change + temp)*exp; // equation 4

	}

	inline float sCurve(float a, float time, float b)
	{
		// a = location, b = desired location
		float result = a + (b - a) * 0.1f * time;
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

	inline float absoluteValue(float real)
	{
		float result = (float)fabs(real);
		return result;
	}

}

#endif