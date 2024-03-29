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

	struct ScrollWheel
	{
		int x, y;

	};

	struct Input
	{
		bool isConnected;
		bool isAnalog;
		// maybe make an array for mouse buttons
		ButtonState lMouseBtn;
		ButtonState rMouseBtn;
		ButtonState quit;
		ScrollWheel wheel;

		float stickAverageX;
		float stickAverageY;
		float stickAvgRX;
		float stickAvgRY;

		float rightStickValue;
		uint8_t rightStickAxis;
		float rightStickAngle;
		v2 mouseScreen;
		v2 mouseOffset;
		v2 stickDir;
		
		union
		{
			ButtonState buttons[22];
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
				ButtonState space;
				ButtonState rightStickUp;
				ButtonState rightStickdown;
				ButtonState rightStickleft;
				ButtonState rightStickright;
				ButtonState debug;
				ButtonState num1;
				ButtonState num2;
				ButtonState num3;
				ButtonState num4;
				ButtonState r;
				ButtonState f;
				ButtonState rBumper;
				ButtonState lBumper;
			};
		};
	};
}
#endif