#ifndef GAME_H
#define GAME_H

// TODO(ck): Glad, SDL should not be in the game layer
#include <glad/glad.h>
#include <SDL/SDL.h>

// should not be in this layer either
// the app just takes 
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

	// NOTE(ck): TODO(ck): 
	// Not sure how to design this. Need some kind of debug module that can hook into the game 
	// and then come off for release
	void RunDebugControls(Input* input, MousePicker* picker, World* world, int& selectedIndex);

	enum Mode
	{
		Debug,
		Play
	};

	struct GameState
	{
		World* world;
		Grid* grid;
		MousePicker picker;
		Input input;

		MonGL::Line lineOne;
		MonGL::Line lineTwo;

		// TODO(ck): Camera Manager
		Camera cameras[10];
		int currCameraIndex = 0;
		int cameraCount = 0;

		MonGL::Config* config;

		bool simulate;
		bool drawCollisions;

		// Note(ck): Current game state - State::Debug is default
		int mode;
		// TODO(ck): Handle Multiple selected IDS i.e: selectedIDs[64]; selectedCount; need to store the count as well
		int selectedIndex;
		int mainShaderID;
		double deltaTime;

		// This should be a global static struct that gets called outside
		// of the game?
		MonGL::OpenGL renderer;
		MonGL::RenderSetup setup;
		
		MonGL::Light light;
	};

	bool InitGame(GameState* state, int windowWidth, int windowHeight, float portWidth, float portHeight);
	void Update(GameState* state, double dt, Input* newInput);
	void Render(GameState* state, float time, double dt);
	void CleanUp(GameState* state);
	void SetViewPort(GameState* state, int width, int height);
	bool Playing(GameState* state);
	void PlayMode(GameState* state);
	void DebugMode(GameState* state);

	// TODO(ck): Camera manager
	static unsigned int AddCamera(GameState* state)
	{
		unsigned int cameraIndex = state->cameraCount++;

		Camera* c = &state->cameras[cameraIndex];
		c = {};

		return cameraIndex;
	}

	// TODO(ck): Camera manager
	static Camera* GetCamera(GameState* state, unsigned int index)
	{
		Camera* c = 0;
		if ((index > 0) && (index < ArrayCount(state->cameras)))
		{
			c = &state->cameras[index];
		}
		return c;
	}
}
#endif