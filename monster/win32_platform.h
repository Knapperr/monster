#ifndef WIN32_PLATFORM_H_
#define WIN32_PLATFORM_H_

#include "mon_platform.h"

namespace Mon
{
	class Win32Platform : public Platform
	{
	public:
		bool init(Settings* settings);
		void setWindowSize(Settings* settings);
		void setTile(Settings* settings, const char* title);
		void sleep(int milliseconds);
		uint64_t performanceFrequency();
		uint64_t performanceCounter();
		uint64_t ticks();
		void pollInput(Mon::Input* newInput, Mon::Input* oldInput);
		void processKeyboard(Mon::ButtonState* newState, bool isDown);
		//void processMouseMotion(Mon::Input* newInput, int& lastX, int& lastY);

		void cleanUp();
	};
}


#endif
