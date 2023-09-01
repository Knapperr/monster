#ifndef _MON_GAME2D_H
#define _MON_GAME2D_H

#include <glad/glad.h>
#include <SDL/SDL.h>

#include "mon_ortho_camera.h"
#include "mon_world2D.h"

namespace Mon {

	struct game_memory
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

	enum State
	{
		Debug,
		Play
	};

	struct DebugIcon 
	{
		v3 pos;
		v3 color;
		v3 textureOffset;
	};

	struct DebugIcons
	{
		const char* layerName;
		int batchIndex;
		DebugIcon icons[128];
	};

	struct Game2D
	{
		World2D* world;

		Input input;
		
		Camera2D cameras[5];
		int currentCameraIndex;
		int cameraCount;

		MonGL::Config* config;
		MonGL::OpenGL* renderer;

		// NOTE(ck): Current game state - State::Debug is default
		int state;
	};

	bool Init(Game2D* game);
	void Update(Game2D* game, double dt, Input* input);
	void Render(Game2D* game, double dt);

	void SetViewPort(MonGL::Config* config, int width, int height);
	bool Playing(Game2D* game);

	void PlayMode(Game2D* game);
	void DebugMode(Game2D* game);

	void CleanUp(Game2D* game);

	// TODO(ck): Camera manager
	static unsigned int AddCamera(Game2D* state)
	{
		unsigned int cameraIndex = state->cameraCount++;

		Camera2D* c = &state->cameras[cameraIndex];
		c = {};

		return cameraIndex;
	}

	// TODO(ck): Camera manager
	static Camera2D* GetCamera(Game2D* state, unsigned int index)
	{
		Camera2D* c = 0;
		if ((index > 0) && (index < ArrayCount(state->cameras)))
		{
			c = &state->cameras[index];
		}
		return c;
	}
}
#endif