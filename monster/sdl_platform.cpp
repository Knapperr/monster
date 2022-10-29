#include "sdl_platform.h"

#include "mon_log.h"

#include <stb_image/stb_image.h>

namespace Mon
{
	const int JOYSTICK_DEAD_ZONE = 7849;

	bool SDLPlatform::init(Settings* settings)
	{
		state = {};

		// Required to call this for Windows
		// I'm not sure why SDL2 doesn't do this on Windows automatically?
		#if _WIN32
			SetProcessDPIAware();
		#endif

		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER) < 0)
			return false;

		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
		// TODO(ck): Do I want to double buffer here?
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

#ifdef _3D_
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
#endif
		int flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI;
		window = SDL_CreateWindow(settings->title,
								  SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
								  settings->windowWidth, settings->windowHeight, flags);

		if (window == nullptr)
			return false;

		// Window Icon
		// https://wiki.libsdl.org/SDL_CreateRGBSurfaceFrom
		// This example shows how to create a SDL_Surface* with the data loaded from an image
		// file with the stb_image.h library (https://github.com/nothings/stb/)

		unsigned char* pixels;
		int width, height;
		int originalFormat;
		int requiredFormat = STBI_rgb_alpha;
		//stbi_set_flip_vertically_on_load(flip);
		pixels = stbi_load("res/textures/ch_minion.png", &width, &height, &originalFormat, requiredFormat);
		
		// Set up the pixel format color masks for RGB(A) byte arrays.
		// Only STBI_rgb (3) and STBI_rgb_alpha (4) are supported here!
		Uint32 rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
		int shift = (req_format == STBI_rgb) ? 8 : 0;
		rmask = 0xff000000 >> shift;
		gmask = 0x00ff0000 >> shift;
		bmask = 0x0000ff00 >> shift;
		amask = 0x000000ff >> shift;
#else // little endian, like x86
		rmask = 0x000000ff;
		gmask = 0x0000ff00;
		bmask = 0x00ff0000;
		amask = (requiredFormat == STBI_rgb) ? 0 : 0xff000000;
#endif

		int depth, pitch;
		if (requiredFormat == STBI_rgb) 
		{
			depth = 24;
			pitch = 3 * width; // 3 bytes per pixel * pixels per row
		}
		else 
		{ // STBI_rgb_alpha (RGBA)
			depth = 32;
			pitch = 4 * width;
		}
		SDL_Surface* surface = SDL_CreateRGBSurfaceFrom((void*)pixels, width, height, depth, pitch,
													 rmask, gmask, bmask, amask);
		// The icon is attached to the window pointer
		SDL_SetWindowIcon(window, surface);
		SDL_FreeSurface(surface);
		stbi_image_free(pixels);

		// TODO(ck): Platform creates the context and grabs it from this 
		context = SDL_GL_CreateContext(window);
		if (nullptr == context)
			return false;

		Mon::Log::print("Platform: SDL2");
		gladLoadGLLoader(SDL_GL_GetProcAddress);
		
		unsigned char* info[3] = {};
		info[0] = (unsigned char*)glGetString(GL_VENDOR);
		info[1] = (unsigned char*)glGetString(GL_RENDERER);
		info[2] = (unsigned char*)glGetString(GL_VERSION);
		Mon::Log::print("Vendor", (char*)info[0]);
		Mon::Log::print("Renderer", (char*)info[1]);
		Mon::Log::print("Version", (char*)info[2]);
		Mon::Log::print("PID", _getpid());

		// Use v-sync
		// https://wiki.libsdl.org/SDL_GL_SetSwapInterval
		// 0 for immediate updates, 1 for updates synchronized with the vertical retrace, -1 for adaptive 
		SDL_GL_SetSwapInterval(1);
		settings->vsync = SDL_GL_GetSwapInterval();

#ifdef _3D_
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glEnable(GL_MULTISAMPLE);

