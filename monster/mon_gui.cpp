#include "mon_gui.h"

#include <string>

// TODO(ck): open files with current platform layer
#include <fstream>
#ifdef _3D_GUI_
void writeEntities(Mon::Entity* entities, int shaderID)
{
	std::ofstream file;
	file.open("scene_1.txt");

	for (int i = 0; i < ArrayCount(&entities); ++i)
	{
		file << entities[i].name << "\n"
			<< entities[i].particle.pos.x << "\n" << entities[i].particle.pos.y << "\n" << entities[i].particle.pos.z << "\n"
			<< shaderID << "\n"
			<< entities[i].data.texturePath << "\n";
	}

	file.close();
}

void LoadImpFile(Mon::Game* game)
{
	std::ifstream file("player.imp");
	if (!file.is_open())
	{
		Mon::Log::print("Failure to open file");
	}

	// TODO(ck): RESEARCH(ck):
	// This loads ONE model inside of the file right now. The imp asset file needs to be worked out 

	std::string line;
	// Create new entity and grab it to get it ready
	Mon::AddEntity(game->world);
	Mon::Entity* e = Mon::GetEntity(game->world, game->world->entityCount - 1);
	while (file >> line)
	{
		// Get the name and the size of the vertices & indices here
		e->name = line;
		file >> e->data.verticeCount;
		file >> e->data.indiceCount;
		break;
	}

	e->data.vertices = new MonGL::Vertex3D[e->data.verticeCount];
	e->data.indices = new unsigned int[e->data.indiceCount];

	while (file >> line)
	{
		for (int i = 0; i < e->data.verticeCount; ++i)
		{
			if (i == 0)
				e->data.vertices[i].position.x = std::stof(line);
			else
				file >> e->data.vertices[i].position.x;

			file >> e->data.vertices[i].position.y;
			file >> e->data.vertices[i].position.z;
			file >> e->data.vertices[i].normal.x;
			file >> e->data.vertices[i].normal.y;
			file >> e->data.vertices[i].normal.z;
			file >> e->data.vertices[i].texCoords.x;
			file >> e->data.vertices[i].texCoords.y;
			file >> e->data.vertices[i].tangent.x;
			file >> e->data.vertices[i].tangent.y;
			file >> e->data.vertices[i].tangent.z;
			file >> e->data.vertices[i].bitangent.x;
			file >> e->data.vertices[i].bitangent.y;
			file >> e->data.vertices[i].bitangent.z;
		}
		for (int j = 0; j < e->data.indiceCount; ++j)
		{
			file >> e->data.indices[j];
		}
	}
	file.close();

	MonGL::InitModel(&e->data);
	MonGL::LoadTexture(&e->data, 0, MonGL::TextureType::Diffuse, game->shader.handle, "res/textures/grass.png", false);
	e->particle.pos = Mon::v3(10.0f, 0.3f, 20.0f);
	MonGL::InitBoundingBox(&e->collider.data);
}

void writeImpFile(Mon::Game* game)
{
	// write the selected index to a file....
	// this will be a good way to see if the file data is loaded properly.
	Mon::Entity* e = Mon::GetEntity(game->world, game->selectedIndex);
	std::string path = "test_data.imp";
	std::ofstream file;
	file.open(path);

	file << e->name << "\n";
	file << e->data.verticeCount << "\n";
	file << e->data.indiceCount << "\n";

	for (int i = 0; i < e->data.verticeCount; ++i)
	{
	   file << e->data.vertices[i].position.x << "\n" << e->data.vertices[i].position.y << "\n" << e->data.vertices[i].position.z << "\n"
			<< e->data.vertices[i].normal.x << "\n" << e->data.vertices[i].normal.y << "\n" << e->data.vertices[i].normal.z << "\n"
			<< e->data.vertices[i].texCoords.x << "\n" << e->data.vertices[i].texCoords.y << "\n"
			<< e->data.vertices[i].tangent.x << "\n" << e->data.vertices[i].tangent.y << "\n" << e->data.vertices[i].tangent.z << "\n"
			<< e->data.vertices[i].bitangent.x << "\n" << e->data.vertices[i].bitangent.y << "\n" << e->data.vertices[i].bitangent.z << "\n";
	}
	for (int j = 0; j < e->data.indiceCount; ++j)
	{
		file << e->data.indices[j] << "\n";
	}
}

