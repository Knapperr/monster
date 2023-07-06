#include "mon_app.h"

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
	Mon::Log::Log();
	platform = new Mon::SDLPlatform();
	settings = {};
	settings.title = "Monster";
	settings.windowWidth = 1440;
	settings.windowHeight = 900;
	settings.portWidth = 960.0f;
	settings.portHeight = 540.0f;
	Mon::Log::print("Title", settings.title);
	Mon::Log::print("Window Width", settings.windowWidth);
	Mon::Log::print("Window Height", settings.windowHeight);
	Mon::Log::print("port width, height", settings.portWidth, settings.portHeight);

	if (!platform->init(&settings))
		return false;

	InitGui(platform->window, &platform->context);

	//Platform->SetTitle(window, "Monster");

#ifdef _3D_
#if 0 // HANDMADE_INTERNAL
	LPVOID BaseAddress = (LPVOID)Terabytes(2);
#else
	LPVOID BaseAddress = 0;
#endif

	memory = {};
	memory.permanentStorageSize = Megabytes(64);
	memory.transientStorageSize = Gigabytes(1);
	// memory.debugStorageSize = Gigabytes(1); 

	platform->state.totalSize = (memory.permanentStorageSize + memory.transientStorageSize);
	// debugsize
	//platform->state.totalSize = (memory.permanentStorageSize + memory.transientStorageSize + memory.debugStorageSize);
	platform->state.gameMemoryBlock = VirtualAlloc(BaseAddress, (size_t)platform->state.totalSize,
												   MEM_RESERVE | MEM_COMMIT,
												   PAGE_READWRITE);
	memory.permanentStorage = platform->state.gameMemoryBlock;
	memory.transientStorage = ((uint8_t*)memory.permanentStorage + memory.permanentStorageSize);
	//memory.debugStorage = ((uint8_t *)memory.transientStorage + memory.transientStorageSize)

	if (memory.permanentStorage && memory.transientStorage)
	{
		if (!Mon::InitGame(&memory, settings.windowWidth, settings.windowHeight, settings.portWidth, settings.portHeight))
			return false;
	}
#else

	// TODO(ck): memory allocation
	game2D = new Mon::Game2D();
	if (!Mon::Init(game2D))
		return false;
#endif

	return true;
}

void App::run()
{
	Mon::Input* oldInput = &input[0];
	Mon::Input* newInput = &input[1];

	// startup
	//if (app_config.on_startup != nullptr)
		//app_config.on_startup();
	
	// time struct
	/*
	u64 Time::ticks = 0;
	u64 Time::previous_ticks = 0;
	double Time::seconds = 0;
	double Time::previous_seconds = 0;
	float Time::delta = 0;
	float Time::pause_timer = 0;
	*/
	
	// NOTE(ck): https://gafferongames.com/post/fix_your_timestep/
	// Semi-fixed timestep to start
	double t = 0.0;
	double frameRate = 60.0;
	double dt = 1 / frameRate;

	// hires time in seconds
	uint64_t currentTime = SDL_GetPerformanceCounter();

	while (running)
	{
		uint64_t newTime = SDL_GetPerformanceCounter();
		double frameTime = (newTime - currentTime) / (double)SDL_GetPerformanceFrequency();
		currentTime = newTime;

		// TODO(ck): Deal with spiral of death here


		// Process platform input and other system events
		platform->pollInput(newInput, oldInput);
		if (platform->quit == true)
			running = false;

		if (newInput->quit.endedDown)
		{
			showGUI = !showGUI;
		}

		while (frameTime > 0.0)
		{

			double deltaTime = std::min(frameTime, dt);

#ifdef _3D_
			Mon::Update(&memory, deltaTime, newInput);
#else
			Mon::Update(game2D, deltaTime, newInput);
#endif
			frameTime -= deltaTime;
			// NOTE(ck): T is the current time ... not using this?
			// t += deltaTime;
		}

#ifdef _3D_
#else
		glClear(GL_COLOR_BUFFER_BIT);
#endif

#ifdef _3D_
		// TODO(ck): PASS DELTA TIME TO RENDERER
		float time = float(SDL_GetTicks() / 1000.0f);
		Mon::Render(&memory, time, 1.0f);
#else
		// TODO(ck): PASS DELTA TIME TO RENDERER
		Mon::Render(game2D, 1.0f);
#endif

		if (showGUI)
		{ 
#ifdef _3D_
			UpdateGui(platform->window, &settings, &memory);
#else
			UpdateGui(platform->window, &settings, game2D);
#endif
			RenderGui();
		}
		platform->swapWindow();


		// Swap input
		Mon::Input* temp = newInput;
		newInput = oldInput;
		oldInput = temp;
	}
	Mon::Log::print("Shutting down...");
	Mon::Log::shutdown();
#ifdef _3D_
	//Mon::CleanUp(gameState);
#else
	Mon::CleanUp(game2D);
#endif
	ShutdownGui();
	platform->cleanUp();
}
