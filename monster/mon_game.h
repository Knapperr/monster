#ifndef GAME_H
#define GAME_H

// TODO(ck): Glad, SDL should not be in the game layer
#include <glad/glad.h>
#include <SDL/SDL.h>

#include "mon_camera.h"
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

	enum State
	{
		Debug,
		Play
	};

	class Game
	{
	public:
		bool init(int windowWidth, int windowHeight);
		void movePlayer(v3* velocity);
		void update(double dt, Input* newInput);
		void render(double dt);
		void cleanUp();

		void setViewPort(int width, int height);
		bool playing();
		void playMode();
		void debugMode();

		World* world;
		Terrain* terrain;

		Input input;
		
		Camera cameras[5];
		int currCameraIndex = 0;
		int cameraCount = 0;

		MonGL::Config* config;

		// TODO(ck): Use one container
		//Entity player;
		//Entity water;
		//std::vector<Entity> enemies;
		bool simulate;
		bool drawCollisions;

		// Note(ck): Current game state - State::Debug is default
		int state; 
		// TODO(ck): Handle Multiple selected IDS i.e: selectedIDs[64]; selectedCount; need to store the count as well
		int selectedIndex;
		int mainShaderID;
		double deltaTime;

		// TODO(ck): 
		// Shader lives in the opengl renderer
		// Need a shader store or something
		MonGL::CommonProgram shader;
		MonGL::WaterDataProgram waterShader;
		MonGL::CommonProgram tileShader;
		MonGL::Light light;
	};
	
	static unsigned int addCamera(Game* game)
	{
		unsigned int cameraIndex = game->cameraCount++;

		Camera* c = &game->cameras[game->cameraCount];
		c = {};

		return cameraIndex;
	}

	static Camera* getCamera(Game* game, unsigned int index)
	{
		Camera* c = 0;
		if ((index > 0) && (index < ArrayCount(game->cameras)))
		{
			c = &game->cameras[index];
		}
		return c;
	}
}
#endif