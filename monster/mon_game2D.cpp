#include "mon_game2D.h"

namespace Mon {
	
	// New 
	bool Init(game_memory* memory, int windowWidth, int windowHeight, float portWidth, float portHeight)
	{
		Game2D* game = (Game2D*)memory->permanentStorage;
		if (memory->isInitialized)
			return true;
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
		//InitAssets2D(g_Assets);
		game->renderer = new MonGL::OpenGL();
		MonGL::InitRenderer2D(game->renderer);
		
		// TODO(ck): Memory management - allocate world
		game->world = new World2D();
		InitWorld(game->world);

		game->config = new MonGL::Config();
		// TODO(ck): View Port in Middle of screen
		//game->config->viewPort = { (float)(screenWidth - portWidth) / 2 , (float)(screenHeight - portHeight) / 2, (float)portWidth, (float)portHeight };
		game->config->viewPort = { 0.0f , 0.0f, portWidth, portHeight };
		MonGL::ViewPort(&game->config->viewPort);

		AddCamera(game);
		game->currentCameraIndex = AddCamera(game);
		// InitCamera
		InitCamera(&game->cameras[game->currentCameraIndex], game->config->viewPort);


		//Entity2D* player = GetPlayer(game->world);
		//game->camera = OrthoCamera(player->pos, &game->config->viewPort);
		
		// Only memory needs to be checked for init

		game->state = State::Play;
		memory->isInitialized = true;
		return memory->isInitialized;
	}

	void Update(game_memory* memory, double dt, Input* input)
	{

		Game2D *game = (Game2D*)memory->permanentStorage;
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

			//Mon::MovePlayer(game->world->map, p, &velocity, (float)dt);
			Mon::MovePlayer(p, &velocity, (float)dt);
			Update(&game->cameras[game->currentCameraIndex], p->pos, (float)dt);

			for (unsigned int i = 1; i < game->world->entityCount; ++i)
			{
				Entity2D* e = &game->world->entities[i];
				MonGL::BatchItem2D batchItem;
				if (e->isPlayer)
					batchItem.isPlayer = e->isPlayer;

				batchItem.worldPos = e->pos;
				batchItem.tileSize = 16;
				batchItem.spriteSize = 2.0f;
				batchItem.animationIndex = 1;
				game->renderer->batchItems2D.push_back(batchItem);
			}

		}
		else
		{
			// Debug updates down here?


		}
	}

	void Render(game_memory* memory, double dt)
	{
		Game2D* game = (Game2D*)memory->permanentStorage;

		MonGL::ViewPort(&game->config->viewPort);

		int shaderID = game->renderer->program.handle;
		glUseProgram(shaderID);


		mat4 projection = Projection(&game->cameras[game->currentCameraIndex]);
		glUniformMatrix4fv(glGetUniformLocation(shaderID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		mat4 view = ViewMatrix(&game->cameras[game->currentCameraIndex]);
		glUniformMatrix4fv(glGetUniformLocation(shaderID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		
		// TODO(ck): The batch should be internal to the renderer and not really in the game layer although
		// this does make it easier... i guess you should be able to access the batch functions but the batch itself
		// you shouldn't be calling GetBatch in here... it should be in the renderer.
		// can we send a buffer to fill batch instead? and then loop internally?

		// Pre-Render Fill sprite and tile batches

		MonGL::BatchData* tileBatch = MonGL::GetBatch2D(game->renderer, 1);
		for (int i = 0; i < game->world->map->tiles.size(); ++i)
		{
			v2 textOffset = v2(game->world->map->tiles[i]->textureOffsetX, game->world->map->tiles[i]->textureOffsetY);
			MonGL::FillBatch(tileBatch, 256.0f, 16, 1.0f, game->world->map->tiles[i]->x, game->world->map->tiles[i]->y,
							 textOffset,
							 game->cameras[game->currentCameraIndex].pos);
		}


		MonGL::BatchData* spriteBatch = MonGL::GetBatch2D(game->renderer, 2);

		MonGL::SortBatch2D(game->renderer->batchItems2D);

		MonGL::GLSpriteAnimator* animator = &game->renderer->spriteAnimators[1];
		MonGL::GLSpriteAnimation* animation = &animator->animations[0];
		//MonGL::UpdateSpriteAnimation(animation, 1, dt);
		
		MonGL::GLSubTexture* subTexture = &animation->frames[animation->frameIndex].subTexture;
		for (int i = 0; i < game->renderer->batchItems2D.size(); ++i)
		{
			MonGL::BatchItem2D item = game->renderer->batchItems2D[i];
			if (item.isPlayer)
			{
				MonGL::GLSpriteAnimator* walkAnim = &game->renderer->spriteAnimators[2];
				MonGL::GLSpriteAnimation* anim = &walkAnim->animations[0];
				MonGL::UpdateSpriteAnimation(anim, 1, (float)dt);

				MonGL::GLSubTexture* walkSubText = &anim->frames[anim->frameIndex].subTexture;//&animator->animations[item.animationIndex].frames[state->selectedSubTextureIndex].subTexture;
				MonGL::FillBatch(spriteBatch, 256.0f, item.tileSize, item.spriteSize, item.worldPos.x, item.worldPos.y,
								 *walkSubText,
								 game->cameras[game->currentCameraIndex].pos);
				continue;
			}

			MonGL::FillBatch(spriteBatch, 256.0f, item.tileSize, item.spriteSize, item.worldPos.x, item.worldPos.y,
							 *subTexture,
							 game->cameras[game->currentCameraIndex].pos);
		}

		
		// TODO(ck): Pull Texture out of sheet
		
		// Loop through batches 
		// batch needs its own shader id and texture id for the sheet
		MonGL::Texture* tilemapAtlas = MonGL::GetTexture(game->renderer, 19);
		DrawBatch(tileBatch, &game->renderer->program, tilemapAtlas->id, false);

		MonGL::Texture* spriteAtlas = MonGL::GetTexture(game->renderer, 17);
		DrawBatch(spriteBatch, &game->renderer->program, spriteAtlas->id, false);
	
		EndRender(game->renderer);
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