void loadEntities(Mon::Game* game)
{
	// TODO(ck): memory management should probably have something in the list to only reload part of it
	// READ MEMORY FROM FILE
	//game->entities.clear();
	std::string line;
	std::ifstream file("scene_1.txt");
	if (!file.is_open())
	{
		printf("Failure to open file!");
	}
	while (file >> line)
	{
		Mon::Entity e = {};
		e.name = line;
		file >> e.particle.pos.x;
		file >> e.particle.pos.y;
		file >> e.particle.pos.z;

		// TODO(ck): This is why a raw array needs to be used
		// the render data needs to be rebuilt. Serialization I think 
		// will take care of having to reload any rendering data.
		int shaderID = 0;
		std::string textPath = "";
		file >> shaderID;
		file >> textPath;

		MonGL::InitQuad(&e.data);
		MonGL::LoadTexture(&e.data, 0, MonGL::TextureType::Diffuse, shaderID, textPath);
		//game->entities.push_back(e);
	}
}
#endif

#ifdef USE_SDL

const char* GetSwapInterval()
{
	switch (SDL_GL_GetSwapInterval())
	{
	case 0:
		return "off";
	case 1:
		return "synchronized";
	case -1:
		return "adaptive";
	}
	return "not found";
}


void InitGui(SDL_Window* window, SDL_GLContext* context)
{
	const char* glsl_version = "#version 130";

	// set up
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;


	/*ImGui::GetStyle().WindowRounding = 0.0f;
	ImGui::GetStyle().AntiAliasedFill = false;
	ImGui::GetStyle().AntiAliasedLines = false;*/

	//ImGui::SetMouseCursor(ImGuiMouseCursor_None);
	io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer bindings
	ImGui_ImplSDL2_InitForOpenGL(window, context);
	ImGui_ImplOpenGL3_Init(glsl_version);
}


#ifdef _3D_GUI_
static void TerrainColliderWindow(Terrain* terrain)
{
	if (ImGui::TreeNode("Colliders"))
	{

		ImGui::Checkbox("draw collider", &terrain->collider.data.visible);


		if (ImGui::SliderFloat3("collider min", &terrain->collider.size.min[0], 0.0f, 500.0f))
		{
			Mon::SetBoxTransform(&terrain->collider, terrain->collider.worldPos, Mon::v3(1.0f));
		}
		if (ImGui::SliderFloat3("collider max", &terrain->collider.size.max[0], 0.0f, 500.0f))
		{
			Mon::SetBoxTransform(&terrain->collider, terrain->collider.worldPos, Mon::v3(1.0f));
		}
		if (ImGui::DragFloat("collider x", &terrain->collider.worldPos.x, 0.1f, -1000.0f, 1000.0f, "%.02f"))
		{
			Mon::SetBoxTransform(&terrain->collider, terrain->collider.worldPos, Mon::v3(1.0f));
		}
		if (ImGui::DragFloat("collider y", &terrain->collider.worldPos.y, 0.1f, -1000.0f, 1000.0f, "%.02f"))
		{
			Mon::SetBoxTransform(&terrain->collider, terrain->collider.worldPos, Mon::v3(1.0f));
		}
		if (ImGui::DragFloat("collider z", &terrain->collider.worldPos.z, 0.1f, -1000.0f, 1000.0f, "%.02f"))
		{
			Mon::SetBoxTransform(&terrain->collider, terrain->collider.worldPos, Mon::v3(1.0f));
		}


		// End
		ImGui::TreePop();
	}
}

static void TerrainMaterialWindow(Terrain* terrain)
{
	if (ImGui::TreeNode("Materials"))
	{
		ImGui::PushItemWidth(100.0f);
		ImGui::DragFloat("Mat diffuse r", &terrain->mesh.mat.diffuse.x, 0.01f, 0.0f, 1.0f, "%.02f");
		ImGui::DragFloat("Mat diffuse g", &terrain->mesh.mat.diffuse.y, 0.01f, 0.0f, 1.0f, "%.02f");
		ImGui::DragFloat("Mat diffuse b", &terrain->mesh.mat.diffuse.z, 0.01f, 0.0f, 1.0f, "%.02f");
		ImGui::Separator();
		ImGui::DragFloat("Mat ambient r", &terrain->mesh.mat.ambient.x, 0.01f, 0.0f, 1.0f, "%.02f");
		ImGui::DragFloat("Mat ambient g", &terrain->mesh.mat.ambient.y, 0.01f, 0.0f, 1.0f, "%.02f");
		ImGui::DragFloat("Mat ambient b", &terrain->mesh.mat.ambient.z, 0.01f, 0.0f, 1.0f, "%.02f");
		ImGui::PopItemWidth();


		ImGui::TreePop();
	}
}

