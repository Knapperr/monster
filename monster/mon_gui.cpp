#include "mon_gui.h"

// TODO(ck): remove
#include <string>

#ifdef USE_SDL
void InitGui(SDL_Window* window, SDL_GLContext* context)
{
	const char* glsl_version = "#version 130";

	// set up
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::SetMouseCursor(ImGuiMouseCursor_None);
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
	if (ImGui::Button("UV")) { game->terrain->selectedTextureId = game->terrain->textureIds[0]; }
	ImGui::SameLine();
	if (ImGui::Button("Rock")) { game->terrain->selectedTextureId = game->terrain->textureIds[1]; }
	ImGui::SameLine();
	if (ImGui::Button("Grass")) { game->terrain->selectedTextureId = game->terrain->textureIds[2]; }
	ImGui::SameLine();
	if (ImGui::Button("Snow")) { game->terrain->selectedTextureId = game->terrain->textureIds[3]; }
	ImGui::Separator();

	ImGui::Checkbox("Wireframe", &game->terrain->wireFrame);

	ImGui::PushItemWidth(100.0f);
		ImGui::DragFloat("Mat diffuse r", &game->terrain->material.diffuse.x, 0.01f, 0.0f, 1.0f, "%.02f");
		ImGui::DragFloat("Mat diffuse g", &game->terrain->material.diffuse.y, 0.01f, 0.0f, 1.0f, "%.02f");
		ImGui::DragFloat("Mat diffuse b", &game->terrain->material.diffuse.z, 0.01f, 0.0f, 1.0f, "%.02f");
		ImGui::Separator();
		ImGui::DragFloat("Mat ambient r", &game->terrain->material.ambient.x, 0.01f, 0.0f, 1.0f, "%.02f");
		ImGui::DragFloat("Mat ambient g", &game->terrain->material.ambient.y, 0.01f, 0.0f, 1.0f, "%.02f");
		ImGui::DragFloat("Mat ambient b", &game->terrain->material.ambient.z, 0.01f, 0.0f, 1.0f, "%.02f");
	ImGui::PopItemWidth();

	ImGui::End();
}

void CameraWindow(bool* p_open, Mon::Game* game)
{
	ImGui::Begin("Cammy", p_open);

	float velocity = game->cam.speed * game->deltaTime;
	
	ImGui::PushButtonRepeat(true);
	ImGui::Indent(32);
	if (ImGui::ArrowButton("##up", ImGuiDir_Up))
		game->cam.pos += game->cam.front * velocity;	
	ImGui::Unindent(32);

	if (ImGui::ArrowButton("##left", ImGuiDir_Left))
		game->cam.pos -= game->cam.right * velocity;
	ImGui::SameLine(30, 40);
	if (ImGui::ArrowButton("##right", ImGuiDir_Right))
		game->cam.pos += game->cam.right * velocity;

	ImGui::Indent(32);
	if (ImGui::ArrowButton("##down", ImGuiDir_Down))
		game->cam.pos -= game->cam.front * velocity;
	ImGui::Unindent(32);
	ImGui::PopButtonRepeat();

	ImGui::Separator();

	ImGui::DragFloat("cam zoom", &game->cam.zoom, 0.1f, -1000.0f, 1000.0f, "%.02f");
	ImGui::DragFloat("cam speed", &game->cam.speed, 0.01f, 1.0f, 200.0f, "%.02f");
	
	ImGui::DragFloat("near plane", &game->cam.nearPlane, 0.01f, 0.1f, 100.0f, "%.02f");
	ImGui::DragFloat("far plane", &game->cam.farPlane, 0.5f, 100.0f, 1000.0f, "%.02f");


	ImGui::End();
}

void StatsWindow(bool* p_open, Mon::Game* game)
{
	ImGui::Begin("stats for me", p_open);

	char buffer[64];
	//printf("%b\n", game->input.leftMouseButton.endedDown);
	snprintf(buffer, sizeof(buffer), "%f", game->input.mouseXOffset);
	ImGui::LabelText(buffer, "mouse x");
	snprintf(buffer, sizeof(buffer), "%f", game->input.mouseYOffset);
	ImGui::LabelText(buffer, "mouse y");
	snprintf(buffer, sizeof(buffer), "%f", game->input.mouseXScreen);
	ImGui::LabelText(buffer, "win x");
	snprintf(buffer, sizeof(buffer), "%f", game->input.mouseYScreen);
	ImGui::LabelText(buffer, "win y");

	snprintf(buffer, sizeof(buffer), "%d", game->input.leftMouseButton.endedDown);
	ImGui::LabelText(buffer, "left mouse down");

	snprintf(buffer, sizeof(buffer), "%d", GuiActive(game->input.leftMouseButton.endedDown));
	ImGui::LabelText(buffer, "gui active");


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

	ImGui::End();
}

void UpdateGui(SDL_Window* window, Mon::Game* game)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(window);
	ImGui::NewFrame();

	static bool showDemoWindow = false;
	static bool showTerrainWindow = false;
	static bool showCameraWindow = false;
	static bool showStatsWindow = false;
	if (showDemoWindow)
		ImGui::ShowDemoWindow(&showDemoWindow);
	if (showTerrainWindow)
		TerrainWindow(&showTerrainWindow, game);
	if (showCameraWindow)
		CameraWindow(&showCameraWindow, game);
	if (showStatsWindow)
		StatsWindow(&showStatsWindow, game);

	ImGui::Begin("DEBUG MENU");

	ImGui::LabelText(std::to_string(game->deltaTime).c_str(), "dt:");

	ImGui::Checkbox("Demo", &showDemoWindow);
	ImGui::SameLine();
	ImGui::Checkbox("Terrain", &showTerrainWindow);
	ImGui::SameLine();
	ImGui::Checkbox("Camera", &showCameraWindow);
	ImGui::SameLine();
	ImGui::Checkbox("stats", &showStatsWindow);

	ImGui::Checkbox("camera disabled", &game->cam.disabled);

	/// 
	/// Player 
	///
	ImGui::LabelText("", "Player");
	if (ImGui::SmallButton("reset Pos"))
	{
		game->player.particle.pos.y = 1.0f;
		game->player.particle.pos.x = 10.0f;
		game->player.particle.pos.z = 10.0f;
	}

	ImGui::SliderFloat3("color", &game->player.colliderData.color[0], 0.0f, 1.0f);
	ImGui::SliderInt("Collider Width", &game->player.colliderData.lineWidth, 1, 100, NULL);
	ImGui::SliderFloat2("X", &game->player.colliderData.size.x[0], 1.0f, 50.0f);
	ImGui::SliderFloat2("Y", &game->player.colliderData.size.y[0], 1.0f, 50.0f);
	ImGui::SliderFloat2("Z", &game->player.colliderData.size.z[0], 1.0f, 50.0f);


	ImGui::Checkbox("simulate", &game->simulate);

	////// TODO(ck): Button pops this little window
	ImGui::Separator();
	ImGui::LabelText("", "Light");
	ImGui::DragFloat("L X", &game->light.pos.x, 0.01f, -1500.0f, 1500.0f,  "%.02f");
	ImGui::DragFloat("L Y", &game->light.pos.y, 0.01f, -1500.0f, 1500.0f, "%.02f");
	ImGui::DragFloat("L Z", &game->light.pos.z, 0.01f, -1500.0f, 1500.0f, "%.02f");

	ImGui::PushItemWidth(100.0f);
		ImGui::DragFloat("L diffuse r", &game->light.diffuse.x, 0.01f, 0.0f, 1.0f, "%.02f");
		ImGui::DragFloat("L diffuse g", &game->light.diffuse.y, 0.01f, 0.0f, 1.0f, "%.02f");
		ImGui::DragFloat("L diffuse b", &game->light.diffuse.z, 0.01f, 0.0f, 1.0f, "%.02f");
		ImGui::DragFloat("L ambient r", &game->light.ambient.x, 0.01f, 0.0f, 1.0f, "%.02f");
		ImGui::DragFloat("L ambient g", &game->light.ambient.y, 0.01f, 0.0f, 1.0f, "%.02f");
		ImGui::DragFloat("L ambient b", &game->light.ambient.z, 0.01f, 0.0f, 1.0f, "%.02f");
	
	//	// Material ----
		ImGui::Separator();
		ImGui::DragFloat("M diffuse r", &game->player.data.mat.diffuse.x, 0.01f, 0.0f, 1.0f, "%.02f");
		ImGui::DragFloat("M diffuse g", &game->player.data.mat.diffuse.y, 0.01f, 0.0f, 1.0f, "%.02f");
		ImGui::DragFloat("M diffuse b", &game->player.data.mat.diffuse.z, 0.01f, 0.0f, 1.0f, "%.02f");
		ImGui::DragFloat("M ambient r", &game->player.data.mat.ambient.x, 0.01f, 0.0f, 1.0f, "%.02f");
		ImGui::DragFloat("M ambient g", &game->player.data.mat.ambient.y, 0.01f, 0.0f, 1.0f, "%.02f");
		ImGui::DragFloat("M ambient b", &game->player.data.mat.ambient.z, 0.01f, 0.0f, 1.0f, "%.02f");
	ImGui::PopItemWidth();

	ImGui::Separator();

	// 2D // 
	// -------------
	// TODO(CK): put player at the 0th index for now
	//ImGui::SliderFloat("Player Speed", &game->world->player->speed, 0.0f, 500.0f);

	//char buffer[64];
	//snprintf(buffer, sizeof(buffer), "%f", game->world->player->pos.x);
	//ImGui::LabelText("player x", buffer);
	//snprintf(buffer, sizeof(buffer), "%f", game->world->player->pos.y);
	//ImGui::LabelText("player y", buffer);
	
	
	//char entitysizebuf[64];
	//snprintf(entitysizebuf, sizeof(entitysizebuf), "%d", g_GameState->world->entities.size());

	//char dtbuf[64];
	//snprintf(dtbuf, sizeof(dtbuf), "%f", g_GameState->deltaTime);

	
	//ImGui::LabelText("player y", ybuffer);
	//ImGui::LabelText("entities size", entitysizebuf);
	//ImGui::LabelText("delta time: ", dtbuf);

	ImGui::End();
}

#endif

void RenderGui()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

/* 
	NOTE(ck): WantCaptureMouse is returning true when the mouse 
			 is close but not over the gui for some reason. This 
			 could have something to do with SDL. The fix right 
			 now is to check if the mouse button is being pressed
			 and say that we aren't active if mouse button is already
			 being held down.
*/ 
bool GuiActive(bool leftPressed)
{
	if (leftPressed)
		return false;

	return ImGui::GetIO().WantCaptureMouse;
}

void ShutdownGui()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}
