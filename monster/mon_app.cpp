#include "mon_app.h"


#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))

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
	platform = new Mon::SDLPlatform();
	settings = {};
	settings.title = "Monster";
	settings.width = 1440;
	settings.height = 900;

	if (!platform->init(&settings))
		return false;

	InitGui(platform->window, &platform->context);
	
	//Platform->SetTitle(window, "Monster");

	// TODO(ck): Memory Allocation
	oldInput = new Mon::Input();
	newInput = new Mon::Input();
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
	

	// startup
	//if (app_config.on_startup != nullptr)
		//app_config.on_startup();

	uint64_t time_last = 0;
	uint64_t time_accumulator = 0;
	
	// time struct
	/*
	u64 Time::ticks = 0;
	u64 Time::previous_ticks = 0;
	double Time::seconds = 0;
	double Time::previous_seconds = 0;
	float Time::delta = 0;
	float Time::pause_timer = 0;
	*/
	uint64_t ticks = 0;
	uint64_t previousTicks = 0;
	double seconds = 0;
	double previousSeconds = 0;
	float delta = 0;
	float pauseTimer = 0;
	// ------------------------------------------------------------------------------------------------------------------------

	int target_framerate = 60;
	uint64_t ticks_per_second = 1000000;
	int max_updates = 5;

	while (running)
	{
		// fixed time framerate
		{
			uint64_t time_target = (uint64_t)((1.0 / target_framerate) * ticks_per_second);
			uint64_t time_curr = platform->ticks();
			uint64_t time_diff = time_curr - time_last;
			time_last = time_curr;
			time_accumulator += time_diff;

			// do not run too fast
			while (time_accumulator < time_target)
			{
				int milliseconds = (int)(time_target - time_accumulator) / (ticks_per_second / 1000);
				// TODO(ck): Sleep might be causing stuttering this happened in handmade remember
				// TODO(ck): The bandaid for handmade was the multiply the time passed to sleep by 400
				// TODO(ck): For some reason here sleep needs to be divided by 2 in order for it to run smoothly.
				//			 Sleep() on this machine is very problematic?
				platform->sleep(milliseconds/2);

				time_curr = platform->ticks();
				time_diff = time_curr - time_last;
				time_last = time_curr;
				time_accumulator += time_diff;
			}

			// Do not allow us to fall behind too many updates
			// (otherwise we'll get spiral of death)

			uint64_t time_maximum = max_updates * time_target; // max_updates = 5
			if (time_accumulator > time_maximum)
				time_accumulator = time_maximum;

			// do as many updates as we can
			while (time_accumulator >= time_target)
			{
				time_accumulator -= time_target;

				// timer.delta
				delta = (1.0f / target_framerate); // 60

				if (pauseTimer > 0)
				{
					pauseTimer -= delta;
					if (pauseTimer <= -0.0001)
						delta = -pauseTimer;
					else
						continue;
				}

				// Timer:: this needs to be packed in and passed to game
				previousTicks = ticks;
				ticks += time_target;
				previousSeconds = seconds;
				seconds += delta;
				//printf("delta: %f\n", previousSeconds);

				// input backend just resets the state of the input instead of doing it every frame at the top
				//InputBackend::frame();
				//GraphicsBackend::frame();

				//if (app_config.on_update != nullptr)
					//app_config.on_update();


				platform->pollInput(newInput, oldInput);
				if (platform->quit == true)
					running = false;

				if (newInput->quit.endedDown)
				{
					showGUI = !showGUI;
				}

				Mon::Input* temp = newInput;
				newInput = oldInput;
				oldInput = temp;

#ifdef _3D_
				game->update(delta, newInput);
#else
				// TODO(ck): I think I need to reset my inputs here like switch between states
				// that means I would have to remove the resetting of states to be in here 
				// maybe platform->clear() or something like that
				game->update(delta, newInput, 1);


#endif
			}
		}

		
		// TODO(ck): Platform->Renderer->clearColor 
		glClearColor(0.126f, 0.113f, 0.165f, 1.0f);

#ifdef _3D_
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#else
		glClear(GL_COLOR_BUFFER_BIT);
#endif


#ifdef _3D_
		game->render(1);
#else
		game->render(); // NOTE(ck): 2D
#endif

		if (showGUI)
		{ 
			UpdateGui(platform->window, &settings, game);
			RenderGui();
		}

		// TODO(ck): Platform->swapWindow()
		SDL_GL_SwapWindow(platform->window);
	}
	game->cleanUp();
	ShutdownGui();
	platform->cleanUp();
}