void TerrainWindow(bool* p_open, Mon::Game* game)
{
	ImGui::Begin("Terrain", p_open);

	ImGui::Text("Texture");
	ImGui::Separator();
	if (ImGui::Button("UV")) { game->terrain->mesh.selectedTexture = 0; }
	ImGui::SameLine();
	if (ImGui::Button("Grass")) { game->terrain->mesh.selectedTexture = 1; }
	ImGui::SameLine();
	if (ImGui::Button("Pixel Grass")) { game->terrain->mesh.selectedTexture = 2; }
	ImGui::SameLine();
	if (ImGui::Button("Snow")) { game->terrain->mesh.selectedTexture = 3; }
	ImGui::Separator();

	ImGui::Checkbox("Wireframe", &game->terrain->wireFrame);

	TerrainColliderWindow(game->terrain);
	TerrainMaterialWindow(game->terrain);


	ImGui::End();
}

void CameraWindow(bool* p_open, Mon::Game* game)
{
	ImGui::Begin("Cammy", p_open);

	//float velocity = game->cam->speed * game->deltaTime;
	
	ImGui::PushButtonRepeat(true);
	ImGui::Indent(32);
	//if (ImGui::ArrowButton("##up", ImGuiDir_Up))
	//	&game->cam->pos += &game->cam->front * velocity;	
	//ImGui::Unindent(32);

	//if (ImGui::ArrowButton("##left", ImGuiDir_Left))
	//	&game->cam->pos -= game->cam->right * velocity;
	//ImGui::SameLine(30, 40);
	//if (ImGui::ArrowButton("##right", ImGuiDir_Right))
	//	&game->cam->pos += &game->cam->right * velocity;

	//ImGui::Indent(32);
	//if (ImGui::ArrowButton("##down", ImGuiDir_Down))
	//	&game->cam->pos -= &game->cam->front * velocity;
	ImGui::Unindent(32);
	ImGui::PopButtonRepeat();

	ImGui::LabelText(game->cameras[game->currCameraIndex].name, "Current Camera");
	if (ImGui::ArrowButton("##left", ImGuiDir_Left))
	{
		if (game->currCameraIndex > 1)
			game->currCameraIndex--;

		if (game->cameras[game->currCameraIndex].type == Mon::CameraType::Follow)
			game->playMode();
		else
			game->debugMode();
	}
	ImGui::SameLine();
	if (ImGui::ArrowButton("##right", ImGuiDir_Right))
	{
		if (game->currCameraIndex < (game->cameraCount-1))
			game->currCameraIndex++;

		if (game->cameras[game->currCameraIndex].type == Mon::CameraType::Follow)
			game->playMode();
		else
			game->debugMode();
	}

	ImGui::Separator();

	ImGui::DragFloat("cam zoom", &game->cameras[game->currCameraIndex].zoom, 0.1f, -1000.0f, 1000.0f, "%.02f");

	ImGui::DragFloat("near plane", &game->cameras[game->currCameraIndex].nearPlane, 0.01f, 0.1f, 100.0f, "%.02f");
	ImGui::DragFloat("far plane", &game->cameras[game->currCameraIndex].farPlane, 0.5f, 100.0f, 1000.0f, "%.02f");
	
	ImGui::SliderFloat("pitch", &game->cameras[game->currCameraIndex].pitch, -1.10f, 100.0f, "%1.0f");
	//ImGui::SliderFloat("angle", &game->cameras[game->currCameraIndex].angleAroundTarget, -360.0f, 180.0f);
	ImGui::SliderFloat("lerp", &game->cameras[game->currCameraIndex].lerpSpeed, 0.0f, 100.0f);
	ImGui::SliderFloat("smooth", &game->cameras[game->currCameraIndex].smoothness, 0.1f, 10.0f);


	if (ImGui::Button("Log"))
	{ 
		Mon::Log::print("cam zoom", game->cameras[game->currCameraIndex].zoom);
		Mon::Log::print("near plane", game->cameras[game->currCameraIndex].nearPlane);
		Mon::Log::print("far plane", game->cameras[game->currCameraIndex].farPlane);
		Mon::Log::print("pitch", game->cameras[game->currCameraIndex].pitch);
		//Mon::Log::print("angle around target", game->cameras[game->currCameraIndex].angleAroundTarget);
		//Mon::Log::print("lerp speed", game->cameras[game->currCameraIndex].lerpSpeed);
		//Mon::Log::print("smoothness", game->cameras[game->currCameraIndex].smoothness);
	}

	ImGui::End();
}