#endif
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);

		// initialize joystick controller
		// ------------------
		if (SDL_NumJoysticks() < 1)
		{
			Mon::Log::warn("No joysticks are plugged in");
		}
		else
		{
			joyStick = SDL_JoystickOpen(0);
			if (nullptr == joyStick)
			{
				Mon::Log::print("Unable to open game controller SDL Error:", SDL_GetError());
			}
			else
			{
				Mon::Log::print("PLATFORM", "joystick connected");
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

		SDL_GetRelativeMouseState(&x, &y);
		SDL_GetMouseState(&winX, &winY);
		
		newInput->mouseOffset.x = (float)x;
		newInput->mouseOffset.y = (float)-y;

		// TODO IMPORTANT(ck): Do I need to offset this with the port?
		newInput->mouseScreen = v2((float)(winX), (float)(winY));

		lastX = (int)newInput->mouseOffset.x;
		lastY = (int)newInput->mouseOffset.y;
	}

	
	float processStickValue(int16_t value, int deadzoneThreshold)
	{
		float result = 0;
		
		if (value < -deadzoneThreshold)
			result = (float)((value + deadzoneThreshold) / (32768.0f - deadzoneThreshold));
		else if (value > deadzoneThreshold)
			result = (float)((value - deadzoneThreshold) / (32767.0f - deadzoneThreshold));

		return result;
	}

	void SDLPlatform::pollInput(Mon::Input* newInput, Mon::Input* oldInput)
	{
		// clear new input
		*newInput = {};

		for (int buttonIndex = 0; buttonIndex < ArrayCount(newInput->buttons); ++buttonIndex)
		{
			newInput->buttons[buttonIndex].endedDown = oldInput->buttons[buttonIndex].endedDown;
		}

		// TODO(ck): Add to loop mouse[] something like that
		newInput->lMouseBtn.endedDown = oldInput->lMouseBtn.endedDown;
		newInput->rMouseBtn.endedDown = oldInput->rMouseBtn.endedDown;
		//newInput->wheel = oldInput->wheel;	
		newInput->mouseOffset = oldInput->mouseOffset;
		newInput->mouseScreen = oldInput->mouseScreen;
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

		// Update joystick every frame TODO(ck): Method for this
		// ============================================================
		newInput->isAnalog = oldInput->isAnalog;

		int16_t xAxis, yAxis, rXAxis, rYAxis;
		xAxis = SDL_JoystickGetAxis(joyStick, 0);
		yAxis = SDL_JoystickGetAxis(joyStick, 1);
		rXAxis = SDL_JoystickGetAxis(joyStick, 2);
		rYAxis = SDL_JoystickGetAxis(joyStick, 3);
		newInput->stickAverageX = processStickValue(xAxis, JOYSTICK_DEAD_ZONE);
		newInput->stickAverageY = processStickValue(yAxis, JOYSTICK_DEAD_ZONE);
		newInput->stickAvgRX = processStickValue(rXAxis, JOYSTICK_DEAD_ZONE);
		newInput->stickAvgRY = processStickValue(rYAxis, JOYSTICK_DEAD_ZONE);

		if ((newInput->stickAverageX != 0.0f) ||
			(newInput->stickAverageY != 0.0f))
		{
			newInput->isAnalog = true;
		}
		else
		{
			newInput->isAnalog = false;
		}

		// ============================================================


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
				if (e.button.button == SDL_BUTTON_LEFT && isGuiActive == false)
				{
					processKeyboard(&newInput->lMouseBtn, true);

					SDL_SetRelativeMouseMode(SDL_TRUE);
					relativeMouseMode = true;
					lastXAfterPress = (int)newInput->mouseScreen.x;
					lastYAfterPress = (int)newInput->mouseScreen.y;
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
				bool isDown = (e.type == SDL_KEYDOWN ? true : false);

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
					if (keyCode == SDLK_r)
						processKeyboard(&newInput->r, isDown);
					if (keyCode == SDLK_f)
						processKeyboard(&newInput->f, isDown);
					if (keyCode == SDLK_e)
						processKeyboard(&newInput->raise, isDown);
					if (keyCode == SDLK_q)
						processKeyboard(&newInput->lower, isDown);
					if (keyCode == SDLK_g)
						processKeyboard(&newInput->debug, isDown);
					if (keyCode == SDLK_LSHIFT)
						processKeyboard(&newInput->shift, isDown);
					if (keyCode == SDLK_ESCAPE)
						processKeyboard(&newInput->quit, isDown);
					if (keyCode == SDLK_SPACE)
						processKeyboard(&newInput->space, isDown);

					if (keyCode == SDLK_KP_1)
						processKeyboard(&newInput->num1, isDown);
					if (keyCode == SDLK_KP_2)
						processKeyboard(&newInput->num2, isDown);
					if (keyCode == SDLK_KP_3)
						processKeyboard(&newInput->num3, isDown);
					if (keyCode == SDLK_KP_4)
						processKeyboard(&newInput->num4, isDown);
				}
			}
			else if (e.type == SDL_JOYBUTTONDOWN || e.type == SDL_JOYBUTTONUP)
			{
				//int buttons = SDL_JoystickNumButtons(joyStick);
				bool isDown = (e.type == SDL_JOYBUTTONDOWN ? true : false);
				// TODO(ck): Handle buttons... 
				if (e.jbutton.button == 6) // SELECT 
					processKeyboard(&newInput->debug, isDown);

				if (e.jbutton.button == 4)
				{
					processKeyboard(&newInput->lBumper, isDown);
				}
				if (e.jbutton.button == 5)
				{
					processKeyboard(&newInput->rBumper, isDown);
				}

			}
			else if (e.type == SDL_MOUSEWHEEL)
			{
				// TODO(ck): Handle X axis and direction
				newInput->wheel.y = e.wheel.y;
			}
			// IMPORTANT(ck): Need to always process joystick at beginning of poll
			// SDL_CONTROLLERAXISMOTION
			else if (e.type == SDL_JOYAXISMOTION)
			{
				if (e.jaxis.which == 0)
				{
					// Joystick
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

	void SDLPlatform::swapWindow()
	{
		SDL_GL_SwapWindow(window);
	}

	void SDLPlatform::setWindowSize(Settings* settings)
	{

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