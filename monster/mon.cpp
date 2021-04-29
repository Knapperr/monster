#include "mon.h"

#include "handmade_random.h"

namespace Mon
{

#define real_pow powf
	void Particle::integrate(float duration)
	{
		if (inverseMass <= 0.0f)
			return;

		// assert(duration > 0.0);
		/*x += vector.x * scale;
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
		// 
		MonGL::gl_InitRenderDataCube(&player.data);
		MonGL::gl_InitRenderDataCube(&player.colliderData);
		player.particle.pos = glm::vec3(10.0f, 10.0f, 20.0f);
		player.particle.inverseMass = 10.0f;
		player.particle.velocity = glm::vec3(20.0f, 30.0f, 20.0f); // 35m/s
		player.particle.acceleration = glm::vec3(0.0f, -80.0f, 0.0f);
		player.particle.damping = 0.99f;

		player.data.mat.ambient = glm::vec3(1.0f, 0.5f, 0.6f);
		player.data.mat.diffuse = glm::vec3(1.0f, 0.5f, 0.31f);
		player.data.mat.specular = glm::vec3(0.5f, 0.5f, 0.5f);
		player.data.mat.shininess = 32.0f;

		light = {};
		glm::vec3 lightColor = glm::vec3(0.2f, 0.3f, 0.6f);
		light.diffuse = lightColor * glm::vec3(0.5f);
		light.ambient = light.diffuse * glm::vec3(0.2f);
		light.specular = glm::vec3(1.0f, 1.0f, 1.0f);
		light.pos = glm::vec3(16.0f, 10.0f, 16.0f);


		// TODO(ck): Memory Allocation
		terrain = new Terrain(0, 0);
		terrain->generate();

		simulate = false;

		return true;
	}

	void Game::update(double dt, Input* input)
	{
		deltaTime = dt;

		if (cam.disabled == true)
		{
			if (input->up.endedDown)
				player.particle.pos.z -= 6 * dt;
			if (input->down.endedDown)
				player.particle.pos.z += 6 * dt;
			if (input->left.endedDown)
				player.particle.pos.x -= 6 * dt;
			if (input->right.endedDown)
				player.particle.pos.x += 6 * dt;

			//player.particle.integrate(dt);
		}
		cam.update(deltaTime, input, true);

		if (simulate == true)
			player.particle.integrate(deltaTime);
	}

	void Game::render(double dt)
	{
		//projection = glm::perspective(glm::radians(cam.zoom), 1440.0f / 720.0f, 0.1f, 100.0f);
		glm::mat4 projection = glm::perspective(glm::radians(cam.zoom), 960.0f / 540.0f, cam.nearPlane, cam.farPlane);
		glm::mat4 view = cam.viewMatrix();

		// TERRAIN
		// ------------------------------------------
		glUseProgram(shader.id);

		glUniformMatrix4fv(glGetUniformLocation(shader.id, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(shader.id, "view"), 1, GL_FALSE, glm::value_ptr(view));

		glUniform3fv(glGetUniformLocation(shader.id, "light.pos"), 1, &light.pos[0]);
		glUniform3fv(glGetUniformLocation(shader.id, "light.ambient"), 1, &light.ambient[0]);
		glUniform3fv(glGetUniformLocation(shader.id, "light.diffuse"), 1, &light.diffuse[0]);
		glUniform3fv(glGetUniformLocation(shader.id, "light.specular"), 1, &light.specular[0]);

		glUniform3fv(glGetUniformLocation(shader.id, "material.ambient"), 1, &terrain->material.ambient[0]);
		glUniform3fv(glGetUniformLocation(shader.id, "material.diffuse"), 1, &terrain->material.diffuse[0]);
		glUniform3fv(glGetUniformLocation(shader.id, "material.specular"), 1, &terrain->material.specular[0]);
		glUniform1f(glGetUniformLocation(shader.id, "material.shininess"), terrain->material.shininess);

		glUniform3fv(glGetUniformLocation(shader.id, "viewPos"), 1, &cam.pos[0]);

		/*
		// set the sampler to the correct texture unit
		glUniform1i(glGetUniformLocation(terr->shader->id, "texture_diffuse1"), 0);
		if (terr->drawTexture)
		{
			glBindTexture(GL_TEXTURE_2D, terr->selectedTextureId);
		}
		*/

		glm::mat4 matModel = glm::mat4(1.0f);

		glm::mat4 matTranslate = glm::translate(glm::mat4(1.0f),
												glm::vec3(terrain->x, 0.0f, terrain->z));
		matModel = matModel * matTranslate;
		glm::mat4 rotateZ = glm::rotate(glm::mat4(1.0f),
										0.0f,
										glm::vec3(0.0f, 0.0f, 1.0f));
		matModel = matModel * rotateZ;
		glm::mat4 rotateY = glm::rotate(glm::mat4(1.0f),
										0.0f,
										glm::vec3(0.0f, 1.0f, 0.0f));
		matModel = matModel * rotateY;
		glm::mat4 rotateX = glm::rotate(glm::mat4(1.0f),
										0.0f,
										glm::vec3(1.0f, 0.0f, 0.0f));
		matModel = matModel * rotateX;

		glm::mat4 matScale = glm::scale(glm::mat4(1.0f),
										glm::vec3(1.0, 1.0, 1.0));

		matModel = matModel * matScale;


		glUniform1i(glGetUniformLocation(shader.id, "useTexture"), true);

		glBindTexture(GL_TEXTURE_2D, terrain->selectedTextureId);

		glUniformMatrix4fv(glGetUniformLocation(shader.id, "model"), 1, GL_FALSE, glm::value_ptr(matModel));
		glBindVertexArray(terrain->VAO);

		terrain->wireFrame ?
			glDrawElements(GL_LINES, terrain->indicesLength, GL_UNSIGNED_INT, 0)
			: glDrawElements(GL_TRIANGLES, terrain->indicesLength, GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);
		// Always good practice to set everything back to defaults once configured
		// NOTE(CK): bind texture must be AFTER glActiveTexture or it will not unbind properly
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
	// ------------------------------------------



	// ------------------------------------------
		// TODO(ck): use shader
		glUseProgram(shader.id);
		// light & material
		glUniform3fv(glGetUniformLocation(shader.id, "light.pos"), 1, &light.pos[0]);
		glUniform3fv(glGetUniformLocation(shader.id, "light.ambient"), 1, &light.ambient[0]);
		glUniform3fv(glGetUniformLocation(shader.id, "light.diffuse"), 1, &light.diffuse[0]);
		glUniform3fv(glGetUniformLocation(shader.id, "light.specular"), 1, &light.specular[0]);

		//MonGL::gl_DrawCube(&player.data, player.particle.pos, cam.pos, projection, view, shader.id);
		MonGL::gl_DrawCube(&player.colliderData, player.particle.pos, cam.pos, projection, view, shader.id);
		// ------------------------------------------


	}

