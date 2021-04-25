#ifndef MON_GUI_H
#define MON_GUI_H

#define USE_SDL

#include <imgui/imgui.h>
#ifdef USE_SDL
#include <imgui/imgui_impl_sdl.h>
#endif
#include <imgui/imgui_impl_opengl3.h>
#include <stdio.h>


#include "mon.h"



#ifdef USE_SDL
void InitGui(SDL_Window* window, SDL_GLContext* context);
void UpdateGui(SDL_Window* window, Game* game);
#else
void InitGui();
void UpdateGui();
#endif


void RenderGui();
bool GuiActive(bool leftPressed);
void ShutdownGui();

#endif