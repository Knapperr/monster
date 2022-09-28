#ifndef MON_PLATFORM_H
#define MON_PLATFORM_H

// IMPORTANT(ck): Defines for 3D game
#define _3D_

// TODO: IMPORTANT(ck): Fix this -- gui and sdl tied together
#include "mon_gui.h"
#include <SDL/SDL_opengl.h>

namespace Mon
{
#define Kilobytes(Value) ((Value)*1024LL)
#define Megabytes(Value) (Kilobytes(Value)*1024LL)
#define Gigabytes(Value) (Megabytes(Value)*1024LL)
#define Terabytes(Value) (Gigabytes(Value)*1024LL)

	struct PlatformState
	{
		uint64_t totalSize;
		void* gameMemoryBlock;
	};

	class Platform
	{
	public:
		// TODO(ck): Need wrappers for these
		SDL_Window* window;
		SDL_GLContext context;
		SDL_Joystick* joyStick;

		PlatformState state;
		bool quit;
		bool cursorDisabled;

		virtual bool init(Settings* settings) = 0;
		virtual void pollInput(Input* newInput, Input* oldInput) = 0;
		virtual void sleep(int milliseconds) = 0;
		virtual uint64_t performanceFrequency() = 0;
		virtual uint64_t performanceCounter() = 0;
		virtual uint64_t ticks() = 0;

		virtual void swapWindow() = 0;
		virtual void setWindowSize(Settings* settings) = 0;
		virtual void cleanUp() = 0;
	};
}
#endif // MON_PLATFORM_H