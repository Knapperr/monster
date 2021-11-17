#include "mon_game.h"

#include "handmade_random.h"

namespace Mon
{

	/*
	GenerateShader("waterDis",
			"shaders/waterdistortion_vert.glsl",
			"shaders/waterdistortion_frag.glsl",
			NULL);

	GenerateShader("waterDir",
				"shaders/waterdirection_vert.glsl",
				"shaders/waterdirection_frag.glsl",
				NULL);
	void LoadDistortedWater(WaterObject* water)
	{
		water->tiling = 5.0f;
		water->speed = 0.3f;
		water->flowStrength = 0.05f;
		water->flowOffset = -0.23f;
		water->heightScale = 0.1f;
		water->heightScaleModulated = 8.0f;

		// CREATE A BASIC SHAPE LOADER replace ASSIMP
		// WE WILL HAVE TO PUSH THIS TEXTURE TO THE TEXUTES OF THE QUAD 
		// IN model.h
		std::string uvpath = "content/textures/water/water.png";
		std::string uvdirectory = uvpath.substr(0, uvpath.find_last_of('/'));
		Texture uvtexture = {};
		uvtexture.id = LoadTextureFile("water.png", uvdirectory);
		uvtexture.type = "texture_diffuse";
		uvtexture.path = uvpath;

		std::string flowpath = "content/textures/water/flow-speed-noise.png";
		std::string flowdirectory = flowpath.substr(0, flowpath.find_last_of('/'));
		Texture flowtexture = {};
		flowtexture.id = LoadTextureFile("flow-speed-noise.png", flowdirectory);
		flowtexture.type = "texture_normal";
		flowtexture.path = flowpath;

		std::string normalpath = "content/textures/water/water-derivative-height.png";
		std::string normaldir = normalpath.substr(0, normalpath.find_last_of('/'));
		Texture normaltexture = {};
		normaltexture.id = LoadTextureFile("water-derivative-height.png", normaldir);
		normaltexture.type = "texture_normal";
		normaltexture.path = normalpath;

		water->model->meshes[0].textures.push_back(uvtexture);
		water->model->meshes[0].textures.push_back(flowtexture);
		water->model->meshes[0].textures.push_back(normaltexture);
	}


	*/

	bool Game::init(int windowWidth, int windowHeight)
	{
		state = State::Debug;

		shader = {};
		waterShader = {};
		MonGL::LoadShader(&shader, "res/shaders/vert_colors.glsl", "res/shaders/frag_colors.glsl", NULL);
		MonGL::LoadShader(&waterShader.common, "res/shaders/vert_water.glsl", "res/shaders/frag_water.glsl", NULL);

		mainShaderID = shader.handle;
		



		// TODO(ck): MEMORY MANAGEMENT
		world = new World();
		initWorld(world, shader.handle);
		initPlayer(world, shader.handle);

		light = {};
		v3 lightColor = v3(0.2f, 0.3f, 0.6f);
		light.diffuse = lightColor * v3(0.5f);
		light.ambient = light.diffuse * v3(0.2f);
		light.specular = v3(1.0f, 1.0f, 1.0f);
		light.pos = v3(16.0f, 10.0f, 16.0f);


		//water = {};
		//MonGL::initDistortedWater(&water.data, &water.setup);
		
		//MonGL::initDistortedWater(water.data, wa)
		//MonGL::initQuad(&water.data);
		//MonGL::loadTexture(&water.data, MonGL::Type::Diffuse, shader.handle, "res/textures/water/uv.png");
		//MonGL::initBoundingBox(&water.colliderData);
		//water.particle.pos = v3(40.0f, 0.1f, 10.0);
		//water.particle.orientation = v3(1.0f, 1.0f, 1.0);

		// TODO(ck): Memory Allocation
		terrain = new Terrain(0, 0);
		MonGL::generateTerrain(&terrain->mesh);
		//terrain->generate();

		simulate = false;
		
		config = new MonGL::Config();
		int portWidth = 960;
		int portHeight = 540;
		config->viewPort.x = (windowWidth - portWidth) / 2;
		config->viewPort.y = (windowHeight - portHeight) / 2;
		config->viewPort.w = portWidth;
		config->viewPort.h = portHeight;
		MonGL::viewPort(&config->viewPort);
		Mon::Log::print("port width, height", portWidth, portHeight);

		config->angleDegrees = -30.0f;

		// TODO(ck): Memory management
		cam = new Camera(config->viewPort);

		selectedIndex = 0;
		drawCollisions = true;

		return true;
	}

	v2 getNormalizedDeviceCoords(v2 mouse, Rect viewPort, v2 window)
	{
		v2 port = v2(viewPort.w, viewPort.h);
		v2 offset = (window - port) / 2.0f;
		float x = (2.0f * (mouse.x + offset.x)) / (float)viewPort.w - 1.0f;
		float y = (2.0f * (mouse.y - offset.y)) / (float)viewPort.h - 1.0f;
		return v2(x, -y);
	}

	void updateMousePicker()
	{

	}

