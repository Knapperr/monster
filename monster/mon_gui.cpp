#include "mon_gui.h"

#include <string>

float inputTimer = 0.0f;

// TODO(ck): open files with current platform layer
#include <fstream>
#ifdef _3D_GUI_

void CreateBinaryFile()
{

}

const char * RenderTypeText(Mon::RenderType type)
{
	switch (type)
	{
		case Mon::RenderType::Model:
			return "model";
		case Mon::RenderType::Cube:
			return "cube";
		case Mon::RenderType::Quad:
			return "quad";
		case Mon::RenderType::Debug:
			return "debug";
		case Mon::RenderType::None:
			return "none";
		default:
			return "none";
	}
	return "none";
}

// TODO(ck): Write WORLD not entities
void WriteEntities(Mon::Entity* entities, unsigned int entityCount, int shaderID)
{
	std::ofstream file;
	file.open("scene_1.txt");

	file << entityCount << "\n";

	for (unsigned int i = 1; i < entityCount; ++i)
	{
		file << entities[i].name << "\n"
			<< (int)entities[i].data.meshIndex << "\n"
			<< (int)entities[i].data.textureIndex << "\n"
			<< entities[i].rb.pos.x << "\n" << entities[i].rb.pos.y << "\n" << entities[i].rb.pos.z << "\n"
			<< entities[i].data.scale.x << "\n" << entities[i].data.scale.y << "\n" << entities[i].data.scale.z << "\n"
			<< shaderID << "\n";
	}

	// TODO(ck):
	// Cube and Quad still use imp file because we still need their texture paths???
	// if (entities[i].data.type == MonGL::RenderType.Model)
	//	file << entities.impFilePath;
	

	file.close();
}

const char* GetFolders()
{
	const char* result = "";
	return result;
}

void getFolders(std::string folder)
{
	/*objFile obj = {};
	obj.loadModel = false;
	obj.loaded = false;
	std::string path = "content\\" + folder;
	for (auto& d : fs::recursive_directory_iterator(path))
	{
		if (d.path().extension() == ".obj")
		{
			obj.path = d.path().generic_string();
			obj.name = d.path().filename().generic_string();

			if (folder == "objects")
			{
				objPaths.push_back(obj);
			}
		}
	}*/
}

void LoadSceneFile(Mon::GameState* game)
{
	// TODO(ck): memory management should probably have something in the list to only reload part of it
	// READ MEMORY FROM FILE
	std::string line;
	std::ifstream file("scene_1.txt");
	if (!file.is_open())
	{
		Mon::Log::warn("Failure to open file!");
		return;
	}
	
	unsigned int count = 0;
	file >> count;
	
	// Add extra entities to increment the entityCount
	if (count > game->world->entityCount)
	{
		for (unsigned int i = game->world->entityCount; i < count; i++)
		{
			Mon::AddEntity(game->world);
		}
	}

	// IMPORTANT(ck): I don't think I need to clear anything or do anytihng I just need to 
	// alter what I already have... the data is already initialized its fine?

	//// Clear and reset the entity container
	//for (int i = 1; i < count; ++i)
	//{
	//	Mon::ClearEntity(game->world, i);
	//}
	//game->world->entityCount = 0;
	//AddEntity(game->world); // Add our 0 entity


	// TODO(ck): Not shader index it just needs shader type
	//Mon::Entity* e = Mon::GetPlayer(game->world);
	//Mon::InitPlayer(e, game->renderer.program.handle);

	bool finished = false;
	std::string tempName;
	int index = 1;
	while (file >> line)
	{
		// starts at two
		while (index < count)
		{
			Mon::Entity* e = Mon::GetEntity(game->world, index);
			// TODO(ck): Need constructor for this
			if (index > 1)
			{
				//e->name = tempName.c_str();
				file >> tempName;
				//e->name tempName.c_str();
				//Mon::InitEntity(e, tempName.c_str(), Mon::v3(1.0f), Mon::v3(1.0f), -45.0f, game->renderer.program.handle, 1, 1);
			}
			//else
			//{
				//tempName = line;
				//e->name = line.c_str();
				//Mon::InitEntity(e, line.c_str(), Mon::v3(1.0f), Mon::v3(1.0f), -45.0f, game->renderer.program.handle, 1, 1);
			//}

			file >> e->data.meshIndex;
			file >> e->data.textureIndex;
			file >> e->rb.pos.x;
			file >> e->rb.pos.y;
			file >> e->rb.pos.z;

			file >> e->data.scale.x;
			file >> e->data.scale.y;
			file >> e->data.scale.z;

			int notUsedShaderId;
			file >> notUsedShaderId;

			Mon::Log::print("Entity loaded");
			++index;
		}
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
	const char* glsl_version = "#version 330";

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


void TerrainWindow(bool* p_open, Mon::GameState* game)
{
	ImGui::Begin("Terrain", p_open);

	ImGui::Text("Texture");
	ImGui::Separator();
	if (ImGui::Button("UV")) { game->grid->data.textureIndex = 11; }
	ImGui::SameLine();
	if (ImGui::Button("Grass")) { game->grid->data.textureIndex = 12; }
	ImGui::SameLine();
	if (ImGui::Button("Pixel Grass")) { game->grid->data.textureIndex = 13; }
	ImGui::SameLine();
	if (ImGui::Button("Snow")) { game->grid->data.textureIndex = 14; }
	ImGui::Separator();
	if (ImGui::Button("1x1")) { game->grid->data.textureIndex = 16; }
	ImGui::Separator();

	ImGui::Checkbox("Wireframe", &game->grid->data.wireFrame);

	ImGui::End();
}



void CameraWindow(bool* p_open, Mon::GameState* game)
{
	ImGui::Begin("Cammy", p_open);

	ImGui::LabelText(game->cameras[game->currCameraIndex].name, "Current Camera");
	if (ImGui::ArrowButton("##left", ImGuiDir_Left))
	{
		if (game->currCameraIndex > 1)
			game->currCameraIndex--;

		if (game->cameras[game->currCameraIndex].type == Mon::CameraType::Follow)
			Mon::PlayMode(game);
		else
			Mon::DebugMode(game);
	}
	ImGui::SameLine();
	if (ImGui::ArrowButton("##right", ImGuiDir_Right))
	{
		if (game->currCameraIndex < (game->cameraCount - 1))
			game->currCameraIndex++;

		if (game->cameras[game->currCameraIndex].type == Mon::CameraType::Follow)
			Mon::PlayMode(game);
		else
			Mon::DebugMode(game);
	}

	ImGui::Separator();

	ImGui::DragFloat("cam zoom", &game->cameras[game->currCameraIndex].zoom, 0.1f, -1000.0f, 1000.0f, "%.02f");

	ImGui::DragFloat("near plane", &game->cameras[game->currCameraIndex].nearPlane, 0.01f, 0.1f, 100.0f, "%.02f");
	ImGui::DragFloat("far plane", &game->cameras[game->currCameraIndex].farPlane, 0.5f, 100.0f, 1000.0f, "%.02f");
	
	ImGui::SliderFloat("pitch", &game->cameras[game->currCameraIndex].pitch, -1.10f, 100.0f, "%1.0f");
	ImGui::SliderFloat("angle", &game->cameras[game->currCameraIndex].angleAroundTarget, -360.0f, 180.0f);
	ImGui::SliderFloat("lerp", &game->cameras[game->currCameraIndex].lerpSpeed, 0.0f, 100.0f);
	ImGui::SliderFloat("smooth", &game->cameras[game->currCameraIndex].smoothness, 0.1f, 10.0f);

	if (ImGui::Button("Log"))
	{ 
		Mon::Log::print("cam zoom", game->cameras[game->currCameraIndex].zoom);
		Mon::Log::print("near plane", game->cameras[game->currCameraIndex].nearPlane);
		Mon::Log::print("far plane", game->cameras[game->currCameraIndex].farPlane);
		Mon::Log::print("pitch", game->cameras[game->currCameraIndex].pitch);
		Mon::Log::print("angle around target", game->cameras[game->currCameraIndex].angleAroundTarget);
		Mon::Log::print("lerp speed", game->cameras[game->currCameraIndex].lerpSpeed);
		Mon::Log::print("smoothness", game->cameras[game->currCameraIndex].smoothness);
	}

	ImGui::End();
}

void AddNewEntity(Mon::GameState* game, int meshIndex, int texIndex = 18, Mon::v3 scale = Mon::v3(1.0f))
{
	Mon::AddEntity(game->world);
	Mon::Entity* e = Mon::GetEntity(game->world, game->world->entityCount - 1);
	Mon::InitEntity(e, "new", Mon::v3(1.0f, 0.0f, 1.0f), scale, -45.0f, game->renderer.program.handle, texIndex, meshIndex);
}

void AddWater(Mon::GameState* game)
{
	Mon::AddEntity(game->world);
	Mon::Entity* water = Mon::GetEntity(game->world, game->world->entityCount - 1);
	Mon::InitWater(water, game->renderer.waterProgram.common.handle);
}

void EntityWindow(bool* p_open, Mon::GameState* game)
{
	ImGui::Begin("entities and things", p_open);
	
	if (ImGui::Button("Add Entity")) { AddNewEntity(game, 1); }
	
	ImGui::Separator();
	ImGui::Text("ADD CUBE");
	if (ImGui::Button("1m")) { AddNewEntity(game, 2); }
	ImGui::SameLine();
	if (ImGui::Button("2m")) { AddNewEntity(game, 2, 20, Mon::v3(2.0f)); }
	ImGui::SameLine();
	if (ImGui::Button("4m")) { AddNewEntity(game, 2, 21, Mon::v3(4.0f)); }
	ImGui::SameLine();
	if (ImGui::Button("10m")) { AddNewEntity(game, 2, 22, Mon::v3(10.0f)); }
	ImGui::Separator();


	if (ImGui::Button("Add Water")) { AddWater(game); }
	// NOTE(ck): Write the .imp model to a file 
	/*if (ImGui::Button("Write imp file"))
	{
		writeImpFile(game);
	}*/

	static unsigned int selected = 1;
	ImGui::BeginChild("left pane", ImVec2(150.0f, 0.0f), true);


	// increment or decrement the selectedIndex with r and f for quick editing 
	// this might not be a standard but it will make it quick for me and thats
	// all that matters =)
	// Need to set a timer on this so it cant fire
/*
	TODO(ck): Make the input have a built in delay for menus specifically
	maybe have separate up and downs that

	because we need to use this same delay in the other menus as well
	we will leave it here for now because we dont need quick select on
	asset menus. the entity menu is used for building a scene or chunk
*/
#if 1
	if (inputTimer > 0.0f)
		inputTimer -= (float)game->deltaTime;

	if (game->input.r.endedDown && inputTimer <= 0.0f)
	{
		inputTimer = (float)game->deltaTime * 10.0f;
		if (game->selectedIndex > 1)
		{
			game->selectedIndex--;
			selected--;
		}
	}
	if (game->input.f.endedDown && inputTimer <= 0.0f)
	{
		inputTimer = (float)game->deltaTime * 15.0f;
		if (game->selectedIndex < (game->world->entityCount - 1))
		{
			game->selectedIndex++;
			selected++;
		}
	}
#endif

	for (unsigned int i = 1; i < game->world->entityCount; ++i)
	{
		char label[128];
		sprintf_s(label, "%s %d", game->world->entities[i].name, i);
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
		
		ImGui::Text("%s", game->world->entities[selected].name);

		// TODO(ck): Model loading modal
		// Modal for loading meshes
		if (ImGui::Button("Load Mesh.."))
			ImGui::OpenPopup("Load Mesh");
		if (ImGui::BeginPopupModal("Load Mesh", NULL))
		{
			std::vector<std::string> objPaths;
			objPaths.push_back("apple_sauce");
			for (unsigned int i = 0; i < objPaths.size(); i++)
			{
				//if (ImGui::SmallButton(objPaths[i].name.c_str()))
				if (ImGui::SmallButton("name"))
				{
					//LOG("Loading mesh...");
					//LoadEmptyObject(selected, objPaths[i].name, objPaths[i].path);
					//LOG("mesh loaded!");
					ImGui::CloseCurrentPopup();
				}
			}

			if (ImGui::Button("Close"))
				ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
		}
		ImGui::Separator();
		if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None))
		{
			if (ImGui::BeginTabItem("Controls"))
			{
				//ImGui::TextWrapped("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. ");
				//ImGui::SliderFloat("scale", &game->entities[selected].data.size.max, 0.0f, 200.0f);
				//ImGui::DragFloat("fine scale", &game->entities[selected].data.size, 0.0001f, 0.0f, 200.0f, "%.02f");

				ImGui::DragFloat("x", &game->world->entities[selected].rb.pos.x, 0.1f, -1000.0f, 1000.0f, "%.02f");
				ImGui::DragFloat("y", &game->world->entities[selected].rb.pos.y, 0.1f, -1000.0f, 1000.0f, "%.02f");
				ImGui::DragFloat("z", &game->world->entities[selected].rb.pos.z, 0.1f, -1000.0f, 1000.0f, "%.02f");


				ImGui::SliderFloat3("scale", &game->world->entities[selected].data.scale[0], 1.0f, 100.0f, "%1.0f");
				ImGui::SliderFloat3("collider min", &game->world->entities[selected].collider.min[0], 0.0f, 100.0f);
				ImGui::SliderFloat3("collider max", &game->world->entities[selected].collider.max[0], 0.0f, 100.0f);
				
				ImGui::DragFloat("speed", &game->world->entities[selected].rb.speed, 0.10f, 0.0f, 200.0f, "%.10f");
				ImGui::DragFloat("angle", &game->world->entities[selected].spriteAngleDegrees, 0.10f, -180.0f, 360.0f, "%.10f");
				
				ImGui::SliderInt("mesh index", &game->world->entities[selected].data.meshIndex, 1, Mon::g_Assets->meshCount - 1);
				ImGui::SliderInt("texture index", &game->world->entities[selected].data.textureIndex, 1, game->renderer.textureCount - 1);
				
				ImGui::Checkbox("Wireframe", &game->world->entities[selected].data.wireFrame);
				ImGui::Checkbox("Visible", &game->world->entities[selected].data.visible);
				ImGui::SameLine();
				ImGui::Checkbox("Show collider", &game->world->entities[selected].collider.data.visible);
				//ImGui::Checkbox("show normals", &g_Game->objects[selected]->viewNormals);
				//ImGui::DragFloat("rot x", &g_Game->objects[selected]->orientation.x, 0.05f, -1000.0f, 1000.0f, "%.02f");
				//ImGui::DragFloat("rot y", &g_Game->objects[selected]->orientation.y, 0.05f, -1000.0f, 1000.0f, "%.02f");
				//ImGui::DragFloat("rot z", &g_Game->objects[selected]->orientation.z, 0.05f, -1000.0f, 1000.0f, "%.02f");
				ImGui::EndTabItem();
			}



			// Water render data
			if (game->world->entities[selected].data.programType == MonGL::ProgramType::Water)
			{
				if (ImGui::BeginTabItem("Water options"))
				{
					ImGui::DragFloat("uJump", &game->world->entities[selected].data.programData.uJump, 0.001f, 0.0f, 0.25f, "%.02f");
					ImGui::DragFloat("vJump", &game->world->entities[selected].data.programData.vJump, 0.001f, 0.0f, 0.25f, "%.02f");
					ImGui::DragFloat("tiling", &game->world->entities[selected].data.programData.tiling, 0.001f, 0.0f, 10.00f, "%.01f");
					ImGui::DragFloat("speed", &game->world->entities[selected].data.programData.speed, 0.001f, 0.0f, 2.0f, "%.01f");
					ImGui::DragFloat("flow strength", &game->world->entities[selected].data.programData.flowStrength, 0.001f, 0.0f, 0.5f, "%.02f");
					ImGui::DragFloat("flow offset", &game->world->entities[selected].data.programData.flowOffset, 0.001f, -1.5f, 2.0f, "%.02f");
					ImGui::SliderFloat("height scale", &game->world->entities[selected].data.programData.heightScale, 0.0f, 5.0f);
					ImGui::SliderFloat("height scale modulated", &game->world->entities[selected].data.programData.heightScaleModulated, 0.0f, 20.0f);
					ImGui::SliderFloat("wave length", &game->world->entities[selected].data.programData.waveLength, 0.0f, 100.0f);


					ImGui::EndTabItem();
				}
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

void RenderWindow(bool* p_open, Mon::GameState* game)
{
	ImGui::Begin("Renderer", p_open);

	static unsigned int selectedLight = 1;
	ImGui::BeginChild("left pane renderer", ImVec2(150.0f, 0.0f), true);

	for (unsigned int i = 1; i < game->renderer.lightCount; ++i)
	{
		char label[128];
		sprintf_s(label, "%s %d", game->renderer.lights[i].id, i);
		if (ImGui::Selectable(label, selectedLight == i))
		{
			selectedLight = i;
		}
	}

	ImGui::EndChild();
	ImGui::SameLine();

	ImGui::BeginGroup();
	ImGui::BeginChild("light details", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us

	ImGui::Text("%s", game->renderer.lights[selectedLight].id);
	ImGui::Separator();
	if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None))
	{
		if (ImGui::BeginTabItem("details"))
		{
			//static char buf[32];
			//sprintf_s(buf, "%s", std::to_string(Mon::g_Assets->meshes[selected].VAO).c_str());
			//ImGui::Text("VAO %s", buf, IM_ARRAYSIZE(buf));

			ImGui::DragFloat("x", &game->renderer.lights[selectedLight].pos.x, 0.1f, -1000.0f, 1000.0f, "%.02f");
			ImGui::DragFloat("y", &game->renderer.lights[selectedLight].pos.y, 0.1f, -1000.0f, 1000.0f, "%.02f");
			ImGui::DragFloat("z", &game->renderer.lights[selectedLight].pos.z, 0.1f, -1000.0f, 1000.0f, "%.02f");
			ImGui::SliderFloat3("ambient", &game->renderer.lights[selectedLight].ambient[0], 0.0f, 1.0f, "%0.01f");
			ImGui::SliderFloat3("diffuse", &game->renderer.lights[selectedLight].diffuse[0], 0.0f, 1.0f, "%0.01f");
			ImGui::SliderFloat3("specular", &game->renderer.lights[selectedLight].specular[0], 0.0f, 1.0f, "%0.01f");


			if (ImGui::Button("dim"))
			{
				game->renderer.lights[selectedLight].pos.x = 0.0f;
				game->renderer.lights[selectedLight].pos.y = 3.0f;
				game->renderer.lights[selectedLight].pos.z = 0.0f;
				game->renderer.lights[selectedLight].ambient = Mon::v3(0.2f);
				game->renderer.lights[selectedLight].diffuse = Mon::v3(1.0f);
				game->renderer.lights[selectedLight].specular = Mon::v3(0.3f);
			}
			ImGui::SameLine();
			if (ImGui::Button("avg"))
			{
				game->renderer.lights[selectedLight].pos.x = 0.0f;
				game->renderer.lights[selectedLight].pos.y = 20.0f;
				game->renderer.lights[selectedLight].pos.z = 0.0f;
				game->renderer.lights[selectedLight].ambient = Mon::v3(0.3f);
				game->renderer.lights[selectedLight].diffuse = Mon::v3(0.8f);
				game->renderer.lights[selectedLight].specular = Mon::v3(0.3f);
			}

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::EndChild();
	ImGui::EndGroup();

	ImGui::End();
}

void AssetWindow(bool* p_open, Mon::GameState* game)
{
	ImGui::Begin("Assets", p_open);

	static unsigned int selected = 1;
	ImGui::BeginChild("left pane assets", ImVec2(150.0f, 0.0f), true);

	for (unsigned int i = 1; i < Mon::g_Assets->meshCount; ++i)
	{
		char label[128];
		sprintf_s(label, "%s %d", Mon::g_Assets->meshes[i].id, i);
		if (ImGui::Selectable(label, selected == i))
		{
			selected = i;
		}
	}

	ImGui::EndChild();
	ImGui::SameLine();

	ImGui::BeginGroup();
	ImGui::BeginChild("mesh details", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us

	ImGui::Text("%s", Mon::g_Assets->meshes[selected].id);
	ImGui::Separator();
	if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None))
	{
		if (ImGui::BeginTabItem("details"))
		{
			static char buf[64];
			sprintf_s(buf, "%s", std::to_string(Mon::g_Assets->meshes[selected].VAO).c_str());
			ImGui::Text("VAO %s", buf, IM_ARRAYSIZE(buf));
			
			sprintf_s(buf, "%s", std::to_string(Mon::g_Assets->meshes[selected].verticeCount).c_str());
			ImGui::Text("Vertice Count %s", buf, IM_ARRAYSIZE(buf));

			sprintf_s(buf, "%s", std::to_string(Mon::g_Assets->meshes[selected].indiceCount).c_str());
			ImGui::Text("Indice Count %s", buf, IM_ARRAYSIZE(buf));

			sprintf_s(buf, "%s", RenderTypeText(Mon::g_Assets->meshes[selected].type));
			ImGui::Text("Type %s", buf, IM_ARRAYSIZE(buf));

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::EndChild();
	ImGui::EndGroup();

	ImGui::End();
}

void DebugWindow(bool* p_open, Mon::GameState* game)
{
	ImGui::Begin("Debug Info");

	ImGui::DragFloat("line one x", &game->lineOne.pos.x, 0.01f, 1.0f, 2000.0f, "%.02f");
	ImGui::DragFloat("line two z", &game->lineTwo.pos.z, 0.01f, 1.0f, 2000.0f, "%.02f");

	ImGui::Checkbox("draw collisions", &game->drawCollisions);

	ImGui::End();
}

void StatsWindow(bool* p_open, Mon::GameState* game)
{
	ImGui::Begin("stats for me", p_open);

	//char buffer[64];
	//
	//snprintf(buffer, sizeof(buffer), "%f", game->input.mouseOffset.x);
	//ImGui::LabelText(buffer, "mouse x:");
	//snprintf(buffer, sizeof(buffer), "%f", game->input.mouseOffset.y);
	//ImGui::LabelText(buffer, "mouse y:");
	//snprintf(buffer, sizeof(buffer), "%f", game->input.mouseScreen.x);
	//ImGui::LabelText(buffer, "win x:");
	//snprintf(buffer, sizeof(buffer), "%f", game->input.mouseScreen.y);
	//ImGui::LabelText(buffer, "win y:");

	//snprintf(buffer, sizeof(buffer), "%f", game->picker.currentTerrainPoint.y);
	//ImGui::LabelText(buffer, "current terrain point y:");

	//snprintf(buffer, sizeof(buffer), "%f", game->input.stickAverageX);
	//ImGui::LabelText(buffer, "stick average X:");
	//snprintf(buffer, sizeof(buffer), "%f", (float)game->input.stickAverageY);
	//ImGui::LabelText(buffer, "stick average Y:");

	//snprintf(buffer, sizeof(buffer), "%f", game->input.stickAvgRX);
	//ImGui::LabelText(buffer, "right stick average X:");
	//snprintf(buffer, sizeof(buffer), "%f", (float)game->input.stickAvgRY);
	//ImGui::LabelText(buffer, "right stick average Y:");

	//snprintf(buffer, sizeof(buffer), "%d", game->input.lMouseBtn.endedDown);
	//ImGui::LabelText(buffer, "left mouse down");



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

	//snprintf(buffer, sizeof(buffer), "%f", game->cameras[game->currCameraIndex].pos.x);
	//ImGui::LabelText(buffer, "cam x");
	//snprintf(buffer, sizeof(buffer), "%f", game->cameras[game->currCameraIndex].pos.y);
	//ImGui::LabelText(buffer, "cam y");
	//snprintf(buffer, sizeof(buffer), "%f", game->cameras[game->currCameraIndex].pos.z);
	//ImGui::LabelText(buffer, "cam z");

	//snprintf(buffer, sizeof(buffer), "%f", game->cameras[game->currCameraIndex].right.x);
	//ImGui::LabelText(buffer, "right x");
	//snprintf(buffer, sizeof(buffer), "%f", game->cameras[game->currCameraIndex].right.y);
	//ImGui::LabelText(buffer, "right y");
	//snprintf(buffer, sizeof(buffer), "%f", game->cameras[game->currCameraIndex].right.z);
	//ImGui::LabelText(buffer, "right z");

	//snprintf(buffer, sizeof(buffer), "%f", game->cameras[game->currCameraIndex].front.x);
	//ImGui::LabelText(buffer, "front x");
	//snprintf(buffer, sizeof(buffer), "%f", game->cameras[game->currCameraIndex].front.y);
	//ImGui::LabelText(buffer, "front y");
	//snprintf(buffer, sizeof(buffer), "%f", game->cameras[game->currCameraIndex].front.z);
	//ImGui::LabelText(buffer, "front z");

	//snprintf(buffer, sizeof(buffer), "%f", game->cameras[game->currCameraIndex].pitch);
	//ImGui::LabelText(buffer, "pitch");
	//snprintf(buffer, sizeof(buffer), "%f", game->cameras[game->currCameraIndex].yaw);
	//ImGui::LabelText(buffer, "yaw");

	ImGui::End();
}

void UpdateGui(SDL_Window* window, Settings* settings, Mon::GameState* game)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(window);

	ImGui::NewFrame();
	static bool showDemoWindow = false;

	static bool showTerrainWindow = true;
	static bool showCameraWindow = true;
	static bool showEntityWindow = true;
	static bool showDebugWindow = true;
	static bool showStatsWindow = false;
	static bool showAssetWindow = false;
	static bool showRenderWindow = true;
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
	if (showAssetWindow)
		AssetWindow(&showAssetWindow, game);
	if (showRenderWindow)
		RenderWindow(&showRenderWindow, game);
	if (showDebugWindow)
		DebugWindow(&showDebugWindow, game);

	ImGui::Begin("DEBUG MENU");

	ImGui::Separator();
		ImGui::PushID(0);
			ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::ImColor(170, 40, 44));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::ImColor(105, 24, 27));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::ImColor(66, 15, 16));
			if (ImGui::Button("debug")) { Mon::DebugMode(game); }
			ImGui::PopStyleColor(3);
		ImGui::PopID();
		ImGui::SameLine();

		ImGui::PushID(0);
			ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::ImColor(16, 169, 35));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::ImColor(31, 80, 18));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::ImColor(34, 64, 35));
			if (ImGui::Button("play") && game->mode != Mon::Mode::Play) { Mon::PlayMode(game); }
			ImGui::PopStyleColor(3);
		ImGui::PopID();
		if (ImGui::Button("save")) 
		{
			WriteEntities(game->world->entities, game->world->entityCount, game->mainShaderID); 
			Mon::Log::print("Saved game to master file");
			Mon::Log::warn("Only one master save file active!!!");
		}
		ImGui::SameLine();
		if (ImGui::Button("load")) 
		{ 
			LoadSceneFile(game); 
			Mon::Log::print("Loaded saved scene");
			Mon::Log::warn("Only one master save file active!!!");
		}
		ImGui::SameLine();

		if (ImGui::Button("Fullscreen"))
		{
			SDL_DisplayMode dm;
			SDL_GetCurrentDisplayMode(0, &dm);
			SDL_SetWindowSize(window, dm.w, dm.h);
			SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
			Mon::SetViewPort(game, dm.w, dm.h);
		}
		ImGui::SameLine();
		if (ImGui::Button("Exit"))
		{
			SDL_SetWindowSize(window, settings->windowWidth, settings->windowHeight);
			SDL_SetWindowFullscreen(window, 0);
			Mon::SetViewPort(game, 960, 540);
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
		ImGui::SameLine();
		ImGui::Checkbox("Renderer", &showRenderWindow);
		ImGui::SameLine();
		ImGui::Checkbox("Assets", &showAssetWindow);

		ImGui::Checkbox("Debug Info", &showDebugWindow);


		ImGui::DragFloat("cam speed", &game->cameras[game->currCameraIndex].speed, 0.01f, 1.0f, 200.0f, "%.02f");
		ImGui::DragFloat("material shininess[move]", &game->setup.materialShininess, 0.01f, 1.0f, 200.0f, "%.02f");
		

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

	{
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

		//snprintf(buffer, sizeof(buffer), "%f", game->camera.pos.x);
		ImGui::LabelText(buffer, "cam x");
		//snprintf(buffer, sizeof(buffer), "%f", game->camera.pos.y);
		ImGui::LabelText(buffer, "cam y");

		snprintf(buffer, sizeof(buffer), "%d", GuiActive(false));
		ImGui::LabelText(buffer, "gui active");
	}
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
	if (ImGui::Button("debug")) { DebugMode(game2D); }
	ImGui::SameLine();
	if (ImGui::Button("play") && game2D->state != Mon::State::Play) { PlayMode(game2D); }
	ImGui::SameLine();
	if (ImGui::Button("save"))
	{
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
		Mon::SetViewPort(game2D->config, dm.w, dm.h);
	}
	ImGui::SameLine();
	if (ImGui::Button("Exit"))
	{
		SDL_SetWindowSize(window, settings->windowWidth, settings->windowHeight);
		SDL_SetWindowFullscreen(window, 0);
		Mon::SetViewPort(game2D->config, 960.0f, 540.0f);
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

	//ImGui::LabelText(std::to_string(game2D->dt).c_str(), "dt:");

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


	//ImGui::SliderFloat("Player Speed", &game2D->world->player->speed, 0.0f, 1000.0f);
	ImGui::SliderFloat("camera lerp", &game2D->cameras[game2D->currentCameraIndex].lerpSpeed, 0.0f, 100.0f);
	ImGui::SliderFloat("camera smooth", &game2D->cameras[game2D->currentCameraIndex].smoothness, 0.1f, 10.0f);

	{
		//char buffer[64];
		//snprintf(buffer, sizeof(buffer), "%f", game2D->world->player->pos.x);
		//ImGui::LabelText("player x", buffer);
		//snprintf(buffer, sizeof(buffer), "%f", game2D->world->player->pos.y);
		//ImGui::LabelText("player y", buffer);
	}

	ImGui::DragFloat("cam zoom", &game2D->cameras[game2D->currentCameraIndex].zoom, 0.1f, 1.0f, 200.0f, "%.02f");
	//ImGui::SliderInt("Tile 0 ID: ", &game->world2D->map->tiles[0].tileId, 0, 3, NULL);
	ImGui::DragFloat("x", &game2D->world->entities[1].pos.x, 0.1f, -1000.0f, 1000.0f, "%.02f");
	ImGui::DragFloat("y", &game2D->world->entities[1].pos.y, 0.1f, -1000.0f, 1000.0f, "%.02f");
	ImGui::Checkbox("Wireframe", &game2D->world->map->wireFrame);

	{
		//char entitysizebuf[64];
		//snprintf(entitysizebuf, sizeof(entitysizebuf), "%f", (float)game2D->world->entities.size());
	}

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
