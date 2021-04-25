#include "mon.h"

#include "handmade_random.h"


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
	//float vertices[] = {
	//	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	//	 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	//	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	//	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	//	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	//	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

	//	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	//	 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	//	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	//	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	//	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	//	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

	//	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	//	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	//	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	//	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	//	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	//	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

	//	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	//	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	//	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	//	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	//	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	//	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

	//	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	//	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	//	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	//	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	//	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	//	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

	//	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	//	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	//	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	//	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	//	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	//	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f };

	//
	//glGenVertexArrays(1, &VAO);
	//glGenBuffers(1, &VBO);
	//glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//
	//glBindVertexArray(VAO);
	//// Position
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	//glEnableVertexAttribArray(0);

	//// Normals
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	//glEnableVertexAttribArray(1);

	


	shader = {};
	MonShader::LoadShader(&shader, "res/shaders/vert_colors.glsl", "res/shaders/frag_colors.glsl", NULL);

	cam = Camera();

	player = {};
	MonGL::gl_InitRenderDataCube(&player.data);
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

	if (cam.disabled == true)
	{
		if (input->up.endedDown)    player.particle.pos.z -= 6 * dt;
		if (input->down.endedDown)  player.particle.pos.z += 6 * dt;
		if (input->left.endedDown)  player.particle.pos.x -= 6 * dt;
		if (input->right.endedDown) player.particle.pos.x += 6 * dt;
	}
	cam.update(dt, input, true);

	if (simulate == true)
		player.particle.integrate(dt);
}

void Game::render(double dt)
{
	//projection = glm::perspective(glm::radians(cam.zoom), 1440.0f / 720.0f, 0.1f, 100.0f);
	glm::mat4 projection = glm::perspective(glm::radians(cam.zoom), 960.0f / 540.0f, 0.1f, 1000.0f);
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


	glUniformMatrix4fv(glGetUniformLocation(shader.id, "model"), 1, GL_FALSE, glm::value_ptr(matModel));
	glBindVertexArray(terrain->VAO);

	terrain->wireFrame ?
		  glDrawElements(GL_LINES, terrain->indicesLength, GL_UNSIGNED_INT, 0)
		: glDrawElements(GL_TRIANGLES, terrain->indicesLength, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
	// Always good practice to set everything back to defaults once configured
	// NOTE(CK): bind texture must be AFTER glActiveTexture or it will not unbind properly
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, 0);
// ------------------------------------------



// ------------------------------------------
	// TODO(ck): use shader
	glUseProgram(shader.id);
	// light & material
	glUniform3fv(glGetUniformLocation(shader.id, "light.pos"), 1, &light.pos[0]);
	glUniform3fv(glGetUniformLocation(shader.id, "light.ambient"), 1, &light.ambient[0]);
	glUniform3fv(glGetUniformLocation(shader.id, "light.diffuse"), 1, &light.diffuse[0]);
	glUniform3fv(glGetUniformLocation(shader.id, "light.specular"), 1, &light.specular[0]);

	MonGL::gl_DrawCube(&player.data, player.particle.pos, cam.pos, projection, view, shader.id);
// ------------------------------------------


}

void Game::cleanUp()
{
	// TODO(ck): clean up render data 
	glDeleteVertexArrays(1, &player.data.VAO);
	glDeleteBuffers(1, &player.data.VBO);

	MonShader::DeleteShader(&shader);
}