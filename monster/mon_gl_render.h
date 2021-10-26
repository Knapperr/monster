#ifndef MON_GL_RENDER_H
#define MON_GL_RENDER_H

#include "mon_shader.h"
#include "mon_texture.h"
#include "mon_math.h"

#include <vector>

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
	struct Config
	{
		float angleDegrees;
		Rect viewPort = {};
		int tileSize = 16;
	};

	struct Vertex3D
	{
		v3 position;
		v3 normal;
		v2 texCoords;
		v3 tangent;
		v3 bitangent;
	};

	// TODO(ck): RenderData is the entities draw data I can
	// probably get rid of it and have an openGL struct that has the VAO, VBO, IBO 
	// inside of it. this is holding our textures and stuff i guess that would be part
	// of the entity?
	// I guess the opengl struct even keeps the textures and stuff in it too its controlling
	// and doing everything.

	// for now we need the RenderData to hold our vertices and textures though.
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
		std::vector<Texture> textures;
		std::string texturePath;
		int selectedTexture;

		mat4 worldMatrix;
		// TODO(ck): More collider specific info

		//float pos;
		// view 
		// projection
		// shader data?
	};

	//struct render_setup
	//{
	//	rectangle2 ClipRect;
	//	u32 RenderTargetIndex;
	//	m4x4 Proj;
	//	v3 CameraP;
	//	v3 FogDirection;
	//	v3 FogColor;
	//	f32 FogStartDistance;
	//	f32 FogEndDistance;
	//	f32 ClipAlphaStartDistance;
	//	f32 ClipAlphaEndDistance;
	//};

	// TODO(ck): So basically this is going to replace RenderData in a sense.
	// for now we need the RenderData to hold our vertices and textures though.
	struct RenderSetup
	{
		/*
			model ??? 
			viewPosition,
			cameraPosition,

			
		
		*/

		// Water Data
		float tiling;
		float speed;
		float flowStrength;
		float flowOffset;
		float heightScale;
		float heightScaleModulated;
	};

	struct Vertex
	{
		v3 position;
		v3 color;
		v2 texCoords;
	};

	struct BatchData
	{
		unsigned int VAO;
		unsigned int VBO;
	};

	struct RenderData2D
	{
		Texture texture;
		unsigned int VAO;
		unsigned int VBO;
		//Point pos;
		v2 pos;
		v3 color;
		v2 size;
		bool wireFrame;
	};

	void beginRender(Config* config, mat4 projection, mat4 view, int shaderID);
	void viewPort(Rect* port);
	void initQuad(RenderData* data, bool tangents = false);
	void loadTexture(RenderData* data, Type type, int shaderID, std::string path);
	void initBoundingBox(RenderData* data, ColliderSize* size);
	void drawQuad(Config* config, RenderData* data,
						 v3 playerPos, v3 scale, v3 camPos,
						 unsigned int shaderID, int selectedTexture = 0);
	void initDistortedWater(RenderData* renderData, RenderSetup* setup);
	void drawWater(RenderData* data, RenderSetup* setup, WaterDataProgram* waterData, Light* light, v3 pos, v3 scale, v3 camPos, unsigned int shaderID);
	void drawBoundingBox(RenderData* data,
						 ColliderSize size,
					 v3 playerPos, v3 camPos,
					 mat4 projection, mat4 view,
					 unsigned int shaderID);

	void generateTerrain(RenderData* data);
	void drawTerrain(unsigned int shaderID, RenderData* data, Light* light, mat4 projection, mat4 view, v3 camPos);


	// 2d
	void initRenderData2D(RenderData2D* sprite);
	
	void initTileMap(int tileAmount);
	void fillBatch(int tileOffsetX, int tileOffsetY, float tileXPos, float tileYPos, int tileSize);
	void bindVertices();
	
	void drawObject(CommonProgram* shader, RenderData2D* data);
	void drawMap(CommonProgram* shader, unsigned int textureID);


}
#endif