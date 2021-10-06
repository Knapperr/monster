#include "mon_game.h"

#include "handmade_random.h"

namespace Mon
{

#define real_pow powf
	void Particle::integrate(float duration)
	{
		if (inverseMass <= 0.0f)
			return;

		// assert(duration > 0.0);
		/*
			x += vector.x * scale;
			y += vector.y * scale;
			z += vector.z * scale;
		*/
		pos += velocity * duration;

		// update linear pos
		v3 resultingAcc = acceleration;

		// work out the acceleration from the force
		velocity += resultingAcc * duration;

		// impose drag
		velocity *= real_pow(damping, duration);

		// TODO(ck): TEMP FOR TESTING
		if (velocity.x < -15.0f)
			velocity.x = -15.0f;
		if (velocity.x > 15.0f)
			velocity.x = 15.0f;

		if (velocity.z < -15.0f)
			velocity.z = -15.0f;
		if (velocity.z > 15.0f)
			velocity.z = 15.0f;

		clearAccumulator();
	}

	void Particle::clearAccumulator()
	{
		return;
	}


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

	bool Game::init()
	{
		state = State::Debug;

		shader = {};
		waterShader = {};
		MonGL::LoadShader(&shader, "res/shaders/vert_colors.glsl", "res/shaders/frag_colors.glsl", NULL);
		MonGL::LoadShader(&waterShader.common, "res/shaders/vert_water.glsl", "res/shaders/frag_water.glsl", NULL);

		mainShaderID = shader.handle;
		cam = Camera();

		player = {};
		player.name = "player";
		player.setup = {};
		MonGL::initQuad(&player.data);
		MonGL::loadTexture(&player.data, MonGL::Type::Diffuse, shader.handle, "res/textures/p1.png");
		MonGL::initBoundingBox(&player.colliderData);
		player.particle.pos = v3(40.0f, 0.1f, 10.0);
		player.particle.inverseMass = 10.0f;
		player.particle.velocity = v3(0.0f, 0.0f, 0.0f); // 35m/s
		player.particle.acceleration = v3(-40.0f, 0.0f, 0.0f);
		player.particle.orientation = v3(1.0f, 1.0f, 1.0);
		player.particle.damping = 0.9f;
		player.particle.speed = 50.0f;

		player.data.mat.ambient = v3(1.0f, 0.5f, 0.6f);
		player.data.mat.diffuse = v3(1.0f, 0.5f, 0.31f);
		player.data.mat.specular = v3(0.5f, 0.5f, 0.5f);
		player.data.mat.shininess = 32.0f;

		for (int i = 0; i < 10; ++i)
		{
			Entity tree = {};
			tree.setup = {};
			tree.name = "tree_" + std::to_string(i);
			MonGL::initQuad(&tree.data);
			MonGL::loadTexture(&tree.data, MonGL::Type::Diffuse, shader.handle, "res/textures/tree.png");
			tree.particle.pos = v3(6.0f * (i + 1), 6.80f, 5.5f * i);
			entities.push_back(tree);
		}

		for (int i = 0; i < 10; ++i)
		{
			Entity flower = {};
			flower.setup = {};
			flower.name = "flower_" + std::to_string(i);
			MonGL::initQuad(&flower.data);
			MonGL::loadTexture(&flower.data, MonGL::Type::Diffuse, shader.handle, "res/textures/sflow_tall.png");
			flower.particle.pos = v3(10.0f, 0.1f, 6.0f);
			
			entities.push_back(flower);
		}

		for (int i = 0; i < 4; ++i)
		{
			Entity entity = {};
			entity.setup = {};
			entity.name = "enemy_" + std::to_string(i);
			MonGL::initBoundingBox(&entity.colliderData);
			entity.particle.pos = v3((8.0f * (i + 1)), 0.1f, 1.5f * i);
			mat4 model = mat4(1.0f);
			entity.colliderData.worldMatrix = glm::translate(model, entity.particle.pos);
			entity.colliderData.color = v3(0.0f, 0.0f, 0.0f);
			enemies.push_back(entity);
		}

		light = {};
		v3 lightColor = v3(0.2f, 0.3f, 0.6f);
		light.diffuse = lightColor * v3(0.5f);
		light.ambient = light.diffuse * v3(0.2f);
		light.specular = v3(1.0f, 1.0f, 1.0f);
		light.pos = v3(16.0f, 10.0f, 16.0f);


		water = {};
		MonGL::initDistortedWater(&water.data, &water.setup);
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
		int screenWidth = 1440;
		int screenHeight = 900;
		int portWidth = 960;
		int portHeight = 540;
		config->viewPort.x = (screenWidth - portWidth) / 2;
		config->viewPort.y = (screenHeight - portHeight) / 2;
		config->viewPort.w = portWidth;
		config->viewPort.h = portHeight;
		MonGL::viewPort(&config->viewPort);
		config->angleDegrees = -30.0f;

		selectedIndex = 0;
		drawCollisions = true;
		return true;
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
		*velocity *= player.particle.speed;

		*velocity += -7.0f * player.particle.velocity;

		v3 oldPos = player.particle.pos;
		v3 newPos = oldPos;
		float deltaX = (0.5f * velocity->x * square(deltaTime) + player.particle.velocity.x * deltaTime);
		float deltaZ = (0.5f * velocity->z * square(deltaTime) + player.particle.velocity.z * deltaTime);
		v3 delta = { deltaX, 0.0f, deltaZ };

		// TODO(ck): need to set an offest like casey does
		newPos += delta;


		player.particle.velocity.x = velocity->x * deltaTime + player.particle.velocity.x;
		player.particle.velocity.z = velocity->z * deltaTime + player.particle.velocity.z;

		player.particle.pos = newPos;

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
		cam.update(deltaTime, &input, player.particle.pos, player.particle.orientation, true);

		//if (simulate == true)
			//player.particle.integrate(deltaTime);


		// TODO(ck): Give player pos matrix
		mat4 model = mat4(1.0f);
		float colliderPosX = player.particle.pos.x - (0.5f);
		float colliderPosZ = player.particle.pos.z - (0.5f);
		player.colliderData.worldMatrix = translate(model, v3(colliderPosX, -0.2f, colliderPosZ));
	}