void EntityWindow(bool* p_open, Mon::Game* game)
{
	ImGui::Begin("entities and things", p_open);
	
	ImGui::DragFloat("sprite angle", &game->config->angleDegrees, 0.10f, -180.0f, 360.0f, "%.10f");
	if (ImGui::Button("Add Cube"))
	{
		Mon::AddCube(game->world, game->shader.handle);
	}
	ImGui::SameLine();
	if (ImGui::Button("Load imp file"))
	{
		LoadImpFile(game);
	}
	
	// NOTE(ck): Write the .imp model to a file 
	/*if (ImGui::Button("Write imp file"))
	{
		writeImpFile(game);
	}*/

	static unsigned int selected = 1;
	ImGui::BeginChild("left pane", ImVec2(150.0f, 0.0f), true);

	for (unsigned int i = 1; i < game->world->entityCount; ++i)
	{
		char label[128];
		sprintf_s(label, "%s %d", game->world->entities[i].name.c_str(), i);
		if (ImGui::Selectable(label, selected == i))
		{
			selected = i;
			game->selectedIndex = i;
		}
	}
	
	ImGui::EndChild();
	ImGui::SameLine();

	if (game->world->entityCount > 1)
	{
		ImGui::BeginGroup();
		ImGui::BeginChild("item view", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us
		
		ImGui::Text("%s", game->world->entities[selected].name.c_str());

		// TODO(ck): Model loading modal
		// Modal for loading meshes
		//if (ImGui::Button("Load Mesh.."))
		//	ImGui::OpenPopup("Load Mesh");
		//if (ImGui::BeginPopupModal("Load Mesh", NULL))
		//{
		//	for (unsigned int i = 0; i < objPaths.size(); i++)
		//	{
		//		if (ImGui::SmallButton(objPaths[i].name.c_str()))
		//		{
		//			LOG("Loading mesh...");
		//			LoadEmptyObject(selected, objPaths[i].name, objPaths[i].path);
		//			LOG("mesh loaded!");
		//			ImGui::CloseCurrentPopup();
		//		}
		//	}

		//	if (ImGui::Button("Close"))
		//		ImGui::CloseCurrentPopup();
		//	ImGui::EndPopup();
		//}
		ImGui::Separator();
		if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None))
		{
			if (ImGui::BeginTabItem("Controls"))
			{
				//ImGui::TextWrapped("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. ");
				//ImGui::SliderFloat("scale", &game->entities[selected].data.size.max, 0.0f, 200.0f);
				//ImGui::DragFloat("fine scale", &game->entities[selected].data.size, 0.0001f, 0.0f, 200.0f, "%.02f");

				ImGui::DragFloat("x", &game->world->entities[selected].particle.pos.x, 0.1f, -1000.0f, 1000.0f, "%.02f");
				ImGui::DragFloat("y", &game->world->entities[selected].particle.pos.y, 0.1f, -1000.0f, 1000.0f, "%.02f");
				ImGui::DragFloat("z", &game->world->entities[selected].particle.pos.z, 0.1f, -1000.0f, 1000.0f, "%.02f");


				ImGui::SliderFloat3("scale", &game->world->entities[selected].data.scale[0], 0.0f, 100.0f);
				ImGui::SliderFloat3("collider min", &game->world->entities[selected].collider.size.min[0], 0.0f, 100.0f);
				ImGui::SliderFloat3("collider max", &game->world->entities[selected].collider.size.max[0], 0.0f, 100.0f);
				//ImGui::DragFloat("rot x", &g_Game->objects[selected]->orientation.x, 0.05f, -1000.0f, 1000.0f, "%.02f");
				//ImGui::DragFloat("rot y", &g_Game->objects[selected]->orientation.y, 0.05f, -1000.0f, 1000.0f, "%.02f");
				//ImGui::DragFloat("rot z", &g_Game->objects[selected]->orientation.z, 0.05f, -1000.0f, 1000.0f, "%.02f");

				//ImGui::Checkbox("show normals", &g_Game->objects[selected]->viewNormals);
				//ImGui::Checkbox("wire frame", &g_Game->objects[selected]->wireFrame);
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Details"))
			{
				ImGui::Text("ID: 0123456789");
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}
			
		// TODO(ck): Delete entities
		//if (ImGui::SmallButton("DELETE"))
		//{
		//	UnloadObject(selected);
		//	// Selected is greater than size of vector
		//	// don't move down if empty
		//	if (selected >= g_Game->objects.size() && !g_Game->objects.empty())
		//		selected -= 1;
		//}
	
		ImGui::EndChild();
		ImGui::EndGroup();
	}
	ImGui::End();
}
	

void StatsWindow(bool* p_open, Mon::Game* game)
{
	ImGui::Begin("stats for me", p_open);

	char buffer[64];
	
	snprintf(buffer, sizeof(buffer), "%f", game->input.mouseOffset.x);
	ImGui::LabelText(buffer, "mouse x:");
	snprintf(buffer, sizeof(buffer), "%f", game->input.mouseOffset.y);
	ImGui::LabelText(buffer, "mouse y:");
	snprintf(buffer, sizeof(buffer), "%f", game->input.mouseScreen.x);
	ImGui::LabelText(buffer, "win x:");
	snprintf(buffer, sizeof(buffer), "%f", game->input.mouseScreen.y);
	ImGui::LabelText(buffer, "win y:");

	snprintf(buffer, sizeof(buffer), "%f", game->picker.currentTerrainPoint.y);
	ImGui::LabelText(buffer, "current terrain point y:");

	snprintf(buffer, sizeof(buffer), "%f", game->input.stickAverageX);
	ImGui::LabelText(buffer, "stick average X:");
	snprintf(buffer, sizeof(buffer), "%f", (float)game->input.stickAverageY);
	ImGui::LabelText(buffer, "stick average Y:");

	snprintf(buffer, sizeof(buffer), "%f", game->input.stickAvgRX);
	ImGui::LabelText(buffer, "right stick average X:");
	snprintf(buffer, sizeof(buffer), "%f", (float)game->input.stickAvgRY);
	ImGui::LabelText(buffer, "right stick average Y:");

	snprintf(buffer, sizeof(buffer), "%d", game->input.lMouseBtn.endedDown);
	ImGui::LabelText(buffer, "left mouse down");



	//snprintf(buffer, sizeof(buffer), "%f", game->player.particle.pos.x);
	//ImGui::LabelText(buffer, "player x");
	//snprintf(buffer, sizeof(buffer), "%f", game->player.particle.pos.y);
	//ImGui::LabelText(buffer, "player y");
	//snprintf(buffer, sizeof(buffer), "%f", game->player.particle.pos.z);
	//ImGui::LabelText(buffer, "player z");

	//snprintf(buffer, sizeof(buffer), "%f", game->player.particle.velocity.x);
	//ImGui::LabelText(buffer, "player velocity");
	//snprintf(buffer, sizeof(buffer), "%f", game->player.particle.acceleration.x);
	//ImGui::LabelText(buffer, "player acc");

	snprintf(buffer, sizeof(buffer), "%f", game->cameras[game->currCameraIndex].pos.x);
	ImGui::LabelText(buffer, "cam x");
	snprintf(buffer, sizeof(buffer), "%f", game->cameras[game->currCameraIndex].pos.y);
	ImGui::LabelText(buffer, "cam y");
	snprintf(buffer, sizeof(buffer), "%f", game->cameras[game->currCameraIndex].pos.z);
	ImGui::LabelText(buffer, "cam z");

	snprintf(buffer, sizeof(buffer), "%f", game->cameras[game->currCameraIndex].right.x);
	ImGui::LabelText(buffer, "right x");
	snprintf(buffer, sizeof(buffer), "%f", game->cameras[game->currCameraIndex].right.y);
	ImGui::LabelText(buffer, "right y");
	snprintf(buffer, sizeof(buffer), "%f", game->cameras[game->currCameraIndex].right.z);
	ImGui::LabelText(buffer, "right z");


	snprintf(buffer, sizeof(buffer), "%f", game->cameras[game->currCameraIndex].front.x);
	ImGui::LabelText(buffer, "front x");
	snprintf(buffer, sizeof(buffer), "%f", game->cameras[game->currCameraIndex].front.y);
	ImGui::LabelText(buffer, "front y");
	snprintf(buffer, sizeof(buffer), "%f", game->cameras[game->currCameraIndex].front.z);
	ImGui::LabelText(buffer, "front z");

	snprintf(buffer, sizeof(buffer), "%f", game->cameras[game->currCameraIndex].pitch);
	ImGui::LabelText(buffer, "pitch");
	snprintf(buffer, sizeof(buffer), "%f", game->cameras[game->currCameraIndex].yaw);
	ImGui::LabelText(buffer, "yaw");

	ImGui::End();
}

void UpdateGui(SDL_Window* window, Settings* settings, Mon::Game* game)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(window);

	ImGui::NewFrame();

	static bool showDemoWindow = false;
	static bool showTerrainWindow = true;
	static bool showCameraWindow = true;
	static bool showStatsWindow = false;
	static bool showEntityWindow = true;
	if (showDemoWindow)
		ImGui::ShowDemoWindow(&showDemoWindow);
	if (showTerrainWindow)
		TerrainWindow(&showTerrainWindow, game);
	if (showCameraWindow)
		CameraWindow(&showCameraWindow, game);
	if (showStatsWindow)
		StatsWindow(&showStatsWindow, game);
	if (showEntityWindow)
		EntityWindow(&showEntityWindow, game);

	ImGui::Begin("DEBUG MENU");

	ImGui::Separator();
		ImGui::PushID(0);
			ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::ImColor(170, 40, 44));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::ImColor(105, 24, 27));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::ImColor(66, 15, 16));
			if (ImGui::Button("debug")) { game->debugMode(); }
			ImGui::PopStyleColor(3);
		ImGui::PopID();
		ImGui::SameLine();

		ImGui::PushID(0);
			ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::ImColor(16, 169, 35));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::ImColor(31, 80, 18));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::ImColor(34, 64, 35));
			if (ImGui::Button("play") && game->state != Mon::State::Play) { game->playMode(); }
			ImGui::PopStyleColor(3);
		ImGui::PopID();
		if (ImGui::Button("save")) 
		{
			writeEntities(game->world->entities, game->mainShaderID); 
			Mon::Log::print("Saved game to master file");
			Mon::Log::warn("Only one master save file active!!!");
		}
		ImGui::SameLine();
		if (ImGui::Button("load")) 
		{ 
			loadEntities(game); 
			Mon::Log::print("Loaded last saved scene");
		}
		ImGui::SameLine();

		if (ImGui::Button("Fullscreen"))
		{
			SDL_DisplayMode dm;
			SDL_GetCurrentDisplayMode(0, &dm);
			SDL_SetWindowSize(window, dm.w, dm.h);
			SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
			game->setViewPort(dm.w, dm.h);
		}
		ImGui::SameLine();
		if (ImGui::Button("Exit"))
		{
			SDL_SetWindowSize(window, settings->windowWidth, settings->windowHeight);
			SDL_SetWindowFullscreen(window, 0);
			game->setViewPort(960.0f, 540.0f);
		}

		if (ImGui::Button("720"))
		{
			settings->windowWidth = 1280;
			settings->windowHeight = 720;
			SDL_SetWindowSize(window, settings->windowWidth, settings->windowHeight);
		}
		ImGui::SameLine();
		if (ImGui::Button("1440"))
		{
			settings->windowWidth = 1440;
			settings->windowHeight = 900;
			SDL_SetWindowSize(window, settings->windowWidth, settings->windowHeight);
		}
	
		ImGui::SliderFloat2("port", (float*)&game->config->viewPort, 0.0f, 477.0f);

		// NOTE(ck): SDL Vsync https://wiki.libsdl.org/SDL_GL_SetSwapInterval
		// 0 for immediate updates, 1 for updates synchronized with the vertical retrace, -1 for adaptive 
		if (ImGui::RadioButton("off", settings->vsync == 0)) 
		{
			SDL_GL_SetSwapInterval(0);
			settings->vsync = SDL_GL_GetSwapInterval();
		}
		ImGui::SameLine();
		if (ImGui::RadioButton("vertical retrace", settings->vsync == 1))
		{
			SDL_GL_SetSwapInterval(1);
			settings->vsync = SDL_GL_GetSwapInterval();

		}
		ImGui::SameLine();
		if (ImGui::RadioButton("adaptive", settings->vsync == -1))
		{
			SDL_GL_SetSwapInterval(-1);
			settings->vsync = SDL_GL_GetSwapInterval();
		}
	ImGui::Separator();

		ImGui::LabelText("dt", std::to_string(game->deltaTime).c_str());
		ImGui::LabelText("draw calls", std::to_string(MonGL::globalDrawCalls).c_str());

		ImGui::Checkbox("Demo", &showDemoWindow);
		ImGui::SameLine();
		ImGui::Checkbox("stats", &showStatsWindow);

		ImGui::Checkbox("Terrain", &showTerrainWindow);
		ImGui::SameLine();
		ImGui::Checkbox("Camera", &showCameraWindow);
		ImGui::SameLine();
		ImGui::Checkbox("Things", &showEntityWindow);

		ImGui::DragFloat("cam speed", &game->cameras[game->currCameraIndex].speed, 0.01f, 1.0f, 200.0f, "%.02f");

		/// 
		/// Player 
		///
		ImGui::LabelText("", "Player");
		if (ImGui::SmallButton("reset Pos"))
		{
			//game->player.particle.pos.y = 0.1f;
			//game->player.particle.pos.x = 10.0f;
			//game->player.particle.pos.z = 10.0f;
		}
		//ImGui::SliderFloat("speed", &game->player.particle.speed, 0.0f, 100.0f);
		//ImGui::SliderFloat3("lightpos", &game->light.pos[0], 0.0f, 500.0f);
		//ImGui::SliderFloat3("color", &game->player.collider.data.color[0], 0.0f, 1.0f);
		//ImGui::SliderFloat3("min", &game->player.collider.size.min[0], 0.0f, 50.0f);
		//ImGui::SliderFloat3("max", &game->player.collider.size.max[0], 0.0f, 50.0f);
	

		ImGui::Checkbox("simulate", &game->simulate);
		ImGui::Checkbox("draw collisions", &game->drawCollisions);

	ImGui::Separator();
	
	ImGui::End();

}

