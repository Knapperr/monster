#ifndef GAME_H
#define GAME_H

#define _3D_

// TODO(ck): Glad, SDL should not be in the game layer
#include <glad/glad.h>
#include <SDL/SDL.h>

#include "mon_debug_camera.h"
#include "mon_ortho_camera.h"
//#include "mon_terrain.h"

// should not be in this layer either
// the app just takes 
#include "mon_terrain.h"
#include "mon_gl_render.h"

#ifndef _3D_
#include "mon_world2D.h"
#else 
#include "mon_world.h"
#endif




namespace Mon
{

#if 1
	// TODO: Complete assertion macro
#define Assert(expression) if(!(expression)) {*(int *)0 = 0;}
#else
#define Assert(expression)
#endif

#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))

	//class WaterObject : public GameObject
	//{
	//public:
	//	float uJump = 0.25f;
	//	float vJump = 0.25f;
	//	float tiling = 1.0f;
	//	float speed = 0.2f;
	//	float flowStrength = 0.07f;
	//	float flowOffset = -0.207f;
	//	float heightScale = 0.1f;
	//	float heightScaleModulated = 9.0f;
	//	float gridResolution = 10.0f;
	//	float tilingModulated = 50.0f;
	//	float waveLength = 12.0f;
	//	bool dualGrid = false;


	//	WaterObject(std::string name, glm::vec3 pos,
	//				glm::vec3 orientation, float scale,
	//				std::string path)
	//		: GameObject(name, pos, orientation, scale, path)
	//	{
	//	}

	//};

	// TODO(ck): Extra data is irrelevant now because of the Shader Configs
	struct extraData
	{
		float uJump = 0.25f;
		float vJump = 0.25f;
		float tiling = 1.0f;
		float speed = 0.2f;
		float flowStrength = 0.07f;
		float flowOffset = -0.207f;
		float heightScale = 0.1f;
		float heightScaleModulated = 9.0f;
		float gridResolution = 10.0f;
		float tilingModulated = 50.0f;
		bool dualGrid = false;

	};

	struct WaterData : extraData
	{
		float waveLength = 12.0f;
	};


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

		bool init(int x);
		void update(double dt, Input* input, int x);
		void render();

		void setViewPort(int width, int height);
		bool playing();

		void playMode();
		void debugMode();


		// TODO(ck):
		// DO NOT KEEP the shaders in the game and renderer like you did last time... 
		// keep them in a structure that can be accessed globally instead keep it clean
#ifndef _3D_
		World2D* world2D;
#endif
		Terrain* terrain;

		Camera* cam;
		OrthoCamera camera;
		Input input;
		
		MonGL::Config* config;

		// TODO(ck): Use one container
		//EntityTwo* entites;
		Entity player;
		Entity water;
		std::vector<Entity> entities;
		std::vector<Entity> enemies;
		bool simulate;
		bool drawCollisions;


		// Note(ck): Current game state - State::Debug is default
		int state; 
		// TODO(ck): Handle Multiple selected IDS i.e: selectedIDs[64]; selectedCount; need to store the count as well
		int selectedIndex;
		int mainShaderID;

		// TODO(ck): 
		// Shader lives in the opengl renderer
		// Need a shader store or something
		MonGL::CommonProgram shader;
		MonGL::WaterDataProgram waterShader;
		MonGL::CommonProgram tileShader;

		double deltaTime;
		MonGL::Light light;

	};
}
#endif