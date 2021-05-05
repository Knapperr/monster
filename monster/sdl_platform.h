#ifndef SDL_PLATFORM_H
#define SDL_PLATFORM_H

#include "mon_platform.h"

class SDLPlatform : public Platform
{
public:
	bool init(int SCREEN_WIDTH, int SCREEN_HEIGHT, int PORT_WIDTH, int PORT_HEIGHT);
	void setTitle(Config* config, const char* title);
	void sleep(int milliseconds);
	uint64_t performanceFrequency();
	uint64_t performanceCounter();
	uint64_t ticks();
	void pollInput(Input* newInput, Input* oldInput);
	void processKeyboard(ButtonState* newState, bool isDown);
	void processMouseMotion(Input* newInput, SDL_Event* e, int& lastX, int& lastY);

	void cleanUp();
};

#endif