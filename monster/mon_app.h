#ifndef MON_APP_H
#define MON_APP_H

#include "sdl_platform.h"
#include <iostream>

class App
{
public:
	Platform* platform; /* SDLPlatform */
	Game* game;
	bool running = true;
	Input* oldInput;
	Input* newInput;

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