#include "mon_game2D.h"

namespace Mon {
	
	// New 
	bool Init(Game2D* game)
	{
		//
		// Initialize Assets for game here
		//
		// NOTE(ck): MUST BE CALLED FIRST - LOADS ALL ASSETS FOR GAME
		// TODO(ck): Memory management - allocate world
		
		/*
			g_Assets
			game->renderer
			World2D();

		*/


		InitAssets(g_Assets);
		MonGL::InitRenderer2D(&game->renderer);
		
		// TODO(ck): Memory management - allocate world
		game->world = new World2D();
		InitWorld(game->world);


		// Set up the shader locations for our objects
		//int shaderID = game->renderer.program.handle;
		//glUseProgram(shaderID);

		// TODO(CK): CAMERA
		// So let's say you want your pixel art scale 2:1
		// Then your target 1080p. Just take the resolution and divide by 2. Examples
		// This is for 1920x1080 I am using 1280x720 right now
		// 2:1 960x540 -- 3:1 640x360 --- 4:! 480x240
		//float left = 0.0f;
		//float right = 960.0f;
		//float bottom = 540.0f;
		//float top = 0.0f;

		//mat4 projection = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);

		//int imgLoc = glGetUniformLocation(shaderID, "image");
		//glUniform1i(imgLoc, 0);

		//int projLoc = glGetUniformLocation(shaderID, "projection");
		//glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));


		int screenWidth = 1440;
		int screenHeight = 900;
		int portWidth = 960;
		int portHeight = 540;
		game->config = new MonGL::Config();

		// TODO(ck): View Port in Middle of screen
		//game->config->viewPort = { (float)(screenWidth - portWidth) / 2 , (float)(screenHeight - portHeight) / 2, (float)portWidth, (float)portHeight };
		game->config->viewPort = { 0.0f , 0.0f, (float)portWidth, (float)portHeight };
		MonGL::ViewPort(&game->config->viewPort);

		AddCamera(game);
		game->currentCameraIndex = AddCamera(game);
		// InitCamera
		InitCamera(&game->cameras[game->currentCameraIndex]);


		Entity2D* player = GetPlayer(game->world);
		//game->camera = OrthoCamera(player->pos, &game->config->viewPort);
		
		game->state = State::Play;

		return true;
	}

	void Update(Game2D* game, double dt, Input* input)
	{
		//if (dt > deltaTime || dt < deltaTime)
		//printf("dt: %f\n", dt);
		//game->deltaTime = dt;
		game->input = *input;

		if (game->state == State::Play)
		{
			Entity2D* p = GetPlayer(game->world);
			v2 velocity = {};

			if (input->isAnalog)
			{
				velocity = v2{ input->stickAverageX, -input->stickAverageY };
			}
			else
			{

				if (input->up.endedDown)
				{
#define USE_VELOCITY
#ifdef USE_VELOCITY
					velocity.y = 1.0f;
#else
					p->pos.y += 1.0f * p->speed * dt;
#endif
				}
				if (input->down.endedDown)
				{
#ifdef USE_VELOCITY
					velocity.y = -1.0f;
#else
					p->pos.y -= 1.0f * p->speed * dt;
#endif
				}
				if (input->left.endedDown)
				{
#ifdef USE_VELOCITY
					velocity.x = -1.0f;
#else
					p->pos.x -= 1.0f * p->speed * dt;
#endif
				}
				if (input->right.endedDown)
				{
#ifdef USE_VELOCITY
					velocity.x = 1.0f;
#else
					p->pos.x += 1.0f * p->speed * dt;
#endif
				}
				
			}
#ifdef USE_VELOCITY
			Mon::movePlayer(game->world->map, p, &velocity, dt);
#endif
			// TODO(ck): link sprite position to camera...  
			// https://www.reddit.com/r/gamedev/comments/7cnqpg/lerping_camera_position_causes_jitters_as_it/
			// update sprite position 
			 
			// this one is better explanation for why you shouldn't tie camera to player position and only to an offset
			//https://gamedev.stackexchange.com/questions/2642/scrolling-2d-sprites-on-a-map-with-a-camera

			// positions should be relative to the map the camera shouldn't come into play with real positions..
			//game->cameras[game->currentCameraIndex].update(&p->pos, dt);
			Update(&game->cameras[game->currentCameraIndex], &p->pos, dt);

			p->sprite.pos = p->pos;

		}
	}

	void Render(Game2D* game)
	{
		MonGL::ViewPort(&game->config->viewPort);

		int shaderID = game->renderer.program.handle;
		glUseProgram(shaderID);

		//mat4 projection = game->cameras[game->currentCameraIndex].projectionMatrix();
		mat4 projection = Projection(&game->cameras[game->currentCameraIndex], game->config->viewPort);
		glUniformMatrix4fv(glGetUniformLocation(shaderID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		//mat4 view = game->cameras[game->currentCameraIndex].viewMatrix();
		mat4 view = ViewMatrix(&game->cameras[game->currentCameraIndex]);
		glUniformMatrix4fv(glGetUniformLocation(shaderID, "view"), 1, GL_FALSE, glm::value_ptr(view));

		DrawTileMap(game->world->map, &game->renderer.program, game->world->sheet.texture.id, game->cameras[game->currentCameraIndex].pos);

		for (unsigned int i = 1; i < game->world->entityCount; ++i)
		{
			Entity2D e = game->world->entities[i];
			MonGL::DrawObject(&game->renderer.program, &e.sprite, game->cameras[game->currentCameraIndex].pos);
		}
	}

	void SetViewPort(MonGL::Config *config, int width, int height)
	{
		config->viewPort = Rect{ 0.0f, 0.0f, (float)width, (float)height };
	}

	bool Playing(Game2D *game)
	{
		return (game->state == State::Play);
	}

	void PlayMode(Game2D *game)
	{
		game->state = State::Play;
	}

	void DebugMode(Game2D *game)
	{
		game->state = State::Debug;
	}

	void CleanUp(Game2D *game)
	{
		// TODO(ck): Empty
		return;
	}

}