#else 
///
/// 2D
/// 

void StatsWindow(bool* p_open, Mon::Game2D* game)
{
	ImGui::Begin("stats for me", p_open);

	char buffer[64];

	snprintf(buffer, sizeof(buffer), "%f", game->input.mouseOffset.x);
	ImGui::LabelText(buffer, "mouse x");
	snprintf(buffer, sizeof(buffer), "%f", game->input.mouseOffset.y);
	ImGui::LabelText(buffer, "mouse y");
	snprintf(buffer, sizeof(buffer), "%f", game->input.mouseScreen.x);
	ImGui::LabelText(buffer, "win x");
	snprintf(buffer, sizeof(buffer), "%f", game->input.mouseScreen.y);
	ImGui::LabelText(buffer, "win y");

	snprintf(buffer, sizeof(buffer), "%f", game->input.stickAverageX);
	ImGui::LabelText(buffer, "stick average X:");
	snprintf(buffer, sizeof(buffer), "%f", (float)game->input.stickAverageY);
	ImGui::LabelText(buffer, "stick average Y:");

	snprintf(buffer, sizeof(buffer), "%f", game->input.stickAvgRX);
	ImGui::LabelText(buffer, "right stick average X:");
	snprintf(buffer, sizeof(buffer), "%f", (float)game->input.stickAvgRY);
	ImGui::LabelText(buffer, "right stick average Y:");

	snprintf(buffer, sizeof(buffer), "%d", game->input.lMouseBtn.endedDown);
	ImGui::LabelText(buffer, "left mouse down");

	snprintf(buffer, sizeof(buffer), "%f", game->camera.pos.x);
	ImGui::LabelText(buffer, "cam x");
	snprintf(buffer, sizeof(buffer), "%f", game->camera.pos.y);
	ImGui::LabelText(buffer, "cam y");

	snprintf(buffer, sizeof(buffer), "%d", GuiActive(false));
	ImGui::LabelText(buffer, "gui active");

	ImGui::End();
}

