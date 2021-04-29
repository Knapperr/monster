#ifndef MON_GL_RENDER_H
#define MON_GL_RENDER_H

#include "mon_entity.h"
#include "mon_world.h"
#include "mon_texture.h"
#include "mon_shader.h"

namespace MonGL
{
	struct Material
	{
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
		float shininess;
	};

	struct RenderData
	{
		unsigned int VAO;
		unsigned int VBO;
		unsigned int IBO;
		int elementLength;
		Material mat;
		int lineWidth;

		//float pos;
		// view 
		// projection
		// shader data?
	};

	// TODO(ck): gl_InitBoundingBox
	void gl_InitBoundingBox(RenderData* data);
	// TODO(ck): gl_DrawBoundingBox
	void gl_DrawBoundingBox(RenderData* data, 
					 glm::vec3 playerPos, glm::vec3 camPos,
					 glm::mat4 projection, glm::mat4 view,
					 unsigned int shaderID);



	void gl_InitRenderData(Sprite* sprite);
	void gl_DrawObject(MonShader::Shader* shader, Entity* obj);
	void gl_DrawTile(MonShader::Shader* shader, Tile* obj, Sprite* sprite);
	void gl_DrawTile(MonShader::Shader* shader, float minX, float minY, float maxX, float maxY);
	void gl_CleanUp(Sprite* sprite);
}
#endif