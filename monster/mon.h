#ifndef MON_H
#define MON_H


// TODO(ck): Glad and SDL should not be in the game layer
#include <glad/glad.h>
#include <SDL/SDL.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "mon_entity.h"
#include "mon_world.h"
#include "mon_shader.h"

#include "mon_debug_camera.h"

// should not be in this layer either
// the app just takes 
#include "mon_gl_render.h"


#if 1
// TODO: Complete assertion macro
#define Assert(expression) if(!(expression)) {*(int *)0 = 0;}
#else
#define Assert(expression)
#endif

#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))

struct Material 
{
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;
};

struct Light 
{
	glm::vec3 pos;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
};

struct CameraTwo
{
	glm::vec2 pos;
	glm::vec2 target;
	float zoom;
};


class Game
{
public:
	bool init();
	void update(double dt, Input* input);
	void render(double dt);
	void cleanUp();

	bool init(int x);
	void update(double dt, Input* input, int x);
	void render();

	// TODO(ck):
	// DO NOT KEEP the shaders in the game and renderer like you did last time...
	// keep them in a structure that can be accessed globally instead keep it clean
	World* world;
	//Shader newShader;
	Camera cam;
	CameraTwo camera;
	Input input;

	unsigned int VBO, VAO = -1;
	glm::vec3 pos;
	Shader shader;

	Light light;
	Material material;

};				 

#endif