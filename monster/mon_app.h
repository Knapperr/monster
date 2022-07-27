#ifndef MON_APP_H
#define MON_APP_H

#include "sdl_platform.h"
#include <iostream>

class App
{
public:
	Settings settings;
	Mon::Platform* platform; /* SDLPlatform */

#ifdef _3D_
	Mon::GameMemory memory;
	Mon::GameState* gameState;
#else 
	Mon::Game2D* game2D;
#endif
	Mon::Input input[2];
	bool running = true;
	bool showGUI = true;

	bool init();
	void run();
	// Gui stuff in here too
	// Exit
	// Fullscreen
	// updateRate


}; 

#endif