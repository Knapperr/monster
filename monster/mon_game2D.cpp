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

		// InitCamera
		InitCamera(&game->camera, game->config->viewPort);

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
			v3 aMin = v3(p->pos.x - 0.5f, p->pos.y - 0.5f, 0.0f);
			v3 aMax = v3(p->pos.x + 0.5f, p->pos.y + 0.5f, 0.0f);
			AABB a = {};
			a.min = aMin;
			a.max = aMax;
			bool canMove = true;
			p->colour = v4(1.0f);
			for (unsigned int i = 1; i < game->world->entityCount; ++i)
			{
				// TODO(ck): Broad Phase Collision Check

				// TODO(ck): Precise Collision check

				if (i != 1) // 1 is player index
				{
					Entity2D* testEntity = GetEntity2D(game->world, i);
					testEntity->colour = v4(1.0f);

					AABB b = {};
					v3 bMin = v3(testEntity->pos.x - 0.5f, testEntity->pos.y - 0.5f, 0.0f);
					v3 bMax = v3(testEntity->pos.x + 0.5f, testEntity->pos.y + 0.5f, 0.0f);
					b.min = bMin;
					b.max = bMax;

					float tFirst = 0.0f;
					float tLast = 0.0f;
					if (Mon::InterestMovingAABB(a, b, v3(velocity, 0.0f), v3(0.0f), tFirst, tLast))
					{
						//newPos = player->rb.worldPos + 0.05f;
						//newPos.y = 0.0f;
						// 

						//canMove = false;
						//p->pos -= 0.25f;
						testEntity->colour = v4(1.0f, 0.2f, 0.1f, 1.0f);
						//break; // break on first seen collision but leave off for testing to see colour
					}
					//if (Mon::TestAABB(aMin, aMax, bMin, bMax))
					//{
					//	//newPos = player->rb.worldPos + 0.05f;
					//	//newPos.y = 0.0f;
					//	// 
					//	
					//	//canMove = false;
					//	//p->pos -= 0.25f;
					//	testEntity->colour = v4(1.0f, 0.2f, 0.1f, 1.0f);
					//	//break; // break on first seen collision but leave off for testing to see colour
					//}
				}
			}
			if (canMove)
			{
				//Mon::MovePlayer(game->world->map, p, &velocity, (float)dt);
				
				// COLLISION TEST 
				//Entity otherEnt = world->entities[3];
				//float tFirst = 0.0f;
				//float tLast = 0.0f;

				//AABB a = player->bbox;
				//AABB b = otherEnt.bbox;
				////otherEnt.rb.velocity = v3(1.0f);

				////bool colliding = InterestMovingAABB(a, b, player->rb.velocity, otherEnt.rb.velocity, tFirst, tLast);
				//bool colliding = InterestMovingAABB(a, b, player->rb.velocity, otherEnt.rb.velocity, tFirst, tLast);

				////Mon::Log::print("tFirst:", tFirst);
				////Mon::Log::print("tLast:", tLast);
				//player->rb.velocity.x = velocity->x * dt + player->rb.velocity.x;
				//player->rb.velocity.z = velocity->z * dt + player->rb.velocity.z;
				//return 1;


				//Mon::MovePlayer(p, &velocity, (float)dt);
				//v2 oldPos = p->pos;

				//velocity = velocity * p->speed;
				//p->pos += velocity * v2(dt,dt);
				
				//p->pos.x += (velocity.x * p->speed * dt);
				//p->pos.y += (velocity.y * p->speed * dt);
				float speed = p->speed * dt;
				p->pos += velocity * speed;
			}
				
			v2 ePos = game->world->entities[2].pos;
				// p->pos
			Update(&game->camera, p->pos, (float)dt);

			// Get the mouse position in tile coordinates after updating the camera

			// Need camera
			// Clamp the position to the inside of the tilemap...
			// convert screen to world...
			// world to tile???
			Mon::v2 window = { 1440.0f, 900.0f };
			Mon::v2 port = { 960.0f, 540.0f };
			Mon::v2 offset = (window - port); // port.x & y = 5

			// TODO(ck): Figure out offset. Working for y but not for x 
			// STUDY(ck): 
			// x only needs to account for the offset x position of the port but
			// the y position needs to subtract the offset of the window and port as well
			// but x doesn't seem to need this..?
			float x = (2.0f * (input->mouseScreen.x)) / port.x - 1.0f;
			float y = (2.0f * (input->mouseScreen.y - offset.y)) / port.y - 1.0f;

			// NOTE(ck): Make sure to reverse the direction of y!
			v2 normalizedCoords = {x, -y};
			Mon::v4 clipCoords = Mon::v4(normalizedCoords.x, normalizedCoords.y, -1.0f, 1.0f);
			Mon::v4 eyeCoords = glm::inverse(Mon::Projection(&game->camera)) * clipCoords;
			Mon::v4 finalEyeCoords = Mon::v4(eyeCoords.x, eyeCoords.y, -1.0f, 0.0f);

			Mon::v4 rayWorld = glm::inverse(Mon::ViewMatrix(&game->camera)) * eyeCoords;
			Mon::v3 mouseRay = Mon::v3(rayWorld.x, rayWorld.y, rayWorld.z);
			mouseRay = glm::normalize(mouseRay);

			game->mouseTilePos = v2{ (mouseRay.x), (mouseRay.y) };



		}
		else
		{
			// Debug updates down here?


		}
	}

	void Render(game_memory* memory, double dt)
	{
		Game2D* game = (Game2D*)memory->permanentStorage;

		//MonGL::ViewPort(&game->config->viewPort);


		// Can I do collision down here?
		//game->renderer->batchItems2D.clear();
		//ResetBuffer(&game->renderer->lineBuffer);
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

			glm::vec4 col = e->colour;
			glm::vec4 min = v4(e->pos.x - 0.5f, e->pos.y - 0.5f, 0.0f, 1.0f);
			glm::vec4 max = v4(e->pos.x + 0.5f, e->pos.y + 0.5f, 0.0f, 1.0f);

			MonGL::DrawBox2D(&game->renderer->lineBuffer, min, max, col);
		}

		v4 minTest = v4(0.0f, 0.0f, 0.0f, 1.0f);
		v4 maxTest = v4(1.0f, 1.0f, 0.0f, 1.0f);
		v4 colTest = v4(1.0f);

		MonGL::DrawBox2D(&game->renderer->lineBuffer, minTest, maxTest, colTest);


		glBindFramebuffer(GL_FRAMEBUFFER, game->renderer->fbo);
		int shaderID = game->renderer->program.handle;
		glUseProgram(shaderID);
		// TODO(ck): Pull Texture out of sheet
		v3 clearColour = v3(0.0f, 0.0f, 0.0f);
		glClearColor(clearColour.r, clearColour.g, clearColour.b, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_BLEND);

		mat4 projection = Projection(&game->camera);
		glUniformMatrix4fv(glGetUniformLocation(shaderID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		mat4 view = ViewMatrix(&game->camera);
		glUniformMatrix4fv(glGetUniformLocation(shaderID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		
		// TODO(ck): The batch should be internal to the renderer and not really in the game layer although
		// this does make it easier... i guess you should be able to access the batch functions but the batch itself
		// you shouldn't be calling GetBatch in here... it should be in the renderer.
		// can we send a buffer to fill batch instead? and then loop internally?

		// Pre-Render Fill sprite and tile batches

		MonGL::BatchData* tileBatch = MonGL::GetBatch2D(game->renderer, 0);
		for (int i = 0; i < game->world->map->tiles.size(); ++i)
		{
			v2 textOffset = v2(game->world->map->tiles[i]->textureOffsetX, game->world->map->tiles[i]->textureOffsetY);
			MonGL::FillBatch(tileBatch, 256.0f, 16, game->world->map->tiles[i]->x, game->world->map->tiles[i]->y,
							 textOffset,
							 game->camera.pos);
		}

		v4 colour = v4(1.0f);
		// 0 - 1 is world space or model space??? i think my world space is technically the 
		// i want my world space to go 0 1 2 or at least tile space?
		// draw or pixel would be 0.0 then 16.0f for the 
		MonGL::DrawLine2D(&game->renderer->lineBuffer, v4(0.0f, 0.0f, 0.0f, 1.0f), v4(1.0f, 0.0f, 0.0f, 1.0f), colour);
		MonGL::DrawLine2D(&game->renderer->lineBuffer, v4(2.0f, 0.0f, 0.0f, 1.0f), v4(4.0f, 0.0f, 0.0f, 1.0f), colour);
		MonGL::DrawLine2D(&game->renderer->lineBuffer, v4(0.0f, 5.0f, 0.0f, 1.0f), v4(0.0f, 8.0f, 0.0f, 1.0f), colour);

		colour = v4(0.5f, 0.2f, 0.1f, 1.0f);
		MonGL::DrawLine2D(&game->renderer->lineBuffer, 
						  v4(game->world->entities[1].pos.x, game->world->entities[1].pos.y, 0.0f, 1.0f),
						  v4(game->world->entities[2].pos.x, game->world->entities[2].pos.y, 0.0f, 1.0f), colour);

		// IMPORTANT(ck): TODO(ck): Move all of this into Render2D() method

		MonGL::BatchData* spriteBatch = MonGL::GetBatch2D(game->renderer, 1);

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
				MonGL::FillBatch(spriteBatch, 256.0f, item.tileSize, item.worldPos.x, item.worldPos.y, *walkSubText);
				continue;
			}
			if (i == 3)
				subTexture = &animation->frames[3].subTexture;

			
			MonGL::FillBatch(spriteBatch, 256.0f, item.tileSize, item.worldPos.x, item.worldPos.y, *subTexture);
		}


		// Loop through batches 
		// batch needs its own shader id and texture id for the sheet
		MonGL::Texture* tilemapAtlas = MonGL::GetTexture(game->renderer, 18);
		DrawBatch(tileBatch, &game->renderer->program, tilemapAtlas->id, false);

		MonGL::Texture* spriteAtlas = MonGL::GetTexture(game->renderer, 16);
		DrawBatch(spriteBatch, &game->renderer->program, spriteAtlas->id, false);


	

		// Render framebuffer texture
#if 1
		// Bind the default frame buffer now
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST); // do not discard the screen quad
		// clear colour to white but unecessary so why do it???? ASK JASPER
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(game->renderer->frameBufferProgram.handle);
		//glUniform1f(glGetUniformLocation(game->renderer->frameBufferProgram.handle, "brightness"), state.framebufferBrightness);


		glBindVertexArray(game->renderer->screenVAO);
		glBindTexture(GL_TEXTURE_2D, game->renderer->textureColorbuffer);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
#endif



		if (game->drawDebug)
		{
			// maybe i need to disable depth
			//glEnable(GL_BLEND);
			// debugProgram.handle;
			glUseProgram(game->renderer->debugProgram.handle);
			//glDisable(GL_DEPTH_TEST);
			//glDisable(GL_CULL_FACE);

			//glClearColor(state.clearColour.r, state.clearColour.g, state.clearColour.b, 1.0f);
			//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			//glEnable(GL_MULTISAMPLE);
			//glm::mat4 comboMat = projection * view;
			//glm::mat4 toWorldFromClip = glm::inverse(comboMat);
			glUniformMatrix4fv(glGetUniformLocation(game->renderer->debugProgram.handle, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
			glUniformMatrix4fv(glGetUniformLocation(game->renderer->debugProgram.handle, "view"), 1, GL_FALSE, glm::value_ptr(view));

			// update the aabb batch
			glBindVertexArray(game->renderer->lineBuffer.VAO);

			// upload the batch and reset its offsets and lastvertice and lastindice indexes
			glNamedBufferSubData(game->renderer->lineBuffer.VBO, 0, sizeof(MonGL::DebugVertex) * (game->renderer->lineBuffer.lastVerticeIndex), game->renderer->lineBuffer.vertices);
			glNamedBufferSubData(game->renderer->lineBuffer.IBO, 0, sizeof(unsigned int short) * (game->renderer->lineBuffer.lastIndiceIndex), game->renderer->lineBuffer.indices);

			glDrawElements(GL_LINES, game->renderer->lineBuffer.usedIndices, GL_UNSIGNED_SHORT, nullptr);

			glBindVertexArray(0);
		}

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

		/*
		------------
		 game world
		------------
		clean entities


		------------
		  renderer
		------------
		clean batches
		opengl objects
		

		*/
		
		return;
	}

}