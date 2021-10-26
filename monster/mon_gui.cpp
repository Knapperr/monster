#include "mon_gui.h"

#include <string>

//#define _3D_GUI_

// TODO(ck): Must switch Entity to plain array so i can save their memory to a file cant do it with vector
// TODO(ck): open files with current platform layer
// COPY MEMORY TO FILE and read it in
#include <fstream>
void writeEntities(std::vector<Mon::Entity>& entities, int shaderID)
{
	std::ofstream file;
	file.open("scene_1.txt");

	for (int i = 0; i < entities.size(); ++i)
	{
		file << entities[i].name << "\n"
			<< entities[i].particle.pos.x << "\n" << entities[i].particle.pos.y << "\n" << entities[i].particle.pos.z << "\n"
			<< shaderID << "\n"
			<< entities[i].data.texturePath << "\n";

	}

	file.close();
}

void loadEntities(Mon::Game* game)
{
	// TODO(ck): memory management should probably have something in the list to only reload part of it
	// READ MEMORY FROM FILE
	game->entities.clear();

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

		MonGL::initQuad(&e.data);
		MonGL::loadTexture(&e.data, MonGL::Type::Diffuse, shaderID, textPath);
		game->entities.push_back(e);

	}
	
}

#ifdef USE_SDL
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

	ImGui::PushItemWidth(100.0f);
		ImGui::DragFloat("Mat diffuse r", &game->terrain->mesh.mat.diffuse.x, 0.01f, 0.0f, 1.0f, "%.02f");
		ImGui::DragFloat("Mat diffuse g", &game->terrain->mesh.mat.diffuse.y, 0.01f, 0.0f, 1.0f, "%.02f");
		ImGui::DragFloat("Mat diffuse b", &game->terrain->mesh.mat.diffuse.z, 0.01f, 0.0f, 1.0f, "%.02f");
		ImGui::Separator();
		ImGui::DragFloat("Mat ambient r", &game->terrain->mesh.mat.ambient.x, 0.01f, 0.0f, 1.0f, "%.02f");
		ImGui::DragFloat("Mat ambient g", &game->terrain->mesh.mat.ambient.y, 0.01f, 0.0f, 1.0f, "%.02f");
		ImGui::DragFloat("Mat ambient b", &game->terrain->mesh.mat.ambient.z, 0.01f, 0.0f, 1.0f, "%.02f");
	ImGui::PopItemWidth();

	ImGui::End();
}

void CameraWindow(bool* p_open, Mon::Game* game)
{
	ImGui::Begin("Cammy", p_open);

	float velocity = game->cam->speed * game->deltaTime;
	
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

	ImGui::Separator();

	ImGui::DragFloat("cam zoom", &game->cam->zoom, 0.1f, -1000.0f, 1000.0f, "%.02f");

	ImGui::DragFloat("near plane", &game->cam->nearPlane, 0.01f, 0.1f, 100.0f, "%.02f");
	ImGui::DragFloat("far plane", &game->cam->farPlane, 0.5f, 100.0f, 1000.0f, "%.02f");
	
	ImGui::SliderFloat("camera angle", &game->cam->angleAroundTarget, -360.0f, 180.0f);
	ImGui::SliderFloat("camera pitch", &game->cam->pitch, -1.10f, 100.0f, "%1.0f");
	ImGui::SliderFloat("camera lerp", &game->cam->lerpSpeed, 0.0f, 100.0f);

	ImGui::End();
}

