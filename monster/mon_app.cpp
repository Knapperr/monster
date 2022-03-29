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

	// TODO(ck): Memory Allocation
#ifdef _3D_
	//game = new Mon::Game();
	//if (!game->init(settings.windowWidth, settings.windowHeight, settings.portWidth, settings.portHeight))
		//return false;

	gameState = new Mon::GameState();
	if (false == Mon::InitGame(gameState, settings.windowWidth, settings.windowHeight, settings.portWidth, settings.portHeight))
		return false;

#else
	// TODO(ck): Memory allocation
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
			Mon::Update(gameState, deltaTime, newInput);
#else
			Mon::Update(game2D, deltaTime, newInput);
#endif
			frameTime -= deltaTime;
			// NOTE(ck): T is the current time ... not using this?
			// t += deltaTime;
		}
			
		// TODO(ck): Platform->Renderer->clearColor 
		glClearColor(0.126f, 0.113f, 0.165f, 1.0f);

#ifdef _3D_
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#else
		glClear(GL_COLOR_BUFFER_BIT);
#endif


#ifdef _3D_
		Mon::Render(gameState, 1.0f);
#else
		Mon::Render(game2D);
#endif

		if (showGUI)
		{ 
#ifdef _3D_
			UpdateGui(platform->window, &settings, gameState);
#else
			UpdateGui(platform->window, &settings, game2D);
#endif
			RenderGui();
		}

		// TODO(ck): Platform->swapWindow()
		SDL_GL_SwapWindow(platform->window);


		// Swap input
		Mon::Input* temp = newInput;
		newInput = oldInput;
		oldInput = temp;
	}
	Mon::Log::print("Shutting down...");
	Mon::Log::shutdown();
#ifdef _3D_
	Mon::CleanUp(gameState);
	//game->cleanUp();
#else
	Mon::CleanUp(game2D);
#endif
	ShutdownGui();
	platform->cleanUp();
}