void UpdateGui(SDL_Window* window, Settings* settings, Mon::Game2D* game2D)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(window);
	ImGui::NewFrame();

	static bool showDemoWindow = false;
	static bool showTerrainWindow = false;
	static bool showCameraWindow = false;
	static bool showStatsWindow = false;
	static bool showEntityWindow = false;
	if (showDemoWindow)
		ImGui::ShowDemoWindow(&showDemoWindow);
	//if (showTerrainWindow)
	//	TerrainWindow(&showTerrainWindow, game);
	//if (showCameraWindow)
	//	CameraWindow(&showCameraWindow, game);
	if (showStatsWindow)
		StatsWindow(&showStatsWindow, game2D);
	//if (showEntityWindow)
	//	EntityWindow(&showEntityWindow, game);

	ImGui::Begin("DEBUG MENU");

	/* - Color buttons, demonstrate using PushID() to add unique identifier in the ID stack, and changing style.
	for (int i = 0; i < 7; i++)
	{
		if (i > 0)
			ImGui::SameLine();
		ImGui::PushID(i);
		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(i / 7.0f, 0.6f, 0.6f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(i / 7.0f, 0.7f, 0.7f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(i / 7.0f, 0.8f, 0.8f));
		ImGui::Button("Click");
		ImGui::PopStyleColor(3);
		ImGui::PopID();
	}*/
	ImGui::Separator();
	if (ImGui::Button("debug")) { game2D->debugMode(); }
	ImGui::SameLine();
	if (ImGui::Button("play") && game2D->state != Mon::State::Play) { game2D->playMode(); }
	ImGui::SameLine();
	if (ImGui::Button("save"))
	{
#ifdef _3D_GUI_
		writeEntities(game->world->entities, game->mainShaderID);
		Mon::Log::print("Saved game to master file");
		Mon::Log::warn("Only one master save file active!!!");
#endif
	}
	ImGui::SameLine();
	if (ImGui::Button("load"))
	{
		//loadEntities(game);
		Mon::Log::print("Loaded last saved scene");
	}
	ImGui::SameLine();

	if (ImGui::Button("Fullscreen"))
	{
		SDL_DisplayMode dm;
		SDL_GetCurrentDisplayMode(0, &dm);
		SDL_SetWindowSize(window, dm.w, dm.h);
		SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
		game2D->setViewPort(dm.w, dm.h);
	}
	ImGui::SameLine();
	if (ImGui::Button("Exit"))
	{
		SDL_SetWindowSize(window, settings->windowWidth, settings->windowHeight);
		SDL_SetWindowFullscreen(window, 0);
		game2D->setViewPort(960.0f, 540.0f);
	}

	if (ImGui::Button("720"))
	{
		settings->windowWidth = 1280;
		settings->windowHeight = 720;
		SDL_SetWindowSize(window, settings->windowWidth, settings->windowHeight);
	}
	ImGui::SameLine();
	if (ImGui::Button("1440"))
	{
		settings->windowWidth = 1440;
		settings->windowHeight = 900;
		SDL_SetWindowSize(window, settings->windowWidth, settings->windowHeight);
	}

	ImGui::SliderFloat2("port", (float*)&game2D->config->viewPort, 0.0f, 477.0f);
	ImGui::Separator();

	ImGui::LabelText(std::to_string(game2D->deltaTime).c_str(), "dt:");

	if (ImGui::RadioButton("off", settings->vsync == 0))
	{
		SDL_GL_SetSwapInterval(0);
		settings->vsync = SDL_GL_GetSwapInterval();
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("vertical retrace", settings->vsync == 1))
	{
		SDL_GL_SetSwapInterval(1);
		settings->vsync = SDL_GL_GetSwapInterval();

	}
	ImGui::SameLine();
	if (ImGui::RadioButton("adaptive", settings->vsync == -1))
	{
		SDL_GL_SetSwapInterval(-1);
		settings->vsync = SDL_GL_GetSwapInterval();
	}


	ImGui::Checkbox("Demo", &showDemoWindow);
	ImGui::SameLine();
	ImGui::Checkbox("stats", &showStatsWindow);

	// 2D
	// -------------
	// TODO(CK): put player at the 0th index for now


	ImGui::SliderFloat("Player Speed", &game2D->world->player->speed, 0.0f, 1000.0f);
	ImGui::SliderFloat("camera lerp", &game2D->camera.lerpSpeed, 0.0f, 100.0f);
	ImGui::SliderFloat("camera smooth", &game2D->camera.smoothness, 0.1f, 10.0f);

	char buffer[64];
	snprintf(buffer, sizeof(buffer), "%f", game2D->world->player->pos.x);
	ImGui::LabelText("player x", buffer);
	snprintf(buffer, sizeof(buffer), "%f", game2D->world->player->pos.y);
	ImGui::LabelText("player y", buffer);

	ImGui::DragFloat("cam zoom", &game2D->camera.zoom, 0.1f, 1.0f, 200.0f, "%.02f");
	//ImGui::SliderInt("Tile 0 ID: ", &game->world2D->map->tiles[0].tileId, 0, 3, NULL);

	char entitysizebuf[64];
	snprintf(entitysizebuf, sizeof(entitysizebuf), "%f", (float)game2D->world->entities.size());

	ImGui::End();
}


#endif // _3D_GUI_
#endif // USE_SDL





/// 
/// Generic gui functions 
/// 

void RenderGui()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

/* 
	NOTE(ck): SDL Relative Mouse Mode causes the mouse to get
	stuck in the gui. There is a fix in this thread. Right now
	I have a simple bandaid fix as it only happens when the gui
	is near the center of the window and SDLSetRelativeMouseMode 
	is set to true.
	https://github.com/ocornut/imgui/issues/3650
*/ 
bool GuiActive(bool SDLRelativeMouseMode)
{
	if (SDLRelativeMouseMode)
		return false;

	return ImGui::GetIO().WantCaptureMouse;
}

void ShutdownGui()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}
