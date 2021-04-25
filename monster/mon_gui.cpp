#include "mon_gui.h"

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

/*
void TerrainWindow(bool* p_open, Game* game)
{
	ImGui::Begin("Terrain", p_open);

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
*/

void UpdateGui(SDL_Window* window, Game* game)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(window);
	ImGui::NewFrame();

	static bool showDemoWindow = false;
	if (showDemoWindow)
		ImGui::ShowDemoWindow(&showDemoWindow);

	ImGui::Begin("DEBUG MENU");

	ImGui::Checkbox("Demo", &showDemoWindow);
	
	// TODO(CK): put player at the 0th index for now
	ImGui::SliderFloat("Player Speed", &game->world->player->speed, 0.0f, 500.0f);

	char buffer[64];
	snprintf(buffer, sizeof(buffer), "%f", game->world->player->pos.x);
	ImGui::LabelText("player x", buffer);
	snprintf(buffer, sizeof(buffer), "%f", game->world->player->pos.y);
	ImGui::LabelText("player y", buffer);
	
	
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