	void Game::render(double dt)
	{
		//projection = glm::perspective(glm::radians(cam.zoom), 1440.0f / 720.0f, 0.1f, 100.0f);
		mat4 proj = cam.projection();
		mat4 view = cam.viewMatrix();

		// TODO(ck): pass all shaders to beginRender or they live in the opengl layer and get
		// activated that way
		MonGL::beginRender(config, proj, view, shader.handle);
		MonGL::beginRender(config, proj, view, waterShader.common.handle);

		MonGL::drawTerrain(shader.handle, &terrain->mesh, &light, proj, view, cam.pos);

		// TODO(ck): use shader
		glUseProgram(shader.handle);
		// light & material
		glUniform3fv(glGetUniformLocation(shader.handle, "light.pos"), 1, &light.pos[0]);
		glUniform3fv(glGetUniformLocation(shader.handle, "light.ambient"), 1, &light.ambient[0]);
		glUniform3fv(glGetUniformLocation(shader.handle, "light.diffuse"), 1, &light.diffuse[0]);
		glUniform3fv(glGetUniformLocation(shader.handle, "light.specular"), 1, &light.specular[0]);


		if (drawCollisions)
			MonGL::drawBoundingBox(&player.colliderData, player.particle.pos, cam.pos, proj, view, shader.handle);

		MonGL::drawQuad(config, &player.data, player.particle.pos, v3(1.0f), cam.pos, shader.handle);

		for (auto& e : enemies)
		{
			MonGL::drawBoundingBox(&e.colliderData, e.particle.pos, cam.pos, proj, view, shader.handle);
		}
		for (auto& e : entities)
		{

			MonGL::drawQuad(config, &e.data, e.particle.pos, v3(16.0f, 16.0f, 1.0f), cam.pos, shader.handle);
		}
		
		glUseProgram(waterShader.common.handle);
		MonGL::drawWater(&water.data, &water.setup, &waterShader, &light, water.particle.pos, v3(10.0f), cam.pos, waterShader.common.handle);
		//MonGL::drawQuad(config, &water.data, water.particle.pos, v3(5.0f), cam.pos, shader.handle);

	}

