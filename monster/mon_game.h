#ifndef GAME_H
#define GAME_H

// TODO(ck): Glad, SDL should not be in the game layer
#include <glad/glad.h>
#include <SDL/SDL.h>

// should not be in this layer either
// the app just takes 
#include "mon_gl_render.h"
#include "mon_world.h"

#include "mon_memory.h"

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

	struct GameMemory
	{
		bool isInitialized;

		uint64_t permanentStorageSize;
		void* permanentStorage; // NOTE: REQUIRED to be cleared to zero at startup

		uint64_t transientStorageSize;
		void* transientStorage; // NOTE: REQUIRED to be cleared to zero at startup

		//debug_platform_read_entire_file* DEBUGPlatformReadEntireFile;
		//debug_platform_free_file_memory* DEBUGPlatformFreeFileMemory;
		//debug_platform_write_entire_file* DEBUGPlatformWriteEntireFile;
	};


	enum Mode
	{
		Debug,
		Play
	};

	struct GameState
	{
		bool initialized;

		//MemoryArena worldArena;
		World* world;
		Grid* grid;
		Grid_* grid_;
		MousePicker picker;
		Input input;

		// TODO(ck): Camera Manager
		Camera cameras[10];
		int currCameraIndex = 0;
		int cameraCount = 0;

		MonGL::Config config;

		bool simulate;
		bool drawCollisions;

		// Note(ck): Current game state - State::Debug is default
		int mode;

		// TEMP(ck): REMOVE THIS IS FOR TESTING GUI
		int selectedSubTextureIndex;

		// TODO(ck): Handle Multiple selected IDS i.e: selectedIDs[64]; selectedCount; need to store the count as well
		int selectedIndex;
		int mainShaderID;
		double deltaTime;

		// This should be a global static struct that gets called outside
		// of the game?
		MonGL::OpenGL* renderer;
		MonGL::RenderSetup setup;

		// TODO(ck): Should this be in here???
		MemoryArena assetArena;
	};

	bool InitGame(GameMemory* memory, int windowWidth, int windowHeight, float portWidth, float portHeight);
	void Update(GameMemory* memory, double dt, Input* newInput);
	void Render(GameMemory* memory, float time, double dt);


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