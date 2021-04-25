#ifndef MON_DEBUG_CAMERA_H
#define MON_DEBUG_CAMERA_H

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "mon_input.h"


class Camera
{
public:
	Camera();
	void update(double dt, Input* input, bool constrainPitch = true);
	void getKeyboardInput();
	glm::mat4 viewMatrix();
	glm::vec3 pos;
	float speed;
	float zoom;
	bool disabled;

private:
	glm::vec3 target;
	glm::vec3 direction;
	glm::vec3 right;
	glm::vec3 worldUp;
	glm::vec3 up;
	glm::vec3 front;

	float yaw;
	float pitch;
	float mouseSensitivity;


	void keyInput(double dt, Input* input);
	void mouseInput(float xOffset, float yOffset, bool constrainPitch);

};


#endif 