	void Game::cleanUp()
	{
		// TODO(ck): clean up render data 
		glDeleteVertexArrays(1, &player.data.VAO);
		glDeleteBuffers(1, &player.data.VBO);

		MonGL::DeleteShader(&shader);
	}

	void Game::setViewPort(int width, int height)
	{
		config->viewPort = Rect{ 0.0f, 0.0f, (float)width, (float)height };
	}

	bool Game::playing()
	{
		return (state == State::Play && cam.follow == true);
	}

	void Game::playMode()
	{
		state = State::Play;
		cam.followOn();
	}

	void Game::debugMode()
	{
		state = State::Debug;
		cam.followOff();
	}

	/// 
	/// 2D
	/// 

	bool Game::init(int x)
	{
		state = State::Play;

		// TODO(ck): Memory management
		world = new World();
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

		camera = OrthoCamera(world->player->pos);

		return true;
	}

	void Game::update(double dt, Input* input, int x)
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
					velocity.y = -1.0f;
					//p->position.y -= 1.0f * p->speed * dt;
				}
				if (input->down.endedDown)
				{
					velocity.y = 1.0f;
					//p->position.y += 1.0f * p->speed * dt;

				}
				if (input->left.endedDown)
				{
					velocity.x = -1.0f;
					//p->position.x -= 1.0f * p->speed * dt;
				}
				if (input->right.endedDown)
				{
					velocity.x = 1.0f;
					//p->position.x += 1.0f * p->speed * dt;
				}
			}
			Mon::movePlayer(world->map, p, &velocity, deltaTime);
			// update sprite position 
			// TODO(ck): I am pretty sure in handmade hero he rounds these values before drawing
			//p->sprite.pos.x = (int)p->pos.x;
			//p->sprite.pos.y = (int)p->pos.y;
			//p->sprite.pos = p->pos;
			p->sprite.pos = p->position;
			// TODO(ck): Update camera pos
			// camera position = player position

			//real32 playerGroundPointX = screenCenterX + metersToPixels * diff.dX;
			//real32 playerGroundPointY = screenCenterY - metersToPixels * diff.dY;

			camera.update(&p->sprite.pos, deltaTime);
		}
		//camera.update(&world->player->position, deltaTime);
	}

	void Game::render()
	{
		// TODO(ck): Clean up --- camera
		//float left = 0.0f;

		MonGL::viewPort(&config->viewPort);

		// 640.0f = window.x
		// 360.0f = window.y
		// TODO(ck): my camera isn't deciding the projection that gets sent to the renderer
		// this might be a good thing because i can separate that off here
		v2 target = camera.target;
		float width = 960.0f;
		float height = 540.0f;

#if 1
		float half = 4.0f;
		float left = target.x - width / half;
		float right = target.x + width / half;
		float top = target.y - height / half;
		float bottom = target.y + height / half;
#else
		float left = 0.0f;
		float right = 960.0f;
		float top = 0.0f;
		float bottom = 540.0f;
#endif

		mat4 projection = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
		//mat4 zoom = glm::scale(mat4(1.0f), v3(camera.zoom, camera.zoom, camera.zoom));
		//projection *= zoom;

		// TEST DRAW
		glUseProgram(tileShader.handle);
		int projLoc = glGetUniformLocation(tileShader.handle, "projection");
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		MonGL::drawMap(&tileShader, world->sheet.texture.id);

		glUseProgram(shader.handle);
		projLoc = glGetUniformLocation(shader.handle, "projection");
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		for (unsigned int i = 0; i < world->entities.size(); ++i)
		{
			//state->world->entities[i]->pos.x *= time;
			//state->world->entities[i]->pos.y *= time;
			MonGL::drawObject(&shader, &world->entities[i]->sprite);
		}

	
		MonGL::drawObject(&shader, &world->player->sprite);
		
	}

}