	// TODO(ck): Should this param be pointer?
	// should velocity be acceleratio?
	void Game::movePlayer(v3* velocity)
	{
		// ddPLength
		float velocityLength = lengthSq(*velocity);
		if (velocityLength > 1.0f)
		{
			*velocity *= (1.0f / squareRoot(velocityLength));
		}
		*velocity *= world->player->particle.speed;

		*velocity += -7.0f * world->player->particle.velocity;

		
		v3 oldPos = world->player->particle.pos;
		v3 newPos = oldPos;
		float deltaX = (0.5f * velocity->x * square(deltaTime) + world->player->particle.velocity.x * deltaTime);
		float deltaY = world->player->particle.velocity.y;
		//float deltaY = velocity->y * square(deltaTime) + player.particle.velocity.y * deltaTime);
		float deltaZ = (0.5f * velocity->z * square(deltaTime) + world->player->particle.velocity.z * deltaTime);
		v3 delta = { deltaX, deltaY, deltaZ };

		// TODO(ck): need to set an offest like casey does
		newPos += delta;


		world->player->particle.velocity.x = velocity->x * deltaTime + world->player->particle.velocity.x;
		world->player->particle.velocity.z = velocity->z * deltaTime + world->player->particle.velocity.z;

		world->player->particle.pos = newPos;


		if ((world->player->particle.velocity.x == 0.0f) && (world->player->particle.velocity.z == 0.0f))
		{
			// NOTE(ck): Leave facingDirection whatever it was 
		}
		else if (absoluteValue(world->player->particle.velocity.x) > absoluteValue(world->player->particle.velocity.z))
		{
			if (world->player->particle.velocity.x > 0)
			{
				world->player->facingDir = 0; // right
			}
			else
			{
				world->player->facingDir = 2; // left
			}
		}
		else
		{
			if (world->player->particle.velocity.z > 0)
			{
				world->player->facingDir = 1; // back

			}
			else
			{
				world->player->facingDir = 3; // front
			}

		}
	}

	void Game::update(double dt, Input* newInput)
	{
		if (dt > deltaTime)
			printf("HIGHER dt: %f\n", deltaTime);
		if (dt < deltaTime)
			printf("LOWER dt: %f\n", deltaTime);

		deltaTime = dt;
		input = *newInput;

		//
		// Input start
		//
		// TODO(ck): TEMP INPUT
		if (input.debug.endedDown)
		{
			if (state == State::Debug)
			{
				playMode();
			}
			else if (state == State::Play)
			{
				debugMode();
			}
		}
		
		
		bool enabled = (state == State::Play);
		if (enabled)
		{
			v3 velocity = {};

			if (input.isAnalog)
			{
				velocity = v3{ input.stickAverageX, 0.0f ,input.stickAverageY };
			}
			else
			{
				if (input.up.endedDown)
					velocity.z = -1.0f;
				if (input.down.endedDown)
					velocity.z = 1.0f;
				if (input.left.endedDown)
					velocity.x = -1.0f;
				if (input.right.endedDown)
					velocity.x = 1.0f;
				if (input.space.endedDown)
					velocity.y = 1.0f;
			}
			// PIPE velocity to function
			movePlayer(&velocity);
		}
		//player.particle.velocity *= player.particle.speed;
		//player.particle.pos.x += player.particle.velocity.x * dt;
		//player.particle.pos.z += player.particle.velocity.z * dt;
		
		//input.mouseOffset.x -= config->viewPort.x;
		//input.mouseOffset.y -= config->viewPort.y;
		//int xOffset = (1440 - 960) / 2;
		//int yOffset = (900 - 540) / 2;

		//float x = (2.0f * (mouseX)) / (float)displayWidth - 1.0f;
		//float y = (2.0f * (mouseY - yOffset)) / (float)displayHeight - 1.0f;
		//input.mouseOffset.x -= config->viewPort.x;
		//input.mouseOffset.y -= config->viewPort.y;
		cam->update(deltaTime, &input, world->player->particle.pos, world->player->particle.orientation, true);

		//if (simulate == true)
			//player.particle.integrate(deltaTime);


		// TODO(ck): Give player pos matrix
		mat4 model = mat4(1.0f);
		float colliderPosX = world->player->particle.pos.x - (0.5f);
		float colliderPosZ = world->player->particle.pos.z - (0.5f);
		world->player->collider.data.worldMatrix = translate(model, v3(colliderPosX, -0.2f, colliderPosZ));
	}

