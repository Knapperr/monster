#ifndef MON_APP_H
#define MON_APP_H

#include "sdl_platform.h"
#include <iostream>

// TODO(ck): include gui instead of sdl_platform

class App
{
public:
	Settings settings;
	Mon::Platform* platform; /* SDLPlatform */
	Mon::Game* game;
	bool running = true;
	Mon::Input* oldInput;
	Mon::Input* newInput;

	bool init();
	void run();
	// Gui stuff in here too
	// Input
	// Running
	// Run
	// Exit
	// Fullscreen
	// updateRate


}; 

#endif