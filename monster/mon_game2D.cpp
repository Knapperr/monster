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
		// TODO(ck) : memory allocation
		g_Assets = new Assets();
		InitAssets(g_Assets);
		MonGL::InitRenderer2D(&game->renderer);
		
		// TODO(ck): Memory management - allocate world
		game->world = new World2D();
		InitWorld(game->world);

		int screenWidth = 1440;
		int screenHeight = 900;
		int portWidth = 960; // 960
		int portHeight = 540; // 540
		game->config = new MonGL::Config();

		// TODO(ck): View Port in Middle of screen
		//game->config->viewPort = { (float)(screenWidth - portWidth) / 2 , (float)(screenHeight - portHeight) / 2, (float)portWidth, (float)portHeight };
		game->config->viewPort = { 0.0f , 0.0f, (float)portWidth, (float)portHeight };
		MonGL::ViewPort(&game->config->viewPort);

		AddCamera(game);
		game->currentCameraIndex = AddCamera(game);
		// InitCamera
		InitCamera(&game->cameras[game->currentCameraIndex]);


		//Entity2D* player = GetPlayer(game->world);
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
				if (input->up.endedDown) { velocity.y = 1.0f; }
				if (input->down.endedDown) { velocity.y = -1.0f; }
				if (input->left.endedDown) { velocity.x = -1.0f; }
				if (input->right.endedDown) { velocity.x = 1.0f; }
			}

			Mon::MovePlayer(game->world->map, p, &velocity, dt);			 
			Update(&game->cameras[game->currentCameraIndex], &p->pos, dt);

			// Update after camera update
			// Updating sprite information (moved to fill batch... sending tile positions to batcher)
			//p->sprite.pos = p->pos;
			//for (unsigned int i = 2; i < game->world->entityCount; ++i)
			//{
			//	Entity2D* e = &game->world->entities[i];
			//	e->sprite.pos = e->pos;
			//}

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
		

		// TODO(ck): The batch should be internal to the renderer and not really in the game layer although
		// this does make it easier... i guess you should be able to access the batch functions but the batch itself
		// you shouldn't be calling GetBatch in here... it should be in the renderer.
		// can we send a buffer to fill batch instead? and then loop internally?

		// Pre-Render Fill sprite and tile batches
		MonGL::BatchData* tileBatch = MonGL::GetBatch2D(&game->renderer, 1);
		for (int i = 0; i < game->world->map->tiles.size(); ++i)
		{
			v2 textOffset = v2(game->world->map->tiles[i]->textureOffsetX, game->world->map->tiles[i]->textureOffsetY);
			MonGL::FillBatch(tileBatch, 256.0f, 16.0f, game->world->map->tiles[i]->x, game->world->map->tiles[i]->y,
							 textOffset,
							 game->cameras[game->currentCameraIndex].pos);
		}


		MonGL::BatchData* spriteBatch = MonGL::GetBatch2D(&game->renderer, 2);
		v2 textureOffset = v2(2.0f, 7.0f);
		for (unsigned int i = 1; i < game->world->entityCount; ++i)
		{
			Entity2D e = game->world->entities[i];
			MonGL::FillBatch(spriteBatch, 256.0f, 32.0f, e.pos.x, e.pos.y,
							 textureOffset,
							 game->cameras[game->currentCameraIndex].pos);
		}

		
		// TODO(ck): Pull Texture out of sheet
		
		// Loop through batches 
		// batch needs its own shader id and texture id for the sheet
		DrawBatch(tileBatch, &game->renderer.program, game->world->sheet.texture.id, false);
		MonGL::Texture* texture = MonGL::GetTexture(&game->renderer, 17);
		DrawBatch(spriteBatch, &game->renderer.program, texture->id, false);
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