#ifndef MON_GUI_H
#define MON_GUI_H

#define USE_SDL

#include <imgui/imgui.h>
#ifdef USE_SDL
#include <imgui/imgui_impl_sdl.h>
#endif
#include <imgui/imgui_impl_opengl3.h>
#include <stdio.h>

// IMPORTANT(ck): Defines for 3D game
#define _3D_GUI_

#ifndef _3D_GUI_
#include "mon_game2D.h"
#else
#include "mon_game.h"
#endif

struct Settings
{
	const char* title;
	int windowWidth;
	int windowHeight;
	float portWidth;
	float portHeight;
	int vsync;
};

// Use for showing assets from folder and loading
struct ImpAsset
{
	std::string name;
	std::string impFilePath;
	bool loaded;
};

extern float inputTimer;

#ifdef USE_SDL
void InitGui(SDL_Window* window, SDL_GLContext* context);
#ifdef _3D_GUI_
void UpdateGui(SDL_Window* window, Settings* settings, Mon::Game* game);
#else
void UpdateGui(SDL_Window* window, Settings* settings, Mon::Game2D* game2D);
#endif

void RenderGui();
bool GuiActive(bool SDLRelativeMouseMode);
void ShutdownGui();
#endif

#endif