#include "game.h"

#include "handmade_random.h"

#include <glm/gtc/matrix_transform.hpp>


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
		glm::vec3 resultingAcc = acceleration;

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
		shader = {};
		MonShader::LoadShader(&shader, "res/shaders/vert_colors.glsl", "res/shaders/frag_colors.glsl", NULL);

		cam = Camera();

		player = {};
		MonGL::initCharacter(&player.data, shader.id, 1);
		MonGL::initBoundingBox(&player.colliderData);
		player.particle.pos = glm::vec3(10.0f, 0.0f, 20.0f);
		player.particle.inverseMass = 10.0f;
		player.particle.velocity = glm::vec3(0.0f, 0.0f, 0.0f); // 35m/s
		player.particle.acceleration = glm::vec3(-40.0f, 0.0f, 0.0f);
		player.particle.damping = 0.9f;

		player.data.mat.ambient = glm::vec3(1.0f, 0.5f, 0.6f);
		player.data.mat.diffuse = glm::vec3(1.0f, 0.5f, 0.31f);
		player.data.mat.specular = glm::vec3(0.5f, 0.5f, 0.5f);
		player.data.mat.shininess = 32.0f;

		for (int i = 0; i < 10; ++i)
		{
			EntityTwo tree = {};
			MonGL::initCharacter(&tree.data, shader.id, 2);
			tree.particle.pos = glm::vec3(2.0f * i, 0.1f, 5.5f * i);
			trees.push_back(tree);
		}

		for (int i = 0; i < 4; ++i)
		{
			EntityTwo entity = {};
			MonGL::initBoundingBox(&entity.colliderData);
			entity.particle.pos = glm::vec3(10.0f * i, 0.1f, 1.5f * i);
			glm::mat4 model = glm::mat4(1.0f);
			entity.colliderData.worldMatrix = glm::translate(model, entity.particle.pos);
			entity.colliderData.color = glm::vec3(0.0f, 0.0f, 0.0f);
			enemies.push_back(entity);
		}

		light = {};
		glm::vec3 lightColor = glm::vec3(0.2f, 0.3f, 0.6f);
		light.diffuse = lightColor * glm::vec3(0.5f);
		light.ambient = light.diffuse * glm::vec3(0.2f);
		light.specular = glm::vec3(1.0f, 1.0f, 1.0f);
		light.pos = glm::vec3(16.0f, 10.0f, 16.0f);


		// TODO(ck): Memory Allocation
		terrain = new Terrain(0, 0);
		MonGL::generateTerrain(&terrain->mesh);
		//terrain->generate();

		simulate = false;
		drawCollisions = true;
		return true;
	}

	void Game::update(double dt, Input* newInput)
	{
		if (dt > deltaTime)
			printf("HIGHER dt: %f\n", deltaTime);
		if (dt < deltaTime)
			printf("LOWER dt: %f\n", deltaTime);

		deltaTime = dt;
		input = *newInput;

		// TODO(ck): TEMP INPUT
		if (cam.disabled == true)
		{
			if (input.up.endedDown)
			{
				if (player.particle.dir != Direction::FORWARD)
				{
					player.particle.dir = Direction::FORWARD;
					player.particle.velocity.z = player.particle.velocity.z / 2;
				}


				//player.particle.velocity.x = 0.0f;
				player.particle.acceleration = glm::vec3(0.0f, 0.0f, -10.0f);
				player.particle.integrate(dt);
			}
			if (input.down.endedDown)
			{
				if (player.particle.dir != Direction::BACKWARD)
				{
					player.particle.dir = Direction::BACKWARD;
					player.particle.velocity.z = player.particle.velocity.z / 2;
				}

				player.particle.acceleration = glm::vec3(0.0f, 0.0f, 10.0f);
				player.particle.integrate(dt);
			}
			if (input.left.endedDown)
			{
				if (player.particle.dir != Direction::LEFT)
				{
					player.particle.dir = Direction::LEFT;
					player.particle.velocity.x = player.particle.velocity.x / 2;
				}

				player.particle.acceleration = glm::vec3(-10.0f, 0.0f, 0.0f);
				player.particle.integrate(dt);
			}

			if (input.right.endedDown)
			{
				if (player.particle.dir != Direction::RIGHT)
				{
					player.particle.dir = Direction::RIGHT;
					player.particle.velocity.x = player.particle.velocity.x / 2;
				}

				//player.particle.velocity.z = 0.0f;
				player.particle.acceleration = glm::vec3(10.0f, 0.0f, 0.0f);
				player.particle.integrate(dt);
			}
			// The integate will be called here. Normally i suspect that you call the force and then it applies in the integrate and 
			// clears it out so that if no input is being pressed it wont move the object 

			//player.particle.integrate(dt);
		}
		cam.update(deltaTime, &input, true);

		if (simulate == true)
			player.particle.integrate(deltaTime);

		glm::mat4 model = glm::mat4(1.0f);
		player.colliderData.worldMatrix = glm::translate(model, player.particle.pos);
	}

	void Game::render(double dt)
	{
		//projection = glm::perspective(glm::radians(cam.zoom), 1440.0f / 720.0f, 0.1f, 100.0f);
		glm::mat4 projection = glm::perspective(glm::radians(cam.zoom), 960.0f / 540.0f, cam.nearPlane, cam.farPlane);
		glm::mat4 view = cam.viewMatrix();

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

		MonGL::drawCharacter(&player.data, player.particle.pos, cam.pos, projection, view, shader.id);

		for (auto& e : enemies)
		{
			MonGL::drawBoundingBox(&e.colliderData, e.particle.pos, cam.pos, projection, view, shader.id);
		}
		for (auto& e : trees)
		{
			MonGL::drawCharacter(&e.data, e.particle.pos, cam.pos, projection, view, shader.id);
		}
	}

	void Game::cleanUp()
	{
		// TODO(ck): clean up render data 
		glDeleteVertexArrays(1, &player.data.VAO);
		glDeleteBuffers(1, &player.data.VBO);

		MonShader::DeleteShader(&shader);
	}


	/// 
	/// 2D
	/// 
	bool Game::init(int x)
	{
		// TODO(ck): Memory management
		world = new World();
		LoadShader(&shader, "res/shaders/vert_sprite.glsl", "res/shaders/frag_sprite.glsl", NULL);
		// TODO(ck): REMOVE TESTING TILE SHADER
		LoadShader(&tileShader, "res/shaders/vert_tile.glsl", "res/shaders/frag_tile.glsl", NULL);

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

		glm::mat4 projection = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
		// TODO(CK): We don't need to set the image in the shader im not sure why we
		// are setting it to 0 just to enforce that it gets used maybe????
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

		glm::mat4 model = glm::mat4(1.0f);
		glm::vec3 pos = glm::vec3(0, 0, 1.0f);
		model = glm::translate(model, pos);
		model = glm::scale(model, glm::vec3(glm::vec2(32, 32), 1.0f));
		glUniformMatrix4fv(glGetUniformLocation(tileShader.id, "model"), 1, GL_FALSE, glm::value_ptr(model));


		return true;
	}

	void Game::update(double dt, Input* input, int x)
	{
		if (dt > deltaTime || dt < deltaTime)
			printf("dt: %f\n", dt);

		deltaTime = dt;
		this->input = *input;

		

		//if (state->deltaTime != dt)
			//state->deltaTime = dt;
		float newSpeed = world->player->speed;
		if (input->shift.endedDown)
			newSpeed *= 1.7;

		Entity* p = world->player;


		if (input->up.endedDown)
		{
			p->velocity.y -= p->speed;

			if (p->velocity.y <= -p->maxSpeed)
				p->velocity.y = -p->maxSpeed;
		}
		else if (input->down.endedDown)
		{
			p->velocity.y += p->speed;

			if (p->velocity.y >= p->maxSpeed)
				p->velocity.y = p->maxSpeed;
		}
		else
		{
			p->velocity.y = 0;
		}
		
		if (input->left.endedDown)
		{
			p->velocity.x -= p->speed;

			if (p->velocity.x <= -p->maxSpeed)
				p->velocity.x = -p->maxSpeed;

		}
		else if (input->right.endedDown)
		{
			p->velocity.x += p->speed;

			if (p->velocity.x >= p->maxSpeed)
				p->velocity.x = p->maxSpeed;

		}
		else
		{
			p->velocity.x = 0;
		}

		// TODO(ck): This is making me go too slow
		if ((p->velocity.x != 0.0f) && (p->velocity.y != 0.0f))
		{
			p->velocity *= 0.707106781187f;
			
		}


		p->pos.x += p->velocity.x * dt;
		p->pos.y += p->velocity.y * dt;
		

		// TODO(ck): Update camera pos
		// camera position = player position

		//real32 playerGroundPointX = screenCenterX + metersToPixels * diff.dX;
		//real32 playerGroundPointY = screenCenterY - metersToPixels * diff.dY;



		//const unsigned int SCREEN_WIDTH = 1280;
		//const unsigned int SCREEN_HEIGHT = 720;
		camera.target = world->player->pos;
	}

	void Game::render()
	{
		// TODO(ck): Clean up --- camera
		//float left = 0.0f;

		// 640.0f = window.x
		// 360.0f = window.y
		glm::vec2 target = camera.target;
		float width = 960.0f;
		float height = 540.0f;
		float half = 4.0f;
		float left = target.x - width / half;
		float right = target.x + width / half;
		float top = target.y - height / half;
		float bottom = target.y + height / half;
		glm::mat4 projection = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
		glm::mat4 zoom = glm::scale(glm::mat4(1.0f), glm::vec3(camera.zoom, camera.zoom, camera.zoom));
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

			MonGL::drawObject(&shader, world->entities[i]);
		}

		


		MonGL::drawObject(&shader, world->player);
		


	}

}