void EntityWindow(bool* p_open, Mon::Game* game)
{
	ImGui::Begin("entities and things", p_open);
	ImGui::DragFloat("sprite angle", &game->config->angleDegrees, 0.10f, -180.0f, 360.0f, "%.10f");

	static unsigned int selected = 0;
	ImGui::BeginChild("left pane", ImVec2(150.0f, 0.0f), true);
	
	for (unsigned int i = 0; i < game->entities.size(); ++i)
	{
		char label[128];
		sprintf_s(label, "%s %d", game->entities[i].name.c_str(), i);
		if (ImGui::Selectable(label, selected == i))
		{
			selected = i;
			game->selectedIndex = i;
		}
		// TODO(ck): Model loading
		//ImGui::Separator();
		//if (ImGui::Button("New Model"))
		//{
		//	CreateEmptyObject();
		//}
	}
	
	ImGui::EndChild();
	ImGui::SameLine();

	if (game->entities.empty() == false)
	{
		ImGui::BeginGroup();
		ImGui::BeginChild("item view", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us
		
		ImGui::Text("%s", game->entities[selected].name.c_str());

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

				ImGui::DragFloat("x", &game->entities[selected].particle.pos.x, 0.1f, -1000.0f, 1000.0f, "%.02f");
				ImGui::DragFloat("y", &game->entities[selected].particle.pos.y, 0.1f, -1000.0f, 1000.0f, "%.02f");
				ImGui::DragFloat("z", &game->entities[selected].particle.pos.z, 0.1f, -1000.0f, 1000.0f, "%.02f");

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
	ImGui::LabelText(buffer, "mouse x");
	snprintf(buffer, sizeof(buffer), "%f", game->input.mouseOffset.y);
	ImGui::LabelText(buffer, "mouse y");
	snprintf(buffer, sizeof(buffer), "%f", game->input.mouseXScreen);
	ImGui::LabelText(buffer, "win x");
	snprintf(buffer, sizeof(buffer), "%f", game->input.mouseYScreen);
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

	//snprintf(buffer, sizeof(buffer), "%d", GuiActive(false));
	//ImGui::LabelText(buffer, "gui active");

//
// 3D STATS
//
#ifdef _3D_GUI_
	snprintf(buffer, sizeof(buffer), "%f", game->player.particle.pos.x);
	ImGui::LabelText(buffer, "player x");
	snprintf(buffer, sizeof(buffer), "%f", game->player.particle.pos.y);
	ImGui::LabelText(buffer, "player y");
	snprintf(buffer, sizeof(buffer), "%f", game->player.particle.pos.z);
	ImGui::LabelText(buffer, "player z");

	snprintf(buffer, sizeof(buffer), "%f", game->player.particle.velocity.x);
	ImGui::LabelText(buffer, "player velocity");
	snprintf(buffer, sizeof(buffer), "%f", game->player.particle.acceleration.x);
	ImGui::LabelText(buffer, "player acc");

	snprintf(buffer, sizeof(buffer), "%f", game->cam->pos.x);
	ImGui::LabelText(buffer, "cam x");
	snprintf(buffer, sizeof(buffer), "%f", game->cam->pos.y);
	ImGui::LabelText(buffer, "cam y");
	snprintf(buffer, sizeof(buffer), "%f", game->cam->pos.z);
	ImGui::LabelText(buffer, "cam z");

	snprintf(buffer, sizeof(buffer), "%f", game->cam->right.x);
	ImGui::LabelText(buffer, "right x");
	snprintf(buffer, sizeof(buffer), "%f", game->cam->right.y);
	ImGui::LabelText(buffer, "right y");
	snprintf(buffer, sizeof(buffer), "%f", game->cam->right.z);
	ImGui::LabelText(buffer, "right z");


	snprintf(buffer, sizeof(buffer), "%f", game->cam->front.x);
	ImGui::LabelText(buffer, "front x");
	snprintf(buffer, sizeof(buffer), "%f", game->cam->front.y);
	ImGui::LabelText(buffer, "front y");
	snprintf(buffer, sizeof(buffer), "%f", game->cam->front.z);
	ImGui::LabelText(buffer, "front z");

	snprintf(buffer, sizeof(buffer), "%f", game->cam->pitch);
	ImGui::LabelText(buffer, "pitch");
	snprintf(buffer, sizeof(buffer), "%f", game->cam->yaw);
	ImGui::LabelText(buffer, "yaw");
#endif
	ImGui::End();
}

void UpdateGui(SDL_Window* window, Settings* settings, Mon::Game* game)
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
	if (showTerrainWindow)
		TerrainWindow(&showTerrainWindow, game);
	if (showCameraWindow)
		CameraWindow(&showCameraWindow, game);
	if (showStatsWindow)
		StatsWindow(&showStatsWindow, game);
	if (showEntityWindow)
		EntityWindow(&showEntityWindow, game);

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
		if (ImGui::Button("debug")) { game->debugMode(); }
		ImGui::SameLine();
		if (ImGui::Button("play")) { game->playMode(); }
		ImGui::SameLine();
		if (ImGui::Button("save")) { writeEntities(game->entities, game->mainShaderID); }
		ImGui::SameLine();
		if (ImGui::Button("load")) { loadEntities(game); }
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
			SDL_SetWindowSize(window, settings->width, settings->height);
			SDL_SetWindowFullscreen(window, 0);
			game->setViewPort(960.0f, 540.0f);
		}

		if (ImGui::Button("720"))
		{
			settings->width = 1280;
			settings->height = 720;
			SDL_SetWindowSize(window, settings->width, settings->height);
		}
		ImGui::SameLine();
		if (ImGui::Button("1440"))
		{
			settings->width = 1440;
			settings->height = 900;
			SDL_SetWindowSize(window, settings->width, settings->height);
		}
	
		ImGui::SliderFloat2("port", (float*)&game->config->viewPort, 0.0f, 477.0f);
	ImGui::Separator();

		ImGui::LabelText(std::to_string(game->deltaTime).c_str(), "dt:");

		ImGui::Checkbox("Demo", &showDemoWindow);
		ImGui::SameLine();
		ImGui::Checkbox("stats", &showStatsWindow);

#ifdef _3D_GUI_

		ImGui::Checkbox("Terrain", &showTerrainWindow);
		ImGui::SameLine();
		ImGui::Checkbox("Camera", &showCameraWindow);
		ImGui::SameLine();
		ImGui::Checkbox("Things", &showEntityWindow);

		ImGui::DragFloat("cam speed", &game->cam->speed, 0.01f, 1.0f, 200.0f, "%.02f");

		/// 
		/// Player 
		///
		ImGui::LabelText("", "Player");
		if (ImGui::SmallButton("reset Pos"))
		{
			game->player.particle.pos.y = 0.1f;
			game->player.particle.pos.x = 10.0f;
			game->player.particle.pos.z = 10.0f;
		}
		ImGui::SliderFloat("speed", &game->player.particle.speed, 0.0f, 100.0f);
		ImGui::SliderFloat3("lightpos", &game->light.pos[0], 0.0f, 500.0f);
		ImGui::SliderFloat3("color", &game->player.collider.data.color[0], 0.0f, 1.0f);
		ImGui::SliderFloat3("min", &game->player.collider.size.min[0], 0.0f, 50.0f);
		ImGui::SliderFloat3("max", &game->player.collider.size.max[0], 0.0f, 50.0f);
	

		ImGui::Checkbox("simulate", &game->simulate);
		ImGui::Checkbox("draw collisions", &game->drawCollisions);

	ImGui::Separator();

#endif
	

	// 2D // 
	// -------------
	// TODO(CK): put player at the 0th index for now

#ifndef _3D_GUI_

	ImGui::SliderFloat("Player Speed", &game->world->player->speed, 0.0f, 1000.0f);
	ImGui::SliderFloat("camera lerp", &game->camera.lerpSpeed, 0.0f, 100.0f);

	char buffer[64];
	snprintf(buffer, sizeof(buffer), "%f", game->world->player->pos.x);
	ImGui::LabelText("player x", buffer);
	snprintf(buffer, sizeof(buffer), "%f", game->world->player->pos.y);
	ImGui::LabelText("player y", buffer);
	
	ImGui::SliderInt("Tile 0 ID: ", &game->world->map->tiles[0].tileId, 0, 3, NULL);
	
	char entitysizebuf[64];
	snprintf(entitysizebuf, sizeof(entitysizebuf), "%f", (float)game->world->entities.size());


#endif

	ImGui::End();
}

#endif // USE_SDL

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