	void Game::cleanUp()
	{
		// TODO(ck): clean up render data 
		glDeleteVertexArrays(1, &player.data.VAO);
		glDeleteBuffers(1, &player.data.VBO);

		MonShader::DeleteShader(&shader);
	}

	bool Game::init(int x)
	{
		// TODO(ck): Memory management
		world = new World();
		LoadShader(&shader, "res/shaders/vert_sprite.glsl", "res/shaders/frag_sprite.glsl", NULL);

		Entity* ball = new Entity("res/textures/awesomeface.png", true,
								  glm::vec2(200, 200), glm::vec2(32, 32), glm::vec3(1.0f, 1.0f, 1.0f),
								  200.0f, 0.0f, false);

		world->entities.push_back(ball);


		// Set up the shader locations for our objects
		glUseProgram(shader.id);

		// TODO(CK): CAMERA
		// So let's say you want your pixel art scale 2:1
		// Then your target 1080p. Just take the resolution and divide by 2. Examples
		// This is for 1920x1080 I am using 1280x720 right now
		// 2:1 960x540 -- 3:1 640x360 --- 4:! 480x240
		float left = 0.0f;
		float right = 640.0f;
		float bottom = 360.0f;
		float top = 0.0f;

		glm::mat4 projection = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
		// TODO(CK): We don't need to set the image in the shader im not sure why we
		// are setting it to 0 just to enforce that it gets used maybe????
		int imgLoc = glGetUniformLocation(shader.id, "image");
		glUniform1i(imgLoc, 0);

		int projLoc = glGetUniformLocation(shader.id, "projection");
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));


		return true;
	}

	void Game::update(double dt, Input* input, int x)
	{
		//if (state->deltaTime != dt)
			//state->deltaTime = dt;
		float newSpeed = world->player->speed;
		if (input->shift.endedDown)
			newSpeed *= 2;


		if (input->up.endedDown)    world->player->pos.y -= (int)(newSpeed * dt);
		if (input->down.endedDown)  world->player->pos.y += (int)(newSpeed * dt);
		if (input->left.endedDown)  world->player->pos.x -= (int)(newSpeed * dt);
		if (input->right.endedDown) world->player->pos.x += (int)(newSpeed * dt);

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

		int projLoc = glGetUniformLocation(shader.id, "projection");
		glUseProgram(shader.id);
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));


		//// TODO(ck): Draw tilemap function that we pass the tilemap into
		for (unsigned int i = 0; i < world->map->tiles.size(); ++i)
		{
			int tileId = world->map->tiles[i].tileId;
			MonGL::gl_DrawTile(&shader, &world->map->tiles[i], &world->map->sheet.sprites[tileId]);
		}

		for (unsigned int i = 0; i < world->entities.size(); ++i)
		{
			//state->world->entities[i]->pos.x *= time;
			//state->world->entities[i]->pos.y *= time;

			MonGL::gl_DrawObject(&shader, world->entities[i]);
		}
		MonGL::gl_DrawObject(&shader, world->player);
		
		// gui

		

	}
}