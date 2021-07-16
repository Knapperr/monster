#ifndef MON_PLATFORM_H
#define MON_PLATFORM_H


#define _3D_

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

	// TODO(ck): Need wrappers for these
	SDL_Window* window;
	SDL_GLContext context;
	SDL_Joystick* joyStick;
	bool quit;
	bool cursorDisabled;

	virtual bool init(int SCREEN_WIDTH, int SCREEN_HEIGHT, int PORT_WIDTH, int PORT_HEIGHT) = 0;
	virtual void pollInput(Input* newInput, Input* oldInput) = 0;
	virtual void sleep(int milliseconds) = 0;
	virtual uint64_t performanceFrequency() = 0;
	virtual uint64_t performanceCounter() = 0;
	virtual uint64_t ticks() = 0;
	virtual void setTitle(Config* config, const char* title) = 0;
	virtual void cleanUp() = 0;
};

#endif