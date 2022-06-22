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
		InitAssets(g_Assets);
		MonGL::InitRenderer2D(&game->renderer);
		
		// TODO(ck): Memory management - allocate world
		game->world = new World2D();
		InitWorld(game->world);


		// Set up the shader locations for our objects
		int shaderID = game->renderer.program.handle;
		glUseProgram(shaderID);

		// TODO(CK): CAMERA
		// So let's say you want your pixel art scale 2:1
		// Then your target 1080p. Just take the resolution and divide by 2. Examples
		// This is for 1920x1080 I am using 1280x720 right now
		// 2:1 960x540 -- 3:1 640x360 --- 4:! 480x240
		float left = 0.0f;
		float right = 960.0f;
		float bottom = 540.0f;
		float top = 0.0f;

		mat4 projection = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);

		int imgLoc = glGetUniformLocation(shaderID, "image");
		glUniform1i(imgLoc, 0);

		int projLoc = glGetUniformLocation(shaderID, "projection");
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));


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
		game->cameras[game->currentCameraIndex].lerpSpeed = 7.0f;
		game->cameras[game->currentCameraIndex].smoothness = 0.24f;
		game->cameras[game->currentCameraIndex].pos = v2(0.0f);
		game->cameras[game->currentCameraIndex].vel = v2(0.0f);
		game->cameras[game->currentCameraIndex].zoom = 64.0f;

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
				velocity = v2{ input->stickAverageX, input->stickAverageY };
			}
			else
			{

				if (input->up.endedDown)
				{
#define USE_VELOCITY
#ifdef USE_VELOCITY
					velocity.y = 1.0f;
#else
					p->pos.y -= 1.0f * p->speed * dt;
					//p->pos.y -= 1.0f;
					//game->cameras[1].pos.y -= 1.0f * p->speed * dt;
#endif
				}
				if (input->down.endedDown)
				{
#ifdef USE_VELOCITY
					velocity.y = -1.0f;
#else
					p->pos.y += 1.0f * p->speed * dt;
					//p->pos.y += 1.0f;
					//game->cameras[1].pos.y += 1.0f * p->speed * dt;
#endif
				}
				if (input->left.endedDown)
				{
#ifdef USE_VELOCITY
					velocity.x = -1.0f;
#else
					p->pos.x -= 1.0f * p->speed * dt;
					//p->pos.x -= 1.0f;
					//game->cameras[1].pos.x -= 1.0f * p->speed * dt;
#endif
				}
				if (input->right.endedDown)
				{
#ifdef USE_VELOCITY
					velocity.x = 1.0f;
#else
					p->pos.x += 1.0f * p->speed * dt;
					//p->pos.x += 1.0f;
					//game->cameras[1].pos.x += 1.0f * p->speed * dt;
#endif
				}
				if (input->space.endedDown)
				{

					// GetTileChunk tile_map *tileMap, uint32 tileChunkX, uint32 tileChunkY, uint32 tileChunkZ
	//					tile_chunk* tileChunk = GetTileChunk(tileMap, chunkPos.tileChunkX, chunkPos.tileChunkY, chunkPos.tileChunkZ);

						/*tileChunk = &tileMap->tileChunks[
							tileChunkZ * tileMap->tileChunkCountY * tileMap->tileChunkCountX +
								tileChunkY * tileMap->tileChunkCountX +
								tileChunkX];*/
					int chunkCountY = 0;
					int tileIndex = 500;

					if (game->world->map->tiles[tileIndex]->id == 3)
						UpdateTile(game->world->map, &game->world->sheet, 500, 9);
					else if (game->world->map->tiles[tileIndex]->id == 9)
						UpdateTile(game->world->map, &game->world->sheet, 500, 3);
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
			game->cameras[game->currentCameraIndex].update(&p->pos, dt);

			p->sprite.pos = p->pos;


			// update minion
			Entity2D* minion = GetEntity2D(game->world, 4);
			minion->pos.x = lerp(minion->pos.x, p->pos.x, dt);
			minion->pos.y = lerp(minion->pos.y, p->pos.y, dt);
			minion->sprite.pos = minion->pos;
			//v2 windowSize = v2{ 1440, 900 };
			// screenCoord(worldCoord) .. not working
			//p->sprite.pos = v2{ p->pos.x - camera.pos.x, camera.pos.y - p->pos.y } + windowSize / 2.0f;

			//TODO(ck): This seems to be working??? I think the camera.update needs to process this though 
			// it seems that its trying the sprite position to the camera position but the camera is still "looking" at the old player position?
			// worldCoord(screenCoord) ???
			//v2 q = p->pos - windowSize / 2.0f;
			//p->sprite.pos = v2{ q.x + game->cameras[game->currentCameraIndex].pos.x, (q.y + game->cameras[game->currentCameraIndex].pos.y) };

			// Get screen coords 
			// world coord = screen coord - windowsize / 2
			// worldcoord + cam.x, -(worldcoord.y - cam.y);
			// mapping a world coordinate to 

		}
		// ending update

	}

	void Render(Game2D* game)
	{
		MonGL::ViewPort(&game->config->viewPort);

#if 0 
		// NOTE(ck): Meters to pixel conversions - this is for drawing the tilemap 
		// Im not sure if I need this.. the tilemap is drawn with the vertices built i dont think you need to convert anything? 
		// unless this gets put into the Renderer when we build vertices?

		// Getting the center and tileSide -- this is for drawing the tilemap in handmade
		int tileSideInPixels = 16;
		float tileSideInMeters = 1.6f;
		float metersToPixels = (float)tileSideInPixels / tileSideInMeters;
		
		//real32 screenCenterX = 0.5f * (real32)buffer->width;
		//real32 screenCenterY = 0.5f * (real32)buffer->height;
		int screenCenterX = 0.5f * game->config->viewPort.w;
		int screenCenterY = 0.5f * game->config->viewPort.h;
		
		v2 tileSide = { 0.5f * tileSideInPixels, 0.5f * tileSideInPixels };
		// relColumn and relRow are i&j from a loop
		// gameState->cameraP.offset_.x
		// gameState->cameraP.offset_.y
		float offsetX = 0.35f;
		float offsetY = 0.45;
		int relColumn = 1;
		int relRow = 1;
		v2 cen = { screenCenterX - metersToPixels * offsetX + ((float)relColumn) * tileSideInPixels,
				   screenCenterY + metersToPixels * offsetY - ((float)relRow) * tileSideInPixels };


		// NOTE(ck): Getting the positions and drawing for entities
		float playerGroundPointX = screenCenterX + metersToPixels * e.pos.x;
		float playerGroundPointY = screenCenterY - metersToPixels * e.pos.y;
		v2 playerLeftTop = { playerGroundPointX - 0.5f * metersToPixels * e.sprite.size.x , playerGroundPointY - 0.5f * metersToPixels * e.sprite.size.y };
		v2 playerWidthHeight = { e.sprite.size.x, e.sprite.size. };
		DrawRectangle(buffer, playerLeftTop,
					  playerLeftTop + metersToPixels * playerWidthHeight,
					  playerR, playerG, playerB);
#endif

		int shaderID = game->renderer.program.handle;
		glUseProgram(shaderID);

		mat4 projection = game->cameras[game->currentCameraIndex].projectionMatrix();
		glUniformMatrix4fv(glGetUniformLocation(shaderID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		mat4 view = game->cameras[game->currentCameraIndex].viewMatrix();
		glUniformMatrix4fv(glGetUniformLocation(shaderID, "view"), 1, GL_FALSE, glm::value_ptr(view));

		DrawTileMap(game->world->map, &game->renderer.program, game->world->sheet.texture.id, game->cameras[game->currentCameraIndex].positionOffset);

		for (unsigned int i = 1; i < game->world->entityCount; ++i)
		{
			Entity2D e = game->world->entities[i];
			//state->world->entities[i]->pos.x *= time;
			//state->world->entities[i]->pos.y *= time;
			MonGL::DrawObject(&game->renderer.program, &e.sprite, game->cameras[game->currentCameraIndex].positionOffset);
		}
		//state->world->entities[i]->pos.x *= time;
		//state->world->entities[i]->pos.y *= time;
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