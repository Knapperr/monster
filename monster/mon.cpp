#include "mon.h"

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
		MonGL::gl_InitBoundingBox(&player.data);
		MonGL::gl_InitBoundingBox(&player.colliderData);
		player.particle.pos = glm::vec3(10.0f, 10.0f, 20.0f);
		player.particle.inverseMass = 10.0f;
		player.particle.velocity = glm::vec3(0.0f, 0.0f, 0.0f); // 35m/s
		player.particle.acceleration = glm::vec3(-40.0f, 0.0f, 0.0f);
		player.particle.damping = 0.99f;

		player.data.mat.ambient = glm::vec3(1.0f, 0.5f, 0.6f);
		player.data.mat.diffuse = glm::vec3(1.0f, 0.5f, 0.31f);
		player.data.mat.specular = glm::vec3(0.5f, 0.5f, 0.5f);
		player.data.mat.shininess = 32.0f;

		for (int i = 0; i < 4; ++i)
		{
			EntityTwo entity = {};
			MonGL::gl_InitBoundingBox(&entity.colliderData);
			entity.particle.pos = glm::vec3(5.0f * i, 0.1f, 1.5f * i);
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
		terrain->generate();


		simulate = false;

		return true;
	}

	void Game::update(double dt, Input* newInput)
	{
		if (dt > deltaTime)
			printf("HIGHER dt: %f\n", deltaTime);
		if (dt < deltaTime)
			printf("LOWER dt: %f\n", deltaTime);

		//if (!newInput->leftMouseButton.endedDown)
			//printf("MOUSE LET GO\n");
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


				//player.particle.velocity.x = 0.0f;
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


				//player.particle.velocity.z = 0.0f;

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

		
		// set the sampler to the correct texture unit
		//glUniform1i(glGetUniformLocation(terrain->shader->id, "texture_diffuse1"), 0);
		//if (terr->drawTexture)
		//{
		//	glBindTexture(GL_TEXTURE_2D, terr->selectedTextureId);
		//}
		

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
		MonGL::gl_DrawBoundingBox(&player.colliderData, player.particle.pos, cam.pos, projection, view, shader.id);

		for (auto& e : enemies)
		{
			MonGL::gl_DrawBoundingBox(&e.colliderData, e.particle.pos, cam.pos, projection, view, shader.id);
		}
		// ------------------------------------------

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
		deltaTime = dt;
		this->input = *input;

		//if (state->deltaTime != dt)
			//state->deltaTime = dt;
		float newSpeed = world->player->speed;
		if (input->shift.endedDown)
			newSpeed *= 1.7;

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

		// TEST DRAW
		glUseProgram(tileShader.id);
		int projLoc = glGetUniformLocation(tileShader.id, "projection");
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		//// TODO(ck): Draw tilemap function that we pass the tilemap into
		
		// yeah drawtilemap just sends all of the vertices and everything then
		// calls the renderer inside? or do we just call the renderer here
		// update tilemap will fill the vertices?
		//MonGL::drawTileMap();
		//for (unsigned int i = 0; i < world->map->tiles.size(); ++i)
		//{
		//	//MonGL::drawTile(&tileShader, world->map->tiles[i].offsetX, world->map->tiles[i].offsetY);
		//	//void drawTile(MonShader::Shader * shader, BatchData * batch, int tileOffsetX, int tileOffsetY);
		//}
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