#ifndef MON_GL_RENDER_H
#define MON_GL_RENDER_H


#include "mon_world.h"
#include "mon_shader.h"

#include "mon_math.h"

namespace MonGL
{
	using namespace Mon;

	struct Light
	{
		v3 pos;

		v3 ambient;
		v3 diffuse;
		v3 specular;
	};

	struct Material
	{
		v3 ambient;
		v3 diffuse;
		v3 specular;
		float shininess;
	};

	struct ColliderSize
	{
		v3 min;
		v3 max;
	};

	v3 GetSize(ColliderSize* size);
	v3 GetCenter(ColliderSize* size);
	mat4 GetTransform(ColliderSize* size);

	//v3 center = v3((min_x + max_x) / 2, (min_y + max_y) / 2, (min_z + max_z) / 2);
	//mat4 transform = 

	struct Vertex3D
	{
		v3 position;
		v3 normal;
		v2 texCoords;
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
		v3 color;
		// TODO(ck): Shouldn't be here? 
		// should be initCollider i guess?
		// this isn't just a position its a MATRIX its ALL of the 
		// information its the rotation, position the size of it...
		MonTexture::Texture texture;
		mat4 worldMatrix;
		// TODO(ck): More collider specific info

		ColliderSize size;
		//float pos;
		// view 
		// projection
		// shader data?
	};

	struct Vertex
	{
		v3 position;
		v3 color;
		v2 texCoords;
	};

	struct Config
	{
		float angleDegrees;
	};

	struct BatchData
	{
		unsigned int VAO;
		unsigned int VBO;
	};

	struct RenderData2D
	{
		unsigned int VAO;
		unsigned int VBO;
		mat4 pos;
		v3 col;
		MonTexture::Texture texture;

	};

	void initQuad(RenderData* data, int shaderID, std::string texturePath);
	void initBoundingBox(RenderData* data);
	void drawQuad(Config* config, RenderData* data,
						 v3 playerPos, v3 scale, v3 camPos,
						 mat4 projection, mat4 view,
						 unsigned int shaderID);
	void drawBoundingBox(RenderData* data,
					 v3 playerPos, v3 camPos,
					 mat4 projection, mat4 view,
					 unsigned int shaderID);

	void generateTerrain(RenderData* data);
	void drawTerrain(unsigned int shaderID, RenderData* data, Light* light, mat4 projection, mat4 view, v3 camPos);

	
	// 2d
	void initRenderData(Sprite* sprite);
	
	void initTileMap(int tileAmount);
	void fillBatch(int tileOffsetX, int tileOffsetY, float tileXPos, float tileYPos);
	void bindVertices();
	
	void drawObject(Shader* shader, RenderData2D* data);
	void drawObject(Shader* shader, Entity2D* obj);
	void drawMap(Shader* shader, unsigned int textureID);
	void cleanUp(Sprite* sprite);

}
#endif