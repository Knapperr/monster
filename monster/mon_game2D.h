#ifndef _MON_GAME2D_H
#define _MON_GAME2D_H

#include <glad/glad.h>
#include <SDL/SDL.h>

#include "mon_ortho_camera.h"
#include "mon_world2D.h"

#include "mon_memory.h"

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
		
		Camera2D camera;

		MonGL::Config* config;
		MonGL::OpenGL* renderer;


		v2 mouseTilePos = {};
		// NOTE(ck): Current game state - State::Debug is default
		int state;
		bool drawDebug = true;
	};

	bool Init(game_memory* memory, int windowWidth, int windowHeight, float portWidth, float portHeight);
	void Update(game_memory* memory, double dt, Input* input);
	void Render(game_memory* memory, double dt);

	void SetViewPort(MonGL::Config* config, int width, int height);
	bool Playing(Game2D* game);

	void PlayMode(Game2D* game);
	void DebugMode(Game2D* game);

	void CleanUp(Game2D* game);
}
#endif