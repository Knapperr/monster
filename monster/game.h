#ifndef GAME_H
#define GAME_H



// TODO(ck): Glad and SDL should not be in the game layer
#include <glad/glad.h>
#include <SDL/SDL.h>


#include "mon_debug_camera.h"
//#include "mon_terrain.h"

// should not be in this layer either
// the app just takes 
#include "mon_terrain.h"
#include "mon_gl_render.h"
#include "mon_world.h"


namespace Mon
{

#if 1
	// TODO: Complete assertion macro
#define Assert(expression) if(!(expression)) {*(int *)0 = 0;}
#else
#define Assert(expression)
#endif

#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))



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
		v3 pos;
		v3 velocity;
		v3 acceleration;
		v3 orientation;

		Direction dir = Direction::FORWARD;

		float damping;
		float inverseMass;

		void integrate(float duration);
		void clearAccumulator();

		// TODO(ck): TEST REMOVE
		float speed;

	};

	struct Entity
	{
		Particle particle;
		MonGL::RenderData data;
		MonGL::RenderData colliderData;
		
	};

	struct CameraTwo
	{
		v2 pos;
		v2 target;
		float zoom;
		float lerpSpeed;
	};

	enum State
	{
		Debug,
		Play
	};

	class Game
	{
	public:
		bool init();
		void movePlayer(v3* velocity);
		void movePlayer2D(v2* velocity, Entity2D* player);
		void update(double dt, Input* newInput);
		void render(double dt);
		void cleanUp();

		bool init(int x);
		void update(double dt, Input* input, int x);
		void render();

		void fullScreen(int width, int height);
		bool playing();


		// TODO(ck):
		// DO NOT KEEP the shaders in the game and renderer like you did last time... 
		// keep them in a structure that can be accessed globally instead keep it clean
		World* world;
		Terrain* terrain;

		Camera cam;
		CameraTwo camera;
		Input input;
		
		MonGL::Config* config;

		// TODO(ck): Use one container
		//EntityTwo* entites;
		Entity player;
		std::vector<Entity> trees;
		std::vector<Entity> enemies;
		bool simulate;
		bool drawCollisions;


		// Note(ck): Current game state - State::Debug is default
		int state; 

		// TODO(ck): 
		// Shader lives in the opengl renderer
		MonGL::Shader shader;
		MonGL::Shader tileShader;

		double deltaTime;
		MonGL::Light light;

	};
}
#endif