#include "sdl_platform.h"

namespace Mon
{

	const int JOYSTICK_DEAD_ZONE = 8000;

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

		// IMPORTANT(ck): Entry point for renderdoc
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
		printf("PID: %d\n", _getpid());

		// Use v-sync
		// https://wiki.libsdl.org/SDL_GL_SetSwapInterval
		// 0 for immediate updates, 1 for updates synchronized with the vertical retrace, -1 for adaptive vsync
		SDL_GL_SetSwapInterval(1);

		//glEnable(GL_CULL_FACE);
		//glDepthFunc(GL_LESS);
		glEnable(GL_BLEND);
#ifdef _3D_
		glEnable(GL_DEPTH_TEST); // NOTE(ck): OFF FOR 2D
		//glDepthFunc(GL_ALWAYS);
#endif
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


		// controller
		// ------------------
		if (SDL_NumJoysticks() < 1)
		{
			printf("PLATFORM: No joystick connected\n");
		}
		else
		{
			joyStick = SDL_JoystickOpen(0);
			if (joyStick == NULL)
			{
				printf("PLATFORM: Unable to open game controller! SDL Error: %s\n", SDL_GetError());
			}
		}


		return true;
	}


	void SDLPlatform::processKeyboard(Mon::ButtonState* newState, bool isDown)
	{
		if (newState->endedDown != isDown)
		{
			newState->endedDown = isDown;
			++newState->halfTransitionCount;
		}
	}

	void SDLPlatform::processMouseMotion(Mon::Input* newInput, SDL_Event* e, int& lastX, int& lastY)
	{
		int winX, winY, x, y;

		//SDL_GetWindowPosition(window, &winX, &winY);
		SDL_GetRelativeMouseState(&x, &y);
		SDL_GetMouseState(&winX, &winY);
		//SDL_GetGlobalMouseState(&x, &y);
		newInput->mouseOffset.x = (float)x;
		newInput->mouseOffset.y = (float)-y;

		newInput->mouseXScreen = (float)winX;
		newInput->mouseYScreen = (float)winY;

		lastX = newInput->mouseOffset.x;
		lastY = newInput->mouseOffset.y;
	}

	void SDLPlatform::pollInput(Mon::Input* newInput, Mon::Input* oldInput)
	{
		//Input* newInput = &input[0];
		//Input* oldInput = &input[1];

		*newInput = {};


		for (int buttonIndex = 0; buttonIndex < ArrayCount(newInput->buttons); ++buttonIndex)
		{
			newInput->buttons[buttonIndex].endedDown = oldInput->buttons[buttonIndex].endedDown;
		}

		newInput->lMouseBtn.endedDown = oldInput->lMouseBtn.endedDown;
		newInput->rMouseBtn.endedDown = oldInput->rMouseBtn.endedDown;

		// TODO(ck): Add to loop mouse[] something like that
		newInput->mouseOffset = oldInput->mouseOffset;
		newInput->mouseXScreen = oldInput->mouseXScreen;
		newInput->mouseYScreen = oldInput->mouseYScreen;
		newInput->rightStickValue = oldInput->rightStickValue;
		newInput->rightStickAxis = oldInput->rightStickAxis;
		newInput->rightStickAngle = oldInput->rightStickAngle;
		newInput->stickDir = oldInput->stickDir;


		// TODO(ck): Idk if I need these MIGHT BE ABLE TO USE OLD INPUT..?
		// TODO(ck): Then get mouse data
		static int lastX = 0;
		static int lastY = 0;
		static int lastXAfterPress = 0;
		static int lastYAfterPress = 0;
		static bool relativeMouseMode = false;
		SDL_Event e;
		// Update mouse every frame
		processMouseMotion(newInput, &e, lastX, lastY);

		// TODO(ck): fix input polling
		while (SDL_PollEvent(&e))
		{
			// TODO(ck): wrap in gui layer
			// without it you won't have keyboard input and other things
			ImGui_ImplSDL2_ProcessEvent(&e);
			bool isGuiActive = GuiActive(relativeMouseMode);

			// you might also want to check io.WantCaptureMouse and io.WantCaptureKeyboard
			// before processing events


			if (e.type == SDL_QUIT)
			{
				quit = true;
			}

			// TODO(ck):
			// COMBINE INTO ONE SDL_MOUSEBUTTONDOWN || SDL_MOUSEBUTTONUP
			if (e.type == SDL_MOUSEBUTTONDOWN)
			{
				//if (e.button.button == SDL_BUTTON_LEFT)
				if (e.button.button == SDL_BUTTON_LEFT && isGuiActive == false)
				{
					processKeyboard(&newInput->lMouseBtn, true);

					SDL_SetRelativeMouseMode(SDL_TRUE);
					relativeMouseMode = true;
					lastXAfterPress = newInput->mouseXScreen;
					lastYAfterPress = newInput->mouseYScreen;
				}
				if (e.button.button == SDL_BUTTON_RIGHT)
				{
					processKeyboard(&newInput->rMouseBtn, true);
				}
			}
			else if (e.type == SDL_MOUSEBUTTONUP)
			{
				if (e.button.button == SDL_BUTTON_LEFT)
				{
					processKeyboard(&newInput->lMouseBtn, false);

					if (isGuiActive == false)
					{
						SDL_SetRelativeMouseMode(SDL_FALSE);
						relativeMouseMode = false;
						SDL_WarpMouseInWindow(window, lastXAfterPress, lastYAfterPress);
					}
				}
				if (e.button.button == SDL_BUTTON_RIGHT)
				{
					processKeyboard(&newInput->rMouseBtn, false);
				}
			}
			else if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP)
			{
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
			// SDL_CONTROLLERAXISMOTION
			else if (e.type == SDL_JOYAXISMOTION)
			{
				if (e.jaxis.which == 0)
				{
					static int yDir = 0;
					static int xDir = 0;
					if (e.jaxis.axis == 0)
					{
						if (e.jaxis.value < -JOYSTICK_DEAD_ZONE)
						{

							xDir = -1;
							newInput->stickDir.x = -2;
							processKeyboard(&newInput->left, true);
						}
						else if (e.jaxis.value > JOYSTICK_DEAD_ZONE)
						{
							xDir = 1;
							newInput->stickDir.x = 2;
							processKeyboard(&newInput->right, true);
						}
						else
						{
							xDir = 0;
							newInput->stickDir.x = 0;
							processKeyboard(&newInput->left, false);
							processKeyboard(&newInput->right, false);
						}
					}
					if (e.jaxis.axis == 1)
					{
						if (e.jaxis.value < -JOYSTICK_DEAD_ZONE)
						{
							yDir = -1;
							newInput->stickDir.y = 2;
							processKeyboard(&newInput->up, true);
						}
						else if (e.jaxis.value > JOYSTICK_DEAD_ZONE)
						{
							yDir = 1;
							newInput->stickDir.y = -2;
							processKeyboard(&newInput->down, true);
						}
						else
						{
							yDir = 0;
							newInput->stickDir.y = 0;
							processKeyboard(&newInput->up, false);
							processKeyboard(&newInput->down, false);
						}
					}

					newInput->rightStickValue = e.jaxis.value / 32767.0f;;
					newInput->rightStickAxis = e.jaxis.axis;

					/*
					TODO(ck): JOYSTICK ANGLE

					//Calculate angle
					double joystickAngle = atan2((double)yDir, (double)xDir) * (180.0 / M_PI);

					//Correct angle
					if (xDir == 0 && yDir == 0)
					{
						joystickAngle = 0;
					}
					*/

					/*
					else if (event.type == SDL_JOYAXISMOTION)
					{
						auto index = find_joystick_index(event.jdevice.which);
						if (index >= 0)
						{
							if (SDL_IsGameController(index) == SDL_FALSE)
							{
								float value;
								if (event.jaxis.value >= 0)
									value = event.jaxis.value / 32767.0f;
								else
									value = event.jaxis.value / 32768.0f;
								InputBackend::on_axis_move(index, event.jaxis.axis, value);
							}
						}
					}
					*/

					newInput->rightStickAngle = atan2((double)yDir, (double)xDir) * (180.0 / M_PI);
					//if (xDir == 0 && yDir == 0)
						//newInput->rightStickValue = 0;
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

	void SDLPlatform::sleep(int milliseconds)
	{
		if (milliseconds > 0)
			Sleep(milliseconds);
	}

	uint64_t SDLPlatform::ticks()
	{
		uint64_t counter = SDL_GetPerformanceCounter();
		double per_second = (double)SDL_GetPerformanceFrequency();
		// static constexpr u64 ticks_per_second = 1000000;
		return (uint64_t)(counter * (1000000 / per_second));
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
		SDL_JoystickClose(joyStick);
		joyStick = NULL;

		SDL_GL_DeleteContext(context);
		SDL_DestroyWindow(window);
		SDL_Quit();
	}
}