#ifndef _MON_GAME2D_H
#define _MON_GAME2D_H

#include <glad/glad.h>
#include <SDL/SDL.h>

#include "mon_input.h"
#include "mon_ortho_camera.h"
#include "mon_gl_render.h"
#include "mon_world2D.h"

namespace Mon {

	enum State
	{
		Debug,
		Play
	};

	struct Game2D
	{
		Input input;
		World2D* world;
		
		OrthoCamera cameras[5];
		int currentCameraIndex;
		int cameraCount;

		MonGL::Config* config;
		MonGL::OpenGL renderer;

		// Note(ck): Current game state - State::Debug is default
		int state;
	};

	bool Init(Game2D* game);
	void Update(Game2D* game, double dt, Input* input);
	void Render(Game2D* game);

	void SetViewPort(MonGL::Config* config, int width, int height);
	bool Playing(Game2D* game);

	void PlayMode(Game2D* game);
	void DebugMode(Game2D* game);

	void CleanUp(Game2D* game);

	// TODO(ck): Camera manager
	static unsigned int AddCamera(Game2D* state)
	{
		unsigned int cameraIndex = state->cameraCount++;

		OrthoCamera* c = &state->cameras[cameraIndex];
		c = {};

		return cameraIndex;
	}

	// TODO(ck): Camera manager
	static OrthoCamera* GetCamera(Game2D* state, unsigned int index)
	{
		OrthoCamera* c = 0;
		if ((index > 0) && (index < ArrayCount(state->cameras)))
		{
			c = &state->cameras[index];
		}
		return c;
	}
}
#endif