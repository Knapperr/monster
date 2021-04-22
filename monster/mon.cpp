#include "mon.h"

#include "handmade_random.h"


bool Game::init()
{
	float vertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f };

	
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	
	glBindVertexArray(VAO);
	// Position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Normals
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);


	LoadShader(&shader, "res/shaders/vert_colors.glsl", "res/shaders/frag_colors.glsl", NULL);


	cam = Camera();
	pos = glm::vec3(-0.5f, -0.5f, -0.5f);


	light = {};
	glm::vec3 lightColor = glm::vec3(0.2f, 0.3f, 0.6f);
	light.diffuse = lightColor * glm::vec3(0.5f);
	light.ambient = light.diffuse * glm::vec3(0.2f);
	light.specular = glm::vec3(1.0f, 1.0f, 1.0f);
	light.pos = glm::vec3(1.2f, 1.0f, 2.0f);

	material = {};
	material.ambient = glm::vec3(1.0f, 0.5f, 0.31f);
	material.diffuse = glm::vec3(1.0f, 0.5f, 0.31f);
	material.specular = glm::vec3(0.5f, 0.5f, 0.5f);
	material.shininess = 32.0f;
	
	return true;
}

void Game::update(double dt, Input* input)
{
	//if (input->moveUp) world->player->pos.y -= (int)(world->player->speed * dt);
	//if (input->moveDown) world->player->pos.y += (int)(world->player->speed * dt);
	//if (input->moveLeft) world->player->pos.x -= (int)(world->player->speed * dt);
	//if (input->moveRight) world->player->pos.x += (int)(world->player->speed * dt);
	


	cam.update(dt, input, true);
}

void Game::render(double dt)
{
	// light & material
	glUniform3fv(glGetUniformLocation(shader.id, "light.pos"), 1, &light.pos[0]);
	glUniform3fv(glGetUniformLocation(shader.id, "light.ambient"), 1, &light.ambient[0]);
	glUniform3fv(glGetUniformLocation(shader.id, "light.diffuse"), 1, &light.diffuse[0]);
	glUniform3fv(glGetUniformLocation(shader.id, "light.specular"), 1, &light.specular[0]);

	glUniform3fv(glGetUniformLocation(shader.id, "material.ambient"), 1, &material.ambient[0]);
	glUniform3fv(glGetUniformLocation(shader.id, "material.diffuse"), 1, &material.diffuse[0]);
	glUniform3fv(glGetUniformLocation(shader.id, "material.specular"), 1, &material.specular[0]);
	glUniform1f(glGetUniformLocation(shader.id, "material.shininess"), material.shininess);

	glUniform3fv(glGetUniformLocation(shader.id, "viewPos"), 1, &cam.pos[0]);

	//projection = glm::perspective(glm::radians(cam.zoom), 1440.0f / 720.0f, 0.1f, 100.0f);
	glm::mat4 projection = glm::perspective(glm::radians(cam.zoom), 960.0f / 540.0f, 0.1f, 100.0f);
	glm::mat4 view = cam.viewMatrix();

	glUseProgram(shader.id);
	glUniformMatrix4fv(glGetUniformLocation(shader.id, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(glGetUniformLocation(shader.id, "view"), 1, GL_FALSE, glm::value_ptr(view));

	// world transform
	glm::mat4 model = glm::mat4(1.0f);

	glm::mat4 translate = glm::translate(glm::mat4(1.0f),
											glm::vec3(pos.x, pos.y, pos.z));
	model = model * translate;
	
	glUniformMatrix4fv(glGetUniformLocation(shader.id, "model"), 1, GL_FALSE, glm::value_ptr(model));

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);


}

void Game::cleanUp()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	DeleteShader(&shader);
}


/// 
/// 2D CODE
/// 

/*
void Game::Init(int x)
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

}

void Game::Update(double dt, Input* input, int x)
{
	//if (state->deltaTime != dt)
		//state->deltaTime = dt;

	if (input->up.endedDown)    world->player->pos.y -= (int)(world->player->speed * dt);
	if (input->down.endedDown)  world->player->pos.y += (int)(world->player->speed * dt);
	if (input->left.endedDown)  world->player->pos.x -= (int)(world->player->speed * dt);
	if (input->right.endedDown) world->player->pos.x += (int)(world->player->speed * dt);

	// TODO(ck): Update camera pos
	// camera position = player position

	real32 playerGroundPointX = screenCenterX + metersToPixels * diff.dX;
	real32 playerGroundPointY = screenCenterY - metersToPixels * diff.dY;



	//const unsigned int SCREEN_WIDTH = 1280;
	//const unsigned int SCREEN_HEIGHT = 720;
	camera.target = world->player->pos;
}

void Game::Render()
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
		gl_DrawTile(&shader, &world->map->tiles[i], &world->map->sheet.sprite);
	}

	for (unsigned int i = 0; i < world->entities.size(); ++i)
	{
		//state->world->entities[i]->pos.x *= time;
		//state->world->entities[i]->pos.y *= time;

		gl_DrawObject(&shader, world->entities[i]);
	}
	gl_DrawObject(&shader, world->player);
	//gl_DrawObject(&shader, &newBall);

	//DrawCollider(960 / 2, 540 / 2, 0, 120, 360);

}
*/

