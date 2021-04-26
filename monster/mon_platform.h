#ifndef MON_PLATFORM_H
#define MON_PLATFORM_H

//#define _3D_

struct Context
{

};

struct Config
{
	const char* title;
	Context* context;
	int width;
	int height;


};

// TODO: IMPORTANT(ck): Fix this
#include "mon_gui.h"
#include <SDL/SDL_opengl.h>

class Platform
{
public:

	// TODO(ck): Get these out of this layer by changing the
	// gui layer to SDL_GUI or something pack it in with that
	SDL_Window* window;
	SDL_GLContext context;
	bool quit;
	bool cursorDisabled;

	virtual bool init(int SCREEN_WIDTH, int SCREEN_HEIGHT, int PORT_WIDTH, int PORT_HEIGHT) = 0;
	virtual void pollInput(Input* newInput, Input* oldInput) = 0;
	virtual uint64_t performanceFrequency() = 0;
	virtual uint64_t performanceCounter() = 0;
	virtual void setTitle(Config* config, const char* title) = 0;
	virtual void cleanUp() = 0;
};

#endif