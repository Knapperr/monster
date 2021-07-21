#ifndef MON_GL_RENDER_H
#define MON_GL_RENDER_H

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>

#include "mon_world.h"
#include "mon_shader.h"


namespace MonGL
{

	struct Light
	{
		glm::vec3 pos;

		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
	};

	struct Material
	{
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
		float shininess;
	};

	struct ColliderSize
	{
		glm::vec3 min;
		glm::vec3 max;
	};

	glm::vec3 GetSize(ColliderSize* size);
	glm::vec3 GetCenter(ColliderSize* size);
	glm::mat4 GetTransform(ColliderSize* size);

	//glm::vec3 center = glm::vec3((min_x + max_x) / 2, (min_y + max_y) / 2, (min_z + max_z) / 2);
	//glm::mat4 transform = 

	struct Vertex3D
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texCoords;
	};

	// MESH
	struct RenderData
	{
		std::vector<Vertex3D> vertices;

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
		MonTexture::Texture texture;
		glm::mat4 worldMatrix;
		// TODO(ck): More collider specific info

		ColliderSize size;
		//float pos;
		// view 
		// projection
		// shader data?
	};

	struct Vertex
	{
		glm::vec3 position;
		glm::vec3 color;
		glm::vec2 texCoords;
	};

	struct BatchData
	{
		unsigned int VAO;
		unsigned int VBO;
	};


	void initCharacter(RenderData* data, int shaderID, int TESTCHOOSE);
	void initBoundingBox(RenderData* data);
	void drawCharacter(RenderData* data,
						 glm::vec3 playerPos, glm::vec3 camPos,
						 glm::mat4 projection, glm::mat4 view,
						 unsigned int shaderID);
	void drawBoundingBox(RenderData* data,
					 glm::vec3 playerPos, glm::vec3 camPos,
					 glm::mat4 projection, glm::mat4 view,
					 unsigned int shaderID);

	void generateTerrain(RenderData* data);
	void drawTerrain(unsigned int shaderID, RenderData* data, Light* light, glm::mat4 projection, glm::mat4 view,
					 glm::vec3 camPos);

	
	// 2d
	void initRenderData(Sprite* sprite);
	
	void initTileMap(int tileAmount);
	void fillBatch(int tileOffsetX, int tileOffsetY, float tileXPos, float tileYPos);
	void bindVertices();
	
	void drawObject(MonShader::Shader* shader, Entity* obj);
	void drawMap(MonShader::Shader* shader, unsigned int textureID);
	void cleanUp(Sprite* sprite);

}
#endif