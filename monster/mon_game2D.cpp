#include "mon_game2D.h"

namespace Mon {
	
	// New 
	void Init(Game2D_* game)
	{
		InitWorld(game->world);

		MonGL::LoadShader(&game->shader, "res/shaders/vert_sprite.glsl", "res/shaders/frag_sprite.glsl", NULL);
		// TODO(ck): REMOVE TESTING TILE SHADER use above shader for both tiles and quads
		MonGL::LoadShader(&game->tileShader, "res/shaders/vert_tile.glsl", "res/shaders/frag_tile.glsl", NULL);

		
	}


	bool Game2D::init(int x)
	{
		state = State::Play;

		// TODO(ck): Memory management
		world = new World2D();
		MonGL::LoadShader(&shader, "res/shaders/vert_sprite.glsl", "res/shaders/frag_sprite.glsl", NULL);
		// TODO(ck): REMOVE TESTING TILE SHADER
		MonGL::LoadShader(&tileShader, "res/shaders/vert_tile.glsl", "res/shaders/frag_tile.glsl", NULL);

		// Set up the shader locations for our objects
		glUseProgram(shader.handle);

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

		int imgLoc = glGetUniformLocation(shader.handle, "image");
		glUniform1i(imgLoc, 0);

		int projLoc = glGetUniformLocation(shader.handle, "projection");
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));


		// TODO(ck): REMOVE TESTING TILE SHADER
		glUseProgram(tileShader.handle);
		imgLoc = glGetUniformLocation(tileShader.handle, "image");
		glUniform1i(imgLoc, 0);

		projLoc = glGetUniformLocation(tileShader.handle, "projection");
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		mat4 model = mat4(1.0f);
		v3 pos = v3(0, 0, 1.0f);
		model = glm::translate(model, pos);
		model = glm::scale(model, v3(v2(32, 32), 1.0f));
		glUniformMatrix4fv(glGetUniformLocation(tileShader.handle, "model"), 1, GL_FALSE, glm::value_ptr(model));


		int screenWidth = 1440;
		int screenHeight = 900;
		int portWidth = 960;
		int portHeight = 540;
		config = new MonGL::Config();

		config->viewPort = { (float)(screenWidth - portWidth) / 2 , (float)(screenHeight - portHeight) / 2, (float)portWidth, (float)portHeight };
		MonGL::ViewPort(&config->viewPort);

		camera = OrthoCamera(world->player->pos, &config->viewPort);
		return true;
	}

	void Game2D::update(double dt, Input* input, int x)
	{
		if (dt > deltaTime || dt < deltaTime)
			printf("dt: %f\n", dt);

		deltaTime = dt;
		this->input = *input;

		if (state == State::Play)
		{
			Entity2D* p = world->player;
			v2 velocity = {};

			if (input->isAnalog)
			{
				velocity = v2{ input->stickAverageX, input->stickAverageY };
			}
			else
			{

				if (input->up.endedDown)
				{
#if 0
					velocity.y = -1.0f;
#else
					p->pos.y -= 1.0f * p->speed * dt;
#endif
				}
				if (input->down.endedDown)
				{
#if 0
					velocity.y = 1.0f;
#else
					p->pos.y += 1.0f * p->speed * dt;
#endif
				}
				if (input->left.endedDown)
				{
#if 0
					velocity.x = -1.0f;
#else
					p->pos.x -= 1.0f * p->speed * dt;
#endif
				}
				if (input->right.endedDown)
				{
#if 0
					velocity.x = 1.0f;
#else
					p->pos.x += 1.0f * p->speed * dt;
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

					if (world->map->tiles[tileIndex]->id == 3)
						UpdateTile(world->map, &world->sheet, 500, 9);
					else if (world->map->tiles[tileIndex]->id == 9)
						UpdateTile(world->map, &world->sheet, 500, 3);
				}
			}
			//Mon::movePlayer(world->map, p, &velocity, deltaTime);

			// TODO(ck): link sprite position to camera...  
			// https://www.reddit.com/r/gamedev/comments/7cnqpg/lerping_camera_position_causes_jitters_as_it/
			// update sprite position 

			// this one is better explanation for why you shouldn't tie camera to player position and only to an offset
			//https://gamedev.stackexchange.com/questions/2642/scrolling-2d-sprites-on-a-map-with-a-camera

			// positions should be relative to the map the camera shouldn't come into play with real positions..
			camera.update(&p->pos, deltaTime);



			// TODO(ck): These do not work but they prove that the camera is causing the jitter to happen.
			// the player must be drawn relative to camera same with everything else
#if 0
			int tileSize = 16;
			int visibleTilesX = 960 / tileSize;
			int visibleTilesY = 540 / tileSize;
			float offsetX = camera.pos.x - (float)visibleTilesX / 2.0f;
			float offsetY = camera.pos.y - (float)visibleTilesY / 2.0f;
			p->sprite.pos.x = (p->pos.x - offsetX) * tileSize;
			p->sprite.pos.y = (p->pos.y - offsetY) * tileSize;
#else
			p->sprite.pos = p->pos;
#endif

			v2 windowSize = v2{ config->viewPort.w, config->viewPort.h };

			// screenCoord(worldCoord) .. not working
			//p->sprite.pos = v2{ p->pos.x - camera.pos.x, camera.pos.y - p->pos.y } + windowSize / 2.0f;

			//TODO(ck): This seems to be working??? I think the camera.update needs to process this though 
			// it seems that its trying the sprite position to the camera position but the camera is still "looking" at the old player position?
			// worldCoord(screenCoord) ???
			//v2 q = p->pos - windowSize / 2.0f;
			//p->sprite.pos = v2{ q.x + camera.pos.x, (q.y + camera.pos.y) };

			// Get screen coords 
			// world coord = screen coord - windowsize / 2
			// worldcoord + cam.x, -(worldcoord.y - cam.y);
			// mapping a world coordinate to 

		}
		// ending Game::update
	}

	void Game2D::render()
	{
		MonGL::ViewPort(&config->viewPort);

		// test stack
		//MonGL::PushMatrix(&batch, camera.projectionMatrix());

		//	mat4 newerMat = mat4(10.0f);
		//	MonGL::PushMatrix(&batch, newerMat);
		//	MonGL::PopMatrix(&batch);

		//	mat4 newMat = mat4(2.0f);
		//	MonGL::PushMatrix(&batch, newMat);

		//MonGL::PopMatrix(&batch);

		// TEST DRAW
		glUseProgram(tileShader.handle);
		int projLoc = glGetUniformLocation(tileShader.handle, "projection");
		mat4 projection = camera.projectionMatrix();
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		//MonGL::drawMap(&tileShader, world->sheet.texture.id);
		DrawTileMap(world->map, &tileShader, world->sheet.texture.id);


		glUseProgram(shader.handle);
		projLoc = glGetUniformLocation(shader.handle, "projection");
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		for (unsigned int i = 0; i < world->entities.size(); ++i)
		{
			//state->world->entities[i]->pos.x *= time;
			//state->world->entities[i]->pos.y *= time;
			MonGL::DrawObject(&shader, &world->entities[i]->sprite);
		}

		MonGL::DrawObject(&shader, &world->player->sprite);

	}

	///
	/// Utility 
	///

	void Game2D::cleanUp()
	{
		// glDeleteVertexArrays()???? this is in the 3D stuff
		MonGL::DeleteShader(&shader);
	}

	void Game2D::setViewPort(int width, int height)
	{
		config->viewPort = Rect{ 0.0f, 0.0f, (float)width, (float)height };
	}

	bool Game2D::playing()
	{
		return (state == State::Play);
		// checking 3d fly camera
		//return (state == State::Play && cam->follow == true);
	}

	void Game2D::playMode()
	{
		state = State::Play;
		//cam->followOn();
	}

	void Game2D::debugMode()
	{
		state = State::Debug;
		//cam->followOff();
	}
}