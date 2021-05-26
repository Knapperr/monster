#ifndef MON_GL_RENDER_H
#define MON_GL_RENDER_H

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>

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


	struct ColliderSize
	{
		glm::vec2 x;
		glm::vec2 y;
		glm::vec2 z;
		
		float minX() { return x.x; }
		float maxX() {return x.y; }
		void minX(float minX) { x.x = minX; }
		void maxX(float maxX) { x.y = maxX; }
		void X(float minX, float maxX) { x = glm::vec2(minX, maxX); }

		float minY() { return y.x; }
		float maxY() { return y.y; }
		void minY(float minY) { y.x = minY; }
		void maxY(float maxY) { y.y = maxY; }
		void Y(float minY, float maxY) { y = glm::vec2(minY, maxY); }

		float minZ() { return z.x; }
		float maxZ() { return z.y; }
		void minZ(float minZ) { z.x = minZ; }
		void maxZ(float maxZ) { z.y = maxZ; }
		void Z(float minZ, float maxZ) { z = glm::vec2(minZ, maxZ); }
	};

	glm::vec3 GetSize(ColliderSize* size);
	glm::vec3 GetCenter(ColliderSize* size);
	glm::mat4 GetTransform(ColliderSize* size);

	//glm::vec3 center = glm::vec3((min_x + max_x) / 2, (min_y + max_y) / 2, (min_z + max_z) / 2);
	//glm::mat4 transform = 

	struct RenderData
	{
		unsigned int VAO;
		unsigned int VBO;
		unsigned int IBO;
		int elementLength;
		Material mat;
		int lineWidth;
		glm::vec3 color;
		// TODO(ck): Shouldn't be here? 
		// should be initCollider i guess?
		// this isn't just a position its a MATRIX its ALL of the 
		// information its the rotation, position the size of it...
		glm::mat4 worldMatrix;
		// TODO(ck): More collider specific info

		ColliderSize size;
		//float pos;
		// view 
		// projection
		// shader data?
	};

	struct MonVertex
	{
		glm::vec2 position;
		glm::vec4 color;
		glm::vec2 texture;
	};

	struct BatchConfig
	{
		unsigned int renderType;
		int priority;
		unsigned int textureId;
		glm::mat4 transformMatrix;
	};

	class Batch
	{
	public:
		int maxVertices;
		int usedVertices;
		unsigned int VAO;
		unsigned int VBO;

		BatchConfig config;
		MonVertex  lastVertex;
		
		Batch();
		Batch(unsigned int maxNumVertices, BatchConfig batchConfig);
		~Batch();
		
		// isBatchConfig ??
		// isEmpty ??? 
		// isEnoughRoom(numVertices);

		Batch *getFullest(Batch* batch);
		int getPriority() const;

		void add(const std::vector<MonVertex>& vertices, const BatchConfig& config);
		void add(const std::vector<MonVertex>& vertices);
		void render();
		void cleanUp();
	};

	// TODO(ck): gl_InitBoundingBox
	void gl_InitBoundingBox(RenderData* data);
	// TODO(ck): gl_DrawBoundingBox
	void gl_DrawBoundingBox(RenderData* data,
					 glm::vec3 playerPos, glm::vec3 camPos,
					 glm::mat4 projection, glm::mat4 view,
					 unsigned int shaderID);

	// 2d
	void gl_InitRenderData(Sprite* sprite);
	void gl_DrawObject(MonShader::Shader* shader, Entity* obj);
	void gl_DrawTile(MonShader::Shader* shader, Tile* obj, Sprite* sprite);
	void gl_DrawTile(MonShader::Shader* shader, float minX, float minY, float maxX, float maxY);
	void gl_CleanUp(Sprite* sprite);

}
#endif