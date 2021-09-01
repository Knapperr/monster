#include "mon_gui.h"

// TODO(ck): remove
#include <string>


//#define _3D_GUI_

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

void EntityWindow(bool* p_open, Mon::Game* game)
{
	ImGui::Begin("objects and things", p_open);

	ImGui::DragFloat("player sprite angle", &game->config->angleDegrees, 0.10f, -180.0f, 360.0f, "%.10f");

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

	snprintf(buffer, sizeof(buffer), "%f", game->input.rightStickAngle);
	ImGui::LabelText(buffer, "R stick angle");
	snprintf(buffer, sizeof(buffer), "%f", (float)game->input.rightStickAxis);
	ImGui::LabelText(buffer, "R stick axis");
	snprintf(buffer, sizeof(buffer), "%f", game->input.rightStickValue);
	ImGui::LabelText(buffer, "stick value");

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

	snprintf(buffer, sizeof(buffer), "%f", game->cam.pos.x);
	ImGui::LabelText(buffer, "cam x");
	snprintf(buffer, sizeof(buffer), "%f", game->cam.pos.y);
	ImGui::LabelText(buffer, "cam y");
	snprintf(buffer, sizeof(buffer), "%f", game->cam.pos.z);
	ImGui::LabelText(buffer, "cam z");

	snprintf(buffer, sizeof(buffer), "%f", game->cam.right.x);
	ImGui::LabelText(buffer, "right x");
	snprintf(buffer, sizeof(buffer), "%f", game->cam.right.y);
	ImGui::LabelText(buffer, "right y");
	snprintf(buffer, sizeof(buffer), "%f", game->cam.right.z);
	ImGui::LabelText(buffer, "right z");


	snprintf(buffer, sizeof(buffer), "%f", game->cam.front.x);
	ImGui::LabelText(buffer, "front x");
	snprintf(buffer, sizeof(buffer), "%f", game->cam.front.y);
	ImGui::LabelText(buffer, "front y");
	snprintf(buffer, sizeof(buffer), "%f", game->cam.front.z);
	ImGui::LabelText(buffer, "front z");

	snprintf(buffer, sizeof(buffer), "%f", game->cam.pitch);
	ImGui::LabelText(buffer, "pitch");
	snprintf(buffer, sizeof(buffer), "%f", game->cam.yaw);
	ImGui::LabelText(buffer, "yaw");
#endif
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

	ImGui::Separator();
	ImGui::RadioButton("debug", &game->state, Mon::State::Debug);
	ImGui::SameLine();
	ImGui::RadioButton("play", &game->state, Mon::State::Play);
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

	ImGui::Separator();
	if (ImGui::Button("follow on", { 96.0f, 16.0f })) { game->cam.followOn(); }
	ImGui::SameLine();
	if (ImGui::Button("follow off", { 96.0f, 16.0f })) { game->cam.followOff(); }
	ImGui::Separator();

	ImGui::SliderFloat("camera angle", &game->cam.angleAroundTarget, -360.0f, 180.0f);
	ImGui::SliderFloat("camera pitch", &game->cam.pitch, -1.10f, 100.0f, "%1.0f");



	// just make button you press that restarts the camera
	// pass it a boolean from gui?? no we don't want to keep
	// track of state
	//ImGui::Button("debug camera on", )

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
	ImGui::SliderFloat3("color", &game->player.colliderData.color[0], 0.0f, 1.0f);
	ImGui::SliderFloat3("min", &game->player.colliderData.size.min[0], 0.0f, 50.0f);
	ImGui::SliderFloat3("max", &game->player.colliderData.size.max[0], 0.0f, 50.0f);
	

	ImGui::Checkbox("simulate", &game->simulate);
	ImGui::Checkbox("draw collisions", &game->drawCollisions);

	ImGui::Separator();

#endif
	

	// 2D // 
	// -------------
	// TODO(CK): put player at the 0th index for now

#ifndef _3D_GUI_

	ImGui::SliderFloat("Player Speed", &game->world->player->speed, 0.0f, 500.0f);

	char buffer[64];
	snprintf(buffer, sizeof(buffer), "%f", game->world->player->pos.x);
	ImGui::LabelText("player x", buffer);
	snprintf(buffer, sizeof(buffer), "%f", game->world->player->pos.y);
	ImGui::LabelText("player y", buffer);
	
	ImGui::SliderInt("Tile 0 ID: ", &game->world->map->tiles[0].tileId, 0, 3, NULL);
	
	char entitysizebuf[64];
	snprintf(entitysizebuf, sizeof(entitysizebuf), "%d", game->world->entities.size());

#endif
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
