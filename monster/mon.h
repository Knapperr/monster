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

#include "mon_debug_camera.h"
#include "mon_terrain.h"

// should not be in this layer either
// the app just takes 
#include "mon_gl_render.h"

namespace Mon
{

#if 1
	// TODO: Complete assertion macro
#define Assert(expression) if(!(expression)) {*(int *)0 = 0;}
#else
#define Assert(expression)
#endif

#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))

	struct Light
	{
		glm::vec3 pos;

		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
	};


	// Use this for now
	enum class Direction
	{
		LEFT,
		RIGHT,
		FORWARD,
		BACKWARD
	};

	// Get to this starts pg49 Ian Millington
	struct Particle
	{
		glm::vec3 pos;
		glm::vec3 velocity;
		glm::vec3 acceleration;

		Direction dir = Direction::FORWARD;

		float damping;
		float inverseMass;

		void integrate(float duration);
		void clearAccumulator();
	};

	struct EntityTwo
	{
		Particle particle;
		MonGL::RenderData data;
		MonGL::RenderData colliderData;
		
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
		void update(double dt, Input* newInput);
		void render(double dt);
		void cleanUp();

		bool init(int x);
		void update(double dt, Input* input, int x);
		void render();


		// TODO(ck):
		// DO NOT KEEP the shaders in the game and renderer like you did last time...
		// keep them in a structure that can be accessed globally instead keep it clean
		World* world;
		Terrain* terrain;

		Camera cam;
		CameraTwo camera;
		Input input;
		
		EntityTwo player;
		std::vector<EntityTwo> enemies;
		bool simulate;

		// TODO(ck): 
		// Shader lives in the opengl renderer
		MonShader::Shader shader;
		MonShader::Shader tileShader;

		double deltaTime;
		Light light;


		unsigned int testVAO;
		unsigned int testVBO;
		unsigned int testEBO;
		unsigned int testTextureU;
		

	};
}
#endif