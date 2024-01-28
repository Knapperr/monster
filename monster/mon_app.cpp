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
	settings.windowWidth = 1440; // 1440
	settings.windowHeight = 900; // 900
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


#if 0 // HANDMADE_INTERNAL
	LPVOID BaseAddress = (LPVOID)Terabytes(2);
#else
	LPVOID BaseAddress = 0;
#endif
#ifdef _3D_

	memory = {};
	memory.permanentStorageSize = Megabytes(256);
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
	memory = {};
	memory.permanentStorageSize = Megabytes(256);
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
		if (!Mon::Init(&memory, settings.windowWidth, settings.windowHeight, settings.portWidth, settings.portHeight))
		{
			Mon::Log::print("2D Game Init failed...");
			return false;
		}
	}
#endif

	Mon::Log::print("App init successful");
	Mon::Log::print("Launching Monster...");
	return true;
}

struct MonTime
{
	uint64_t ticks = 0;
	uint64_t prevousTicks = 0;
	double seconds = 0.0;
	double previousSeconds = 0.0;
	float delta = 0.0f;
	float pauseTimer = 0.0f;
};

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
	double deltaTime = 0.0;

	// high res time in seconds
	// STUDY(ck): IMPORTANT(ck): idiot!! you can't have currentTime and newTime both set to SDL_GetPerformanceCounter()
	// if you get the same time then you will subtract it to 0 and get no frametime then you will not call Update 
	// before Render()
	// I'm not sure why you do it this way? it seems to be 1 digit away but if they are the same
	// value it crashes the frametime is 0 and i never get an update
	
	// figure out how to deal with this.. is subtracting it a good idea? should it just be set to some value?
	uint64_t currentTime = SDL_GetPerformanceCounter()-100;

	while (running)
	{
		uint64_t newTime = SDL_GetPerformanceCounter();
		double frameTime = (newTime - currentTime) / (double)SDL_GetPerformanceFrequency();
		//Mon::Log::print("performance frequency", std::to_string(SDL_GetPerformanceFrequency()).c_str());
		//Mon::Log::print("current time", std::to_string(currentTime).c_str());
		//Mon::Log::print("new time", std::to_string(newTime).c_str());
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

		
		//Mon::Log::print(std::to_string(frameTime).c_str());
		while (frameTime > 0.0)
		{

			deltaTime = std::min(frameTime, dt);

#ifdef _3D_
			Mon::Update(&memory, deltaTime, newInput);
#else
			Mon::Update(&memory, deltaTime, newInput);
#endif
			frameTime -= deltaTime;
			// NOTE(ck): T is the current time ... not using this?
			// t += deltaTime;
		}

#ifdef _3D_
		// TODO(ck): PASS DELTA TIME TO RENDERER
		float time = float(SDL_GetTicks() / 1000.0f);
		Mon::Render(&memory, time, deltaTime);
#else
		// TODO(ck): PASS DELTA TIME TO RENDERER
		Mon::Render(&memory, deltaTime);
#endif

		if (showGUI)
		{ 
#ifdef _3D_
			UpdateGui(platform->window, &settings, &memory);
#else
			UpdateGui(platform->window, &settings, &memory);
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