	void Game::render(double dt)
	{
		//projection = glm::perspective(glm::radians(cam.zoom), 1440.0f / 720.0f, 0.1f, 100.0f);
		mat4 proj = cam->projection();
		mat4 view = cam->viewMatrix();

		// TODO(ck): pass all shaders to beginRender or they live in the opengl layer and get
		// activated that way
		MonGL::beginRender(config, proj, view, shader.handle);
		MonGL::beginRender(config, proj, view, waterShader.common.handle);

		MonGL::drawTerrain(shader.handle, &terrain->mesh, &light, proj, view, cam->pos);

		// TODO(ck): use shader
		glUseProgram(shader.handle);
		// light & material
		glUniform3fv(glGetUniformLocation(shader.handle, "light.pos"), 1, &light.pos[0]);
		glUniform3fv(glGetUniformLocation(shader.handle, "light.ambient"), 1, &light.ambient[0]);
		glUniform3fv(glGetUniformLocation(shader.handle, "light.diffuse"), 1, &light.diffuse[0]);
		glUniform3fv(glGetUniformLocation(shader.handle, "light.specular"), 1, &light.specular[0]);


		if (drawCollisions)
			MonGL::drawBoundingBox(&world->player->collider.data, world->player->collider.size, world->player->particle.pos, cam->pos, proj, view, shader.handle);

		MonGL::drawQuad(config, &world->player->data, world->player->particle.pos, v3(1.0f), cam->pos, shader.handle, world->player->facingDir);

		//for (auto& e : enemies)
		//{
		//	MonGL::drawBoundingBox(&e.collider.data, e.collider.size, e.particle.pos, cam->pos, proj, view, shader.handle);
		//}

		for (int i = 1; i < world->entityCount; ++i)
		{
			Entity e = world->entities[i];
			MonGL::drawQuad(config, &e.data, e.particle.pos, v3(16.0f, 16.0f, 1.0f), cam->pos, shader.handle);
		}
		
		//glUseProgram(waterShader.common.handle);
		//MonGL::drawWater(&water.data, &water.setup, &waterShader, &light, water.particle.pos, v3(10.0f), cam->pos, waterShader.common.handle);
		//MonGL::drawQuad(config, &water.data, water.particle.pos, v3(5.0f), cam.pos, shader.handle);

	}

	void Game::cleanUp()
	{
		// TODO(ck): clean up render data 
		glDeleteVertexArrays(1, &world->player->data.VAO);
		glDeleteBuffers(1, &world->player->data.VBO);

		MonGL::DeleteShader(&shader);
	}

	void Game::setViewPort(int width, int height)
	{
		config->viewPort = Rect{ 0.0f, 0.0f, (float)width, (float)height };
	}

	bool Game::playing()
	{
		return (state == State::Play && cam->follow == true);
	}

	void Game::playMode()
	{
		state = State::Play;
		cam->followOn();
	}

	void Game::debugMode()
	{
		state = State::Debug;
		cam->followOff();
	}

	/// 
	/// 2D
	/// 

	bool Game::init(int x)
	{
		state = State::Play;

		// TODO(ck): Memory management
#ifndef _3D_
		world2D = new World2D();
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

		config->viewPort.x = (float)(screenWidth - portWidth) / 2;
		config->viewPort.y = (float)(screenHeight - portHeight) / 2;
		config->viewPort.w = portWidth;
		config->viewPort.h = portHeight;
		MonGL::viewPort(&config->viewPort);

		camera = OrthoCamera(world2D->player->pos, &config->viewPort);
#endif
		return true;
	}

	void Game::update(double dt, Input* input, int x)
	{
		if (dt > deltaTime || dt < deltaTime)
			printf("dt: %f\n", dt);

#ifndef _3D_

		deltaTime = dt;
		this->input = *input;
		
		if (state == State::Play)
		{
			Entity2D* p = world2D->player;
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

					if (world2D->map->tiles[tileIndex]->id == 3)
						updateTile(world2D->map, &world2D->sheet, 500, 9);
					else if (world2D->map->tiles[tileIndex]->id == 9)
						updateTile(world2D->map, &world2D->sheet, 500, 3);
				}
			}
			Mon::movePlayer(world2D->map, p, &velocity, deltaTime);
			 
			// TODO(ck): link sprite position to camera...  
			// https://www.reddit.com/r/gamedev/comments/7cnqpg/lerping_camera_position_causes_jitters_as_it/
			// update sprite position 
			
			// this one is better explanation for why you shouldn't tie camera to player position and only to an offset
			//https://gamedev.stackexchange.com/questions/2642/scrolling-2d-sprites-on-a-map-with-a-camera

			// positions should be relative to the map the camera shouldn't come into play with real positions..
			camera.update(&p->pos, deltaTime);



			// TODO(ck): These do not work but they prove that the camera is causing the jitter to happen.
			// the player must be drawn relative to camera same with everything else
			p->sprite.pos = p->pos;
			
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
#endif
		// ending Game::update
	}

	void Game::render()
	{
#ifndef _3D_
		MonGL::viewPort(&config->viewPort);

		// TEST DRAW
		glUseProgram(tileShader.handle);
		int projLoc = glGetUniformLocation(tileShader.handle, "projection");
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(camera.projectionMatrix()));

		//MonGL::drawMap(&tileShader, world->sheet.texture.id);
		drawTileMap(world2D->map, &tileShader, world2D->sheet.texture.id);


		glUseProgram(shader.handle);
		projLoc = glGetUniformLocation(shader.handle, "projection");
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(camera.projectionMatrix()));

		for (unsigned int i = 0; i < world2D->entities.size(); ++i)
		{
			//state->world->entities[i]->pos.x *= time;
			//state->world->entities[i]->pos.y *= time;
			MonGL::drawObject(&shader, &world2D->entities[i]->sprite);
		}

	
		MonGL::drawObject(&shader, &world2D->player->sprite);

#endif

	}

}