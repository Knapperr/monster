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

void UpdateGui(SDL_Window* window, Game* game)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(window);
	ImGui::NewFrame();

	static bool showDemoWindow = false;
	if (showDemoWindow)
		ImGui::ShowDemoWindow(&showDemoWindow);

	ImGui::Begin("DEBUG MENU");

	ImGui::DragFloat("cam zoom", &game->cam.zoom, 0.01f, -1000.0f, 1000.0f, "%.02f");
	ImGui::DragFloat("cam speed", &game->cam.speed, 0.01f, 1.0f, 200.0f, "%.02f");

	ImGui::DragFloat("pos x ", &game->pos.x, 0.01f, -1000.0, 1000.0,   "%.02f");
	ImGui::DragFloat("pos y ", &game->pos.y, 0.01f, -1000.0f, 1000.0f, "%.02f");
	ImGui::DragFloat("pos z ", &game->pos.z, 0.01f, -1000.0f, 1000.0f, "%.02f");

	// TODO(ck): Button pops this little window
	ImGui::DragFloat("L X", &game->light.pos.x, 0.01f, -1500.0f, 1500.0f,  "%.02f");
	ImGui::DragFloat("L Y", &game->light.pos.y, 0.01f, -1500.0f, 1500.0f, "%.02f");
	ImGui::DragFloat("L Z", &game->light.pos.z, 0.01f, -1500.0f, 1500.0f, "%.02f");
	ImGui::DragFloat("L diffuse r", &game->light.diffuse.x, 0.01f, 0.0f, 1.0f, "%.02f");
	ImGui::DragFloat("L diffuse g", &game->light.diffuse.y, 0.01f, 0.0f, 1.0f, "%.02f");
	ImGui::DragFloat("L diffuse b", &game->light.diffuse.z, 0.01f, 0.0f, 1.0f, "%.02f");
	// Materials ----
	


	// TODO(CK): put player at the 0th index for now
	//ImGui::SliderFloat("Player Speed", &g_Game->world->player->speed, 0.0f, 500.0f);

	//char buffer[64];
	//snprintf(buffer, sizeof(buffer), "%f", g_GameState->world->player->pos.x);
	
	//char ybuffer[64];
	//snprintf(ybuffer, sizeof(ybuffer), "%f", g_GameState->world->player->pos.y);
	
	//char entitysizebuf[64];
	//snprintf(entitysizebuf, sizeof(entitysizebuf), "%d", g_GameState->world->entities.size());

	//char dtbuf[64];
	//snprintf(dtbuf, sizeof(dtbuf), "%f", g_GameState->deltaTime);

	//ImGui::LabelText("player x", buffer);
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

bool GuiActive()
{
	return ImGui::GetIO().WantCaptureMouse;
}

void ShutdownGui()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}
