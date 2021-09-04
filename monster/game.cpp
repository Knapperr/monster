#include "game.h"

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

	bool Game::init()
	{
		state = State::Play;

		shader = {};
		MonGL::LoadShader(&shader, "res/shaders/vert_colors.glsl", "res/shaders/frag_colors.glsl", NULL);

		cam = Camera();

		player = {};
		MonGL::initQuad(&player.data, shader.id, "res/textures/p1.png");
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
			MonGL::initQuad(&tree.data, shader.id, "res/textures/tree.png");
			tree.particle.pos = v3(6.0f * (i + 1), 0.1f, 5.5f * i);
			trees.push_back(tree);
		}

		for (int i = 0; i < 4; ++i)
		{
			Entity entity = {};
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


		drawCollisions = true;
		return true;
	}

	// TODO(ck): Should this param be pointer?
	// should velocity be acceleratio?
	void Game::movePlayer(v3* velocity)
	{
		if ((velocity->x != 0.0f) && (velocity->y != 0.0f))
		{
			*velocity *= 0.707106781187f;
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
		
		

		if (input.quit.endedDown)
		{
			state = State::Debug;
			cam.followOff();
			config->viewPort = Rect{ config->viewPort.x, config->viewPort.y, 960, 540 };
		}


		//player.particle.velocity = {};
		bool enabled = state == State::Play;
		if (enabled)
		{
			cam.followOn();

			v3 velocity = {};

			if (input.up.endedDown)
			{
				velocity.z = -1.0f;
				//player.particle.velocity.z = -1.0f;
			}
			if (input.down.endedDown)
			{
				velocity.z = 1.0f;
				//player.particle.velocity.z = 1.0f;
			}
			if (input.left.endedDown)
			{
				velocity.x = -1.0f;
				//player.particle.velocity.x = -1.0f;

			}
			if (input.right.endedDown)
			{
				velocity.x = 1.0f;
				//player.particle.velocity.x = 1.0f;

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

		mat4 model = mat4(1.0f);
		player.colliderData.worldMatrix = translate(model, player.particle.pos);
	}

	void Game::render(double dt)
	{
		//projection = glm::perspective(glm::radians(cam.zoom), 1440.0f / 720.0f, 0.1f, 100.0f);
		mat4 projection = glm::perspective(glm::radians(cam.zoom), 960.0f / 540.0f, cam.nearPlane, cam.farPlane);
		mat4 view = cam.viewMatrix();

		MonGL::viewPort(&config->viewPort);

		MonGL::drawTerrain(shader.id, &terrain->mesh, &light, projection, view, cam.pos);

		// TODO(ck): use shader
		glUseProgram(shader.id);
		// light & material
		glUniform3fv(glGetUniformLocation(shader.id, "light.pos"), 1, &light.pos[0]);
		glUniform3fv(glGetUniformLocation(shader.id, "light.ambient"), 1, &light.ambient[0]);
		glUniform3fv(glGetUniformLocation(shader.id, "light.diffuse"), 1, &light.diffuse[0]);
		glUniform3fv(glGetUniformLocation(shader.id, "light.specular"), 1, &light.specular[0]);


		if (drawCollisions)
			MonGL::drawBoundingBox(&player.colliderData, player.particle.pos, cam.pos, projection, view, shader.id);

		MonGL::drawQuad(config, &player.data, player.particle.pos, v3(1.0f), cam.pos, projection, view, shader.id);

		for (auto& e : enemies)
		{
			MonGL::drawBoundingBox(&e.colliderData, e.particle.pos, cam.pos, projection, view, shader.id);
		}
		for (auto& e : trees)
		{
			MonGL::drawQuad(config, &e.data, e.particle.pos, v3(1.0f, 10.0f, 1.0f), cam.pos, projection, view, shader.id);
		}


	}

	void Game::cleanUp()
	{
		// TODO(ck): clean up render data 
		glDeleteVertexArrays(1, &player.data.VAO);
		glDeleteBuffers(1, &player.data.VBO);

		MonGL::DeleteShader(&shader);
	}


	bool Game::playing()
	{
		return (state == State::Play && cam.follow == true);
	}



	/// 
	/// 2D
	/// 
	
		// TODO(ck): Should this param be pointer?
	// should velocity be acceleratio?
	void Game::movePlayer2D(v2* velocity, Entity2D* p)
	{
		if ((velocity->x != 0.0f) && (velocity->y != 0.0f))
		{
			*velocity *= 0.707106781187f;
		}

		*velocity *= p->speed;

		*velocity += -2.5f * p->velocity;

		v2 oldPos = p->pos;
		v2 newPos = oldPos;
		float deltaX = (0.5f * velocity->x * square(deltaTime) + p->velocity.x * deltaTime);
		float deltaY = (0.5f * velocity->y * square(deltaTime) + p->velocity.y * deltaTime);
		v2 delta = { deltaX, deltaY };

		// TODO(ck): need to set an offest like casey does
		newPos += delta;


		p->velocity.x = velocity->x * deltaTime + p->velocity.x;
		p->velocity.y = velocity->y * deltaTime + p->velocity.y;

		p->pos = newPos;

	}

	bool Game::init(int x)
	{
		state = State::Play;

		// TODO(ck): Memory management
		world = new World();
		MonGL::LoadShader(&shader, "res/shaders/vert_sprite.glsl", "res/shaders/frag_sprite.glsl", NULL);
		// TODO(ck): REMOVE TESTING TILE SHADER
		MonGL::LoadShader(&tileShader, "res/shaders/vert_tile.glsl", "res/shaders/frag_tile.glsl", NULL);

		// Set up the shader locations for our objects
		glUseProgram(shader.id);

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

		int imgLoc = glGetUniformLocation(shader.id, "image");
		glUniform1i(imgLoc, 0);

		int projLoc = glGetUniformLocation(shader.id, "projection");
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));


		// TODO(ck): REMOVE TESTING TILE SHADER
		glUseProgram(tileShader.id);
		imgLoc = glGetUniformLocation(tileShader.id, "image");
		glUniform1i(imgLoc, 0);

		projLoc = glGetUniformLocation(tileShader.id, "projection");
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		mat4 model = mat4(1.0f);
		v3 pos = v3(0, 0, 1.0f);
		model = glm::translate(model, pos);
		model = glm::scale(model, v3(v2(32, 32), 1.0f));
		glUniformMatrix4fv(glGetUniformLocation(tileShader.id, "model"), 1, GL_FALSE, glm::value_ptr(model));



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

		camera = {};
		camera.lerpSpeed = 7.0f;
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

			if (input->up.endedDown)
			{
				velocity.y = -1.0f;
			}
			if (input->down.endedDown)
			{
				velocity.y = 1.0f;
			}
			if (input->left.endedDown)
			{
				velocity.x = -1.0f;
			}
			if (input->right.endedDown)
			{
				velocity.x = 1.0f;
			}

			movePlayer2D(&velocity, p);
			// update sprite position 
			p->sprite.pos = p->pos;

			// TODO(ck): Update camera pos
			// camera position = player position

			//real32 playerGroundPointX = screenCenterX + metersToPixels * diff.dX;
			//real32 playerGroundPointY = screenCenterY - metersToPixels * diff.dY;

		}

		//const unsigned int SCREEN_WIDTH = 1280;
		//const unsigned int SCREEN_HEIGHT = 720;
		// speed 
		camera.target.x = lerp(camera.target.x, camera.lerpSpeed * dt, world->player->pos.x);
		camera.target.y = lerp(camera.target.y, camera.lerpSpeed * dt, world->player->pos.y);
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
		glUseProgram(tileShader.id);
		int projLoc = glGetUniformLocation(tileShader.id, "projection");
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		MonGL::drawMap(&tileShader, world->map->sheet.texture.id);

		glUseProgram(shader.id);
		projLoc = glGetUniformLocation(shader.id, "projection");
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