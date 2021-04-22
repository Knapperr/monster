
#include "sdl_platform.h"

bool SDLPlatform::init(int SCREEN_WIDTH, int SCREEN_HEIGHT, int PORT_WIDTH, int PORT_HEIGHT)
{
	// Required to call this for Windows
	// I'm not sure why SDL2 doesn't do this on Windows automatically?
	//#if _WIN32
	//	SetProcessDPIAware();
	//#endif

		// TODO(ck): Config struct that sets these
		// config has the window in it
		// if config->graphics == OPENGL then set opengl stuff

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER) < 0)
		return false;
	// NOTE(ck): See this in Noel Berry's sdl platform 
	// 	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER) != 0)

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	int flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;
	window = SDL_CreateWindow("Monster",
							  SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
							  SCREEN_WIDTH, SCREEN_HEIGHT,
							  flags);

	if (window == nullptr)
		return false;

	// TODO(ck): Platform creates the context and grabs it from this 
	context = SDL_GL_CreateContext(window);
	if (context == nullptr)
		return false;

	printf("Platform: SDL2\n");
	gladLoadGLLoader(SDL_GL_GetProcAddress);
	printf("Vendor:   %s\n", glGetString(GL_VENDOR));
	printf("Renderer: %s\n", glGetString(GL_RENDERER));
	printf("Version:  %s\n", glGetString(GL_VERSION));

	// Use v-sync
	//SDL_GL_SetSwapInterval(1);

	int xOffset = (SCREEN_WIDTH - PORT_WIDTH) / 2;
	int yOffset = (SCREEN_HEIGHT - PORT_HEIGHT) / 2;
	glViewport(10, yOffset, PORT_WIDTH, PORT_HEIGHT);
	printf("Window Size: %d, %d\n", SCREEN_WIDTH, SCREEN_HEIGHT);
	printf("View Port Size: %d, %d\n", PORT_WIDTH, PORT_HEIGHT);

	//glEnable(GL_CULL_FACE);
	//glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	return true;
}


void SDLPlatform::processKeyboard(ButtonState* newState, bool isDown)
{
	if (newState->endedDown != isDown)
	{
		newState->endedDown = isDown;
		++newState->halfTransitionCount;
	}
}

void SDLPlatform::processMouseMotion(Input* newInput, SDL_Event* e, int& lastX, int& lastY)
{
	int winX, winY, x, y;

	SDL_GetWindowPosition(window, &winX, &winY);
	SDL_GetRelativeMouseState(&x, &y);
	//SDL_GetGlobalMouseState(&x, &y);
	newInput->mouseXOffset = (float)x;
	newInput->mouseYOffset = (float)-y;

	//float x = e->motion.x;
	//float y = e->motion.y;

	lastX = x;
	lastY = y;
}

void SDLPlatform::pollInput(Input* newInput, Input* oldInput)
{
	//Input* newInput = &input[0];
	//Input* oldInput = &input[1];

	*newInput = {};


	for (int buttonIndex = 0; buttonIndex < ArrayCount(newInput->buttons); ++buttonIndex)
	{
		newInput->buttons[buttonIndex].endedDown = oldInput->buttons[buttonIndex].endedDown;
	}

	newInput->leftMouseButton.endedDown = oldInput->leftMouseButton.endedDown;
	newInput->mouseXOffset = oldInput->mouseXOffset;
	newInput->mouseYOffset = oldInput->mouseYOffset;


	// TODO(ck): Idk if I need these MIGHT BE ABLE TO USE OLD INPUT..?
	// TODO(ck): Then get mouse data
	static int lastX = 0;
	static int lastY = 0;
	static int lastXAfterPress = 0;
	static int lastYAfterPress = 0;
	SDL_Event e;
	// Update mouse every frame
	processMouseMotion(newInput, &e, lastX, lastY);

	// TODO(ck): fix input polling
	while (SDL_PollEvent(&e))
	{

		if (e.type == SDL_QUIT)
		{
			quit = true;
		}

		if (e.type == SDL_MOUSEBUTTONDOWN)
		{
			// TODO(ck): Wrap cursor hiding in the input? and send to the camera 
			if (e.button.button == SDL_BUTTON_LEFT && !GuiActive())
			{
				processKeyboard(&newInput->leftMouseButton, true);
				SDL_SetRelativeMouseMode(SDL_TRUE);
				SDL_ShowCursor(SDL_DISABLE);
				lastXAfterPress = e.motion.x;
				lastYAfterPress = e.motion.y;
				//std::cout << "x after " << lastXAfterPress << "\n";
				//std::cout << "x after " << lastYAfterPress << "\n";

			}
		}
		else if (e.type == SDL_MOUSEBUTTONUP)
		{
			if (e.button.button == SDL_BUTTON_LEFT && !GuiActive())
			{
				processKeyboard(&newInput->leftMouseButton, false);
				SDL_SetRelativeMouseMode(SDL_FALSE);
				SDL_WarpMouseInWindow(window, lastXAfterPress, lastYAfterPress);
				SDL_ShowCursor(SDL_ENABLE);
			}
		}
		else if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP)
		{
			// TODO(ck): 
			// Check against SDL_KEYDOWN and SDL_KEYUP
			// set the true for false to this   isDown = ? SDL_KEYDOWN : SDL_KEYUP

			SDL_Keycode keyCode = e.key.keysym.sym;
			bool isDown = e.type == SDL_KEYDOWN ? true : false;

			if (e.key.repeat == 0)
			{
				if (keyCode == SDLK_UP || keyCode == SDLK_w)
					processKeyboard(&newInput->up, isDown);
				if (keyCode == SDLK_DOWN || keyCode == SDLK_s)
					processKeyboard(&newInput->down, isDown);
				if (keyCode == SDLK_LEFT || keyCode == SDLK_a)
					processKeyboard(&newInput->left, isDown);
				if (keyCode == SDLK_RIGHT || keyCode == SDLK_d)
					processKeyboard(&newInput->right, isDown);
				if (keyCode == SDLK_e)
					processKeyboard(&newInput->raise, isDown);
				if (keyCode == SDLK_q)
					processKeyboard(&newInput->lower, isDown);
				if (keyCode == SDLK_LSHIFT)
					processKeyboard(&newInput->shift, isDown);
			}
		}
		else if (e.type == SDL_JOYBUTTONDOWN || e.type == SDL_JOYBUTTONUP)
		{
			bool isDown = e.type == SDL_JOYBUTTONDOWN ? true : false;
			if (e.jbutton.button == 0)
			{
				int x = 0;
				//SDL_JoystickGetButton();
			}
		}
	}
}

uint64_t SDLPlatform::performanceFrequency()
{
	return SDL_GetPerformanceFrequency();
}

uint64_t SDLPlatform::performanceCounter()
{
	return SDL_GetPerformanceCounter();
}

void SDLPlatform::setTitle(Config* config, const char* title)
{
	// TODO(ck):
}

void SDLPlatform::cleanUp()
{
	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();
}