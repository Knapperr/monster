#ifndef MON_APP_H
#define MON_APP_H

#include "sdl_platform.h"
#include <iostream>

// TODO(ck): Pull out of class do not want to allocate the App
// Only want to allocate the platform and the game
class App
{
public:
	Settings settings;
	Mon::Platform* platform; /* SDLPlatform */
 
#ifdef _3D_
	Mon::GameMemory memory;
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