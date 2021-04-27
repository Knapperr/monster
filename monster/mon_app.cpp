#include "mon_app.h"


#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))

const unsigned int SCREEN_WIDTH = 1440;
const unsigned int SCREEN_HEIGHT = 720;
const unsigned int PORT_WIDTH = 960;
const unsigned int PORT_HEIGHT = 540;

struct Timer
{
	uint64_t diff;
	uint64_t milliseconds;
	double fixed_dt;
	double last;
	double elapsed;
	double previousElapsed;
};

bool App::init()
{

	// TODO(ck): Memory Allocation
	platform = new SDLPlatform();


	if (!platform->init(SCREEN_WIDTH, SCREEN_HEIGHT, PORT_WIDTH, PORT_HEIGHT))
		return false;

	InitGui(platform->window, &platform->context);
	
	//Platform->SetTitle(window, "Monster");

	// TODO(ck): Memory Allocation
	oldInput = new Input();
	newInput = new Input();
	//oldInput = &input[0];
	//newInput = &input[1];
	game = new Mon::Game();

#ifdef _3D_
	if (!game->init())
		return false;
#else
	if (!game->init(1))
		return false;
#endif

	return true;
}

void App::run()
{

	uint32_t currTime = (uint32_t)platform->performanceCounter();
	uint32_t last = 0;
	double dt = 0;

	double updateRate = 60.00;
	int updateMultiplicity = 1;
	bool unlockFrameRate = false;

	double fixedDeltaTime = 1.0 / updateRate;
	int64_t desiredFrameTime = (int64_t)platform->performanceFrequency() / updateRate;

	int64_t vsyncMaxError = (int64_t)platform->performanceFrequency() * 0.0002;
	int64_t time60hz = (int64_t)platform->performanceFrequency() / 60;
	int64_t snapFrequencies[] =
	{
		time60hz,
		time60hz * 2,		// 30fps
		time60hz * 3,		// 20fps
		time60hz * 4,		// 15fps
		(time60hz + 1) / 2, // 120fps
	};

	// TODO(ck):
	// Should use a ring buffer for this
	const int TIME_HISTORY_COUNT = 4;
	int64_t timeAverager[TIME_HISTORY_COUNT] = { desiredFrameTime, desiredFrameTime, desiredFrameTime, desiredFrameTime };

	// TODO(ck):
	// should be in an App class that uses the platform layer
	bool resync = true;
	int64_t prevFrameTime = platform->performanceCounter();
	int64_t frameAccumulator = 0;
	// ------------------------------------------------------------------------------------------------------------------------

	while (running)
	{
		// TODO(ck): Platform::PerformanceCounter()
		int64_t currentFrameTime = platform->performanceCounter();
		int64_t deltaTime = currentFrameTime - prevFrameTime;
		prevFrameTime = currentFrameTime;

		// handle unexpected timer anomolies (overflow, extra slow frames, etc)
		if (deltaTime > desiredFrameTime * 8) // ignore extra slow frames
		{
			deltaTime = desiredFrameTime;
		}
		if (deltaTime < 0)
		{
			deltaTime = 0;
		}

		// vsync time snapping
		for (int64_t snap : snapFrequencies)
		{
			if (std::abs(deltaTime - snap) < vsyncMaxError)
			{
				deltaTime = snap;
				break;
			}
		}

		// delta time averaging 
		for (int i = 0; i < TIME_HISTORY_COUNT - 1; ++i)
		{
			timeAverager[i] = timeAverager[i + 1];
		}
		timeAverager[TIME_HISTORY_COUNT - 1] = deltaTime;
		deltaTime = 0;
		for (int i = 0; i < TIME_HISTORY_COUNT; ++i)
		{
			deltaTime = timeAverager[i];
		}
		deltaTime /= TIME_HISTORY_COUNT;

		// add to the accumulator 
		frameAccumulator += deltaTime;

		// spriral of death protection
		if (frameAccumulator > desiredFrameTime * 8)
		{
			resync = true;
		}

		// timer resync if requested
		if (resync)
		{
			frameAccumulator = 0;
			deltaTime = desiredFrameTime;
			resync = false;
		}

	
		platform->pollInput(newInput, oldInput);

		// platform checks for quit in the pollinput
		if (platform->quit == true)
			running = false;


		if (unlockFrameRate)
		{
			int64_t consumedDeltaTime = deltaTime;

			while (frameAccumulator >= desiredFrameTime) {
				//game->Update(fixedDeltaTime, newInput);

				if (consumedDeltaTime > desiredFrameTime)
				{
					// whatever this means
					// g_Game->VariableUpdate()
					//game->Update(fixedDeltaTime, newInput);
					consumedDeltaTime -= desiredFrameTime;
				}
				frameAccumulator -= desiredFrameTime;
			}

			// g_Game->VariableUpdate();
			//game->Update((double)consumedDeltaTime / SDL_GetPerformanceFrequency(), newInput);
			//UpdateGui(window);

			glClearColor(0.126f, 0.113f, 0.165f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			//game->Render(fixedDeltaTime);
			RenderGui();

			//SDL_GL_SwapWindow(window);


			// TODO(ck): Input manager deals with this
			//Input* temp = newInput;
			//newInput = oldInput;
			//oldInput = temp;

		}
		else
		{
			//while (frameAccumulator >= desiredFrameTime * updateMultiplicity)
			//{
			//	for (int i = 0; i < updateMultiplicity; ++i)
			//	{
			//		g_Game->Update(fixedDeltaTime, newInput);
			//		// g_Game->VariableUpdate
			//		frameAccumulator -= desiredFrameTime;
			//	}
			//}v
#ifdef _3D_
			game->update(fixedDeltaTime, newInput);
#else
			game->update(fixedDeltaTime, newInput, 1);
#endif

			// Somehow get app state into game to check this and 
			// just call that?


			UpdateGui(platform->window, game);

			// TODO(ck): Platform->Renderer->clearColor 
			glClearColor(0.126f, 0.113f, 0.165f, 1.0f);
			// TODO(ck): Platform->Renderer->clear
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#ifdef _3D_
			game->render(1);
#else
			game->render(); // NOTE(ck): 2D
#endif


			RenderGui();


			// TODO(ck): Platform->swapWindow()
			SDL_GL_SwapWindow(platform->window);

			Input* temp = newInput;
			newInput = oldInput;
			oldInput = temp;
		}
	}
	game->cleanUp();
	ShutdownGui();
	platform->cleanUp();
}


