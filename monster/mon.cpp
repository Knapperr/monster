#include "mon.h"

#include "handmade_random.h"


void Game::init()
{
	float vertices[] = {
		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,

		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,

		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,

		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
	};

	
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	
	glBindVertexArray(VAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);


	LoadShader(&shader, "res/shaders/vert_colors.glsl", "res/shaders/frag_colors.glsl", NULL);

	cam = Camera();
	pos = glm::vec3(-0.5f, -0.5f, -0.5f);
}

void Game::update(double dt, Input* input)
{
	//if (input->moveUp) world->player->pos.y -= (int)(world->player->speed * dt);
	//if (input->moveDown) world->player->pos.y += (int)(world->player->speed * dt);
	//if (input->moveLeft) world->player->pos.x -= (int)(world->player->speed * dt);
	//if (input->moveRight) world->player->pos.x += (int)(world->player->speed * dt);
	
	// TODO(ck): Send through array or something of directions to the camera update 
	// loop through our input?
	// SEND THROUGH ARRAY OF DIRECTIONS then that way you can have cleaner movement

	cam.update(dt, input, true);
}

void Game::render(double dt)
{
	//projection = glm::perspective(glm::radians(cam.zoom), 1440.0f / 720.0f, 0.1f, 100.0f);
	glm::mat4 projection = glm::perspective(glm::radians(cam.zoom), 960.0f / 540.0f, 0.1f, 100.0f);
	glm::mat4 view = cam.viewMatrix();

	glUseProgram(shader.id);
	glUniformMatrix4fv(glGetUniformLocation(shader.id, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(glGetUniformLocation(shader.id, "view"), 1, GL_FALSE, glm::value_ptr(view));

	// colours
	glUniform3f(glGetUniformLocation(shader.id, "objectColor"), 1.0f, 0.5f, 0.31f);
	glUniform3f(glGetUniformLocation(shader.id, "lightColor"), 1.0f, 1.0f, 1.0f);

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
