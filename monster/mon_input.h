#ifndef MON_INPUT_H
#define MON_INPUT_H

#include "mon_math.h"

namespace Mon
{
	struct ButtonState
	{
		int halfTransitionCount;
		bool endedDown;
	};

	struct Input
	{
		bool isConnected;
		// maybe make an array for mouse buttons
		ButtonState lMouseBtn;
		ButtonState rMouseBtn;
		float stickAverageX;
		float stickAverageY;
		float rightStickValue;
		uint8_t rightStickAxis;
		float rightStickAngle;
		float mouseXScreen;
		float mouseYScreen;
		v2 mouseOffset;
		v2 stickDir;

		union
		{
			ButtonState buttons[8];
			struct
			{
				ButtonState up;
				ButtonState down;
				ButtonState left;
				ButtonState right;
				ButtonState raise;
				ButtonState lower;
				ButtonState select;
				ButtonState shift;
				ButtonState rightStickUp;
				ButtonState rightStickdown;
				ButtonState rightStickleft;
				ButtonState rightStickright;
				ButtonState quit;
			};
		};
	};
}
#endif