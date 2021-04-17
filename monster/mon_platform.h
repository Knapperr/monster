#ifndef MON_PLATFORM_H
#define MON_PLATFORM_H

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

	virtual bool init(int SCREEN_WIDTH, int SCREEN_HEIGHT, int PORT_WIDTH, int PORT_HEIGHT) = 0;
	bool cleanUp();
	void setTitle(Config* config, const char* title);
	uint64_t performanceFrequency();
	uint64_t performanceCounter();
	virtual void pollInput(Input* newInput, Input* oldInput)= 0;
	//bool Init (sdl, win32)
	//void PollInput();
	// input 
	// time
	// PlatformWindow window;
	// CleanUp
};

#endif