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
	typedef glm::vec2 v2;
	typedef glm::vec3 v3;
	typedef glm::vec4 v4;

	typedef glm::mat4 mat4;

	struct Rect
	{
		float x, y;
		float w, h;

		float left() { return x; }
		float right() { return x + w; }
		float top() { return y; }
		float bottom() { return y + h; }
	};

	inline float square(float a)
	{
		float result = a * a;
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
	
	//inline float 
	// 
	//inline mat4 translate(mat4& model, v3& pos)
	//{
	//	mat4 result = glm::translate(model, pos);
	//	return result;
	//}

	//inline mat4 rotate(mat4& model, float angle, v3 pos)
	//{
	//	mat4 result = glm::rotate(model, angle, pos);
	//	return result;

	//}
	//
	//inline mat4 scale(mat4& model, v3& scale)
	//{
	//	mat4 result = glm::scale(model, scale);
	//	return result;
	//}

	//inline float radians(float angle)
	//{
	//	float result = glm::radians(angle);
	//	return result;
	//}

}

#endif