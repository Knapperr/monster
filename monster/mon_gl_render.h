#ifndef MON_GL_RENDER_H
#define MON_GL_RENDER_H

#include "mon_entity.h"
#include "mon_world.h"
#include "mon_texture.h"
#include "mon_shader.h"

void gl_InitRenderData(Sprite* sprite);
void gl_DrawObject(Shader* shader, Entity* obj);
void gl_DrawModel(Shader* shader);
void gl_DrawTile(Shader* shader, Tile* obj, Sprite* sprite);
void gl_DrawTile(Shader* shader, float minX, float minY, float maxX, float maxY);
void gl_CleanUp(Sprite* sprite);

#endif