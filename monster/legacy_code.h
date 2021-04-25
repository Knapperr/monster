#pragma once

/*
struct CameraTwo
{
	glm::vec2 pos;
	glm::vec2 target;
	float zoom;
};

/// 
/// 3D CODE
///

/*

struct Light
{
	glm::vec3 pos;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
};

// Get to this starts pg49 Ian Millington
struct Particle
{
	glm::vec3 pos;
	glm::vec3 velocity;
	glm::vec3 acceleration;

	float damping;
	float inverseMass;

	void integrate(float duration);
	void clearAccumulator();
};

struct Player
{
	Particle particle;
	MonGL::RenderData data;
};


*/

/*
struct Light
{
	glm::vec3 pos;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
};

// Get to this starts pg49 Ian Millington
struct Particle
{
	glm::vec3 pos;
	glm::vec3 velocity;
	glm::vec3 acceleration;

	float damping;
	float inverseMass;

	void integrate(float duration);
	void clearAccumulator();
};

struct Player
{
	Particle particle;
	MonGL::RenderData data;
};

struct CameraTwo
{
	glm::vec2 pos;
	glm::vec2 target;
	float zoom;
};

class Game
{
public:
	bool init();
	void update(double dt, Input* input);
	void render(double dt);

	void update(double dt, Input* input, int x);
	void render();
	bool init(int x);

	void cleanUp();

	// TODO(ck):
	// DO NOT KEEP the shaders in the game and renderer like you did last time...
	// keep them in a structure that can be accessed globally instead keep it clean
	World* world;
	Terrain* terrain;

	Camera cam;
	CameraTwo camera;
	Input input;

	Player player;
	bool simulate;
	MonShader::Shader shader;

	Light light;

};

#define real_pow powf
void Particle::integrate(float duration)
{
	if (inverseMass <= 0.0f)
		return;

	// assert(duration > 0.0);
	x += vector.x * scale;
	y += vector.y * scale;
	z += vector.z * scale;

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

	
	// set the sampler to the correct texture unit
	glUniform1i(glGetUniformLocation(terr->shader->id, "texture_diffuse1"), 0);
	if (terr->drawTexture)
	{
		glBindTexture(GL_TEXTURE_2D, terr->selectedTextureId);
	}

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

*/

/*

///
/// 2D CODE
///

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



/* IMPORTANT(ck): OLD ENTRY POINT */

// app->CleanUp();

//Platform* platform = new SDLPlatform();

//if (!platform->Init())
//	return EXIT_FAILURE;


//InitGui(platform->window, &platform->context);

//SDL_SetWindowTitle(platform->window, "Monster");
//g_Game->Init();


//Input input[2] = {};
//Input* newInput = &input[0];
//Input* oldInput = &input[1];

//// NOTE(ck): PLATFROM Layer has timing .. 
//// 	   platform::Time() gets SDL_GetTicks()
////	   platform::PerformanceCounter(); 
//// -------------------------------------------
//uint32_t currTime = SDL_GetPerformanceCounter();
//uint32_t last = 0;
//double dt = 0;

//Timer timer;
//double updateRate = 60.00;
//int updateMultiplicity = 1;
//bool unlockFrameRate = false;

//double fixedDeltaTime = 1.0 / updateRate;
//int64_t desiredFrameTime = SDL_GetPerformanceFrequency() / updateRate;

//int64_t vsyncMaxError = SDL_GetPerformanceFrequency() * 0.0002;
//int64_t time60hz = SDL_GetPerformanceFrequency() / 60;
//int64_t snapFrequencies[] =
//{
//	time60hz,
//	time60hz * 2,		// 30fps
//	time60hz * 3,		// 20fps
//	time60hz * 4,		// 15fps
//	(time60hz + 1) / 2, // 120fps
//};

//// TODO(ck):
//// Should use a ring buffer for this
//const int TIME_HISTORY_COUNT = 4;
//int64_t timeAverager[TIME_HISTORY_COUNT] = { desiredFrameTime, desiredFrameTime, desiredFrameTime, desiredFrameTime };

//// TODO(ck):
//// should be in an App class that uses the platform layer
//bool running = true;
//bool resync = true;
//int64_t prevFrameTime = SDL_GetPerformanceCounter();
//int64_t frameAccumulator = 0;
//// ------------------------------------------------------------------------------------------------------------------------

//while (running)
//{
//	int64_t currentFrameTime = SDL_GetPerformanceCounter();
//	int64_t deltaTime = currentFrameTime - prevFrameTime;
//	prevFrameTime = currentFrameTime;

//	// handle unexpected timer anomolies (overflow, extra slow frames, etc)
//	if (deltaTime > desiredFrameTime * 8) // ignore extra slow frames
//	{
//		deltaTime = desiredFrameTime;
//	}
//	if (deltaTime < 0)
//	{
//		deltaTime = 0;
//	}

//	// vsync time snapping
//	for (int64_t snap : snapFrequencies)
//	{
//		if (std::abs(deltaTime - snap) < vsyncMaxError)
//		{
//			deltaTime = snap;
//			break;
//		}
//	}

//	// delta time averaging 
//	for (int i = 0; i < TIME_HISTORY_COUNT - 1; ++i)
//	{
//		timeAverager[i] = timeAverager[i + 1];
//	}
//	timeAverager[TIME_HISTORY_COUNT - 1] = deltaTime;
//	deltaTime = 0;
//	for (int i = 0; i < TIME_HISTORY_COUNT; ++i)
//	{
//		deltaTime = timeAverager[i];
//	}
//	deltaTime /= TIME_HISTORY_COUNT;

//	// add to the accumulator 
//	frameAccumulator += deltaTime;

//	// spriral of death protection
//	if (frameAccumulator > desiredFrameTime * 8)
//	{
//		resync = true;
//	}

//	// timer resync if requested
//	if (resync)
//	{
//		frameAccumulator = 0;
//		deltaTime = desiredFrameTime;
//		resync = false;
//	}

//	// NOTE(ck): Pass input manager to this?
//	platform->PollInput(newInput, oldInput);

//	if (unlockFrameRate)
//	{
//		int64_t consumedDeltaTime = deltaTime;

//		while (frameAccumulator >= desiredFrameTime) {
//			g_Game->Update(fixedDeltaTime, newInput);

//			if (consumedDeltaTime > desiredFrameTime)
//			{
//				// whatever this means
//				// g_Game->VariableUpdate()
//				g_Game->Update(fixedDeltaTime, newInput);
//				consumedDeltaTime -= desiredFrameTime;
//			}
//			frameAccumulator -= desiredFrameTime;
//		}

//		// g_Game->VariableUpdate();
//		g_Game->Update((double)consumedDeltaTime / SDL_GetPerformanceFrequency(), newInput);
//		UpdateGui(platform->window);

//		glClearColor(0.126f, 0.113f, 0.165f, 1.0f);
//		glClear(GL_COLOR_BUFFER_BIT);
//		g_Game->Render(fixedDeltaTime);
//		RenderGui();

//		SDL_GL_SwapWindow(platform->window);


//		Input* temp = newInput;
//		newInput = oldInput;
//		oldInput = temp;

//	}
//	else
//	{
//		//while (frameAccumulator >= desiredFrameTime * updateMultiplicity)
//		//{
//		//	for (int i = 0; i < updateMultiplicity; ++i)
//		//	{
//		//		g_Game->Update(fixedDeltaTime, newInput);
//		//		// g_Game->VariableUpdate
//		//		frameAccumulator -= desiredFrameTime;
//		//	}
//		//}
//		g_Game->Update(fixedDeltaTime, newInput);

//		UpdateGui(platform->window);

//		glClearColor(0.126f, 0.113f, 0.165f, 1.0f);
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//		g_Game->Render(1);
//		//g_Game->Render();

//		RenderGui();

//		SDL_GL_SwapWindow(platform->window);

//		Input* temp = newInput;
//		newInput = oldInput;
//		oldInput = temp;
//	}


//}
//g_Game->CleanUp();
//ShutdownGui();
//SDL_GL_DeleteContext(platform->context);
//SDL_DestroyWindow(platform->window);
//SDL_Quit();
//exit(EXIT_SUCCESS);
