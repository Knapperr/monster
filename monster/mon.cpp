#include "mon.h"

#include "handmade_random.h"


bool Game::init(int x)
{
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

	if (input->up.endedDown)    world->player->pos.y -= (int)(world->player->speed * dt);
	if (input->down.endedDown)  world->player->pos.y += (int)(world->player->speed * dt);
	if (input->left.endedDown)  world->player->pos.x -= (int)(world->player->speed * dt);
	if (input->right.endedDown) world->player->pos.x += (int)(world->player->speed * dt);

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
		MonGL::gl_DrawTile(&shader, &world->map->tiles[i], &world->map->sheet.sprite);
	}

	for (unsigned int i = 0; i < world->entities.size(); ++i)
	{
		//state->world->entities[i]->pos.x *= time;
		//state->world->entities[i]->pos.y *= time;

		MonGL::gl_DrawObject(&shader, world->entities[i]);
	}
	MonGL::gl_DrawObject(&shader, world->player);
	//gl_DrawObject(&shader, &newBall);

	//DrawCollider(960 / 2, 540 / 2, 0, 120, 360);

}

void Game::cleanUp()
{
	// TODO(ck): clean up render data 
	//glDeleteVertexArrays(1, &player.data.VAO);
	//glDeleteBuffers(1, &player.data.VBO);

	MonShader::DeleteShader(&shader);
}