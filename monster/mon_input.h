#ifndef MON_INPUT_H
#define MON_INPUT_H

struct ButtonState
{
	int halfTransitionCount;
	bool endedDown;
};

struct Input
{
	bool isConnected;
	// maybe make an array for mouse buttons
	ButtonState leftMouseButton;
	ButtonState rightMouseButton;
	float stickAverageX;
	float stickAverageY;
	float mouseXScreen;
	float mouseYScreen;
	float mouseXOffset;
	float mouseYOffset;

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
		};
	};
};

#endif