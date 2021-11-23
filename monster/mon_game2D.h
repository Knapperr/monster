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

	class Game2D
	{
	public:
		Input input;

		World2D* world;
		OrthoCamera camera;

		MonGL::Config* config;

		MonGL::CommonProgram shader;
		MonGL::CommonProgram tileShader;

		// Note(ck): Current game state - State::Debug is default
		int state;

		double deltaTime;

		bool init(int x);
		void update(double dt, Input* input, int x);
		void render();

		void setViewPort(int width, int height);
		bool playing();

		void playMode();
		void debugMode();

		void cleanUp();
	};
}
#endif