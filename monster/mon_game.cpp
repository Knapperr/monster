#include "mon_game.h"

#include "handmade_random.h"

namespace Mon
{
	bool Game::init(int windowWidth, int windowHeight, int portWidth, int portHeight)
	{
		state = State::Debug;

		shader = {};
		waterShader = {};
		MonGL::LoadShader(&shader, "res/shaders/vert_colors.glsl", "res/shaders/frag_colors.glsl", NULL);
		MonGL::LoadShader(&waterShader.common, "res/shaders/vert_water.glsl", "res/shaders/frag_water.glsl", NULL);

		mainShaderID = shader.handle;
		
		// TODO(ck): MEMORY MANAGEMENT
		world = new World();
		InitWorld(world, shader.handle);
		InitPlayer(world, shader.handle);

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
		MonGL::GenerateTerrain(&terrain->mesh);
		//terrain->generate();

		simulate = false;
		
		config = new MonGL::Config();
		config->viewPort = {
			5.0f,
			5.0f,
			(float)portWidth,
			(float)portHeight
		};
		MonGL::ViewPort(&config->viewPort);

		config->angleDegrees = -30.0f;

		// TODO(ck): Memory management
		// reserve slot 0 for NULL camera
		addCamera(this);

		int followCamIndex = addCamera(this);
		InitCamera(&cameras[followCamIndex], CameraType::Follow, "Follow", config->viewPort);

		int debugCamIndex = addCamera(this);
		InitCamera(&cameras[debugCamIndex], CameraType::Fly, "Debug 1", config->viewPort);
		int debugCamIndex2 = addCamera(this);
		InitCamera(&cameras[debugCamIndex2], CameraType::Fly, "Debug 2", config->viewPort);
		
		currCameraIndex = debugCamIndex;
		selectedIndex = 1;
		drawCollisions = true;

		state = State::Debug;
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
		

		//
		// CAMERA
		//
		if (input.num1.endedDown) 
		{ 
			currCameraIndex = 1;
			playMode();
		}
		if (input.num2.endedDown) 
		{ 
			currCameraIndex = 2; 
			debugMode();
		}
		if (input.num3.endedDown) 
		{ 
			currCameraIndex = 3; 
			debugMode();
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
		//cam->update(deltaTime, &input, world->player->particle.pos, world->player->particle.orientation, true);
		

		//
		//	ENTITIES UPDATE
		//



		// 
		// CAMERA UPDATE
		//

		Update(&cameras[currCameraIndex], deltaTime, &input, world->player->particle.pos, world->player->particle.orientation, true);

		//
		// UPDATE COLLIDERS
		// 
		// TODO(ck): Give player pos matrix
		mat4 model = mat4(1.0f);
		float colliderPosX = world->player->particle.pos.x - (0.5f);
		float colliderPosZ = world->player->particle.pos.z - (0.5f);
		world->player->collider.data.worldMatrix = translate(model, v3(colliderPosX, -0.2f, colliderPosZ));
		world->player->collider.data.worldMatrix *= GetTransform(&world->player->collider);
	
		for (int i = 1; i < world->entityCount; ++i)
		{
			mat4 model = mat4(1.0f);
			v3 colliderPos = { world->entities[i].particle.pos.x - (0.5f), -0.2f,
								world->entities[i].particle.pos.z - (0.5f) };
			world->entities[i].collider.data.worldMatrix = translate(model, colliderPos);
			world->entities[i].collider.data.worldMatrix *= GetTransform(&world->entities[i].collider);
		}
	}

	void Game::render(double dt)
	{
		Camera* cam = getCamera(this, currCameraIndex);
		// TODO(ck): pass all shaders to beginRender or they live in the opengl layer and get
		// activated that way
		MonGL::BeginRender(config, Projection(cam), ViewMatrix(cam), shader.handle);
		MonGL::BeginRender(config, Projection(cam), ViewMatrix(cam), waterShader.common.handle);

		MonGL::DrawTerrain(shader.handle, &terrain->mesh, &light, cam);

		// TODO(ck): use shader
		glUseProgram(shader.handle);
		// light & material
		glUniform3fv(glGetUniformLocation(shader.handle, "light.pos"), 1, &light.pos[0]);
		glUniform3fv(glGetUniformLocation(shader.handle, "light.ambient"), 1, &light.ambient[0]);
		glUniform3fv(glGetUniformLocation(shader.handle, "light.diffuse"), 1, &light.diffuse[0]);
		glUniform3fv(glGetUniformLocation(shader.handle, "light.specular"), 1, &light.specular[0]);

		//
		// COLLIDERS DRAW
		//
		if (drawCollisions)
		{
			MonGL::DrawBoundingBox(&world->player->collider.data, cam, shader.handle);
			for (int i = 1; i < world->entityCount; ++i)
				MonGL::DrawBoundingBox(&world->entities[i].collider.data, cam, shader.handle);

		}
		
		//
		// PLAYER DRAW
		//
		MonGL::Draw(config, &world->player->data, world->player->particle.pos, cam, shader.handle, world->player->facingDir);

		//
		// ENTITIES DRAW
		//
		for (int i = 1; i < world->entityCount; ++i)
		{
			Entity e = world->entities[i];
			MonGL::Draw(config, &e.data, e.particle.pos, cam, shader.handle);
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

	///
	/// Utility 
	///

	void Game::setViewPort(int width, int height)
	{
		config->viewPort = Rect{ 0.0f, 0.0f, (float)width, (float)height };
	}

	bool Game::playing()
	{
		return (state == State::Play);
	}

	void Game::playMode()
	{
		currCameraIndex = 1;
		state = State::Play;
	}

	void Game::debugMode()
	{
		if (currCameraIndex != 1)
			state = State::Debug;
	}
}