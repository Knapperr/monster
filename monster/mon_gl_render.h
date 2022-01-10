#ifndef MON_GL_RENDER_H
#define MON_GL_RENDER_H

#include "mon_shader.h"
#include "mon_texture.h"
#include "mon_camera.h"

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

	enum class RenderType
	{
		Model,
		Quad,
		Cube
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

		Vertex3D* vertices;
		unsigned int* indices;

		unsigned int VAO;
		unsigned int VBO;
		unsigned int IBO;
		int verticeCount;
		int indiceCount;
		int lineWidth;

		v3 color;
		v3 scale;
		mat4 worldMatrix;

		std::string texturePath;
		int selectedTexture;
		Texture textures[4];
		Material mat;

		// Note(ck): For saving and loading scene files
		RenderType type;

		bool visible;
		// TODO(ck): More collider specific info

		//float pos;
		// view 
		// projection
		// shader data?
	};

	struct InstancedData
	{
		mat4* matrices;
		int amount = 1000;
		RenderData renderData;
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


	// TODO(ck): TODO(ck): Somehow put into game so we can call from gui
	// can have a getter method that retrieves the globalDrawCalls from here
	extern int globalDrawCalls;

	// Some kind of shader manager for accessing shaders from the render layer
	// dont think i need some kind of manager for generic shaders.
	// ill have few shaders i need to focus on the render layer instead and 
	// get frame buffers all of that stuff going. learn open gl weekend?
	//class ShaderManager
	//{
	//public:
	//	
	//	//CommonProgram programs[];
	//};

	void BeginRender(Config* config, mat4 projection, mat4 view, int shaderID);
	void ViewPort(Rect* port);
	
	void InitInstancedData(InstancedData* data, int amount);
	void InitQuad(RenderData* data, bool tangents = false);
	void InitCube(RenderData* data);
	void InitModel(RenderData* data);
	void InitBoundingBox(RenderData* data);
	void InitDistortedWater(RenderData* renderData, RenderSetup* setup);
	void GenerateTerrain(RenderData* data, float* heightMap);
	void LoadTexture(RenderData* data, int index, TextureType type, int shaderID, std::string path, bool pixelTexture = true);
	
	void Draw(Config* config, RenderData* data, v3 pos, Camera* camera,
			  unsigned int shaderID, int selectedTexture = 0);	
	void DrawBoundingBox(RenderData* data, Camera* camera, unsigned int shaderID);
	void DrawTerrain(unsigned int shaderID, RenderData* data, Light* light, Camera* camera, bool wireFrame);
	
	void DrawWater(RenderData* data, RenderSetup* setup, WaterDataProgram* waterData, Light* light, v3 pos, v3 scale, v3 camPos, unsigned int shaderID);

	void EndRender();


	//
	// 2d
	//
	void InitRenderData2D(RenderData2D* sprite);
	
	void InitTileMap(int tileAmount);
	void FillBatch(int tileOffsetX, int tileOffsetY, float tileXPos, float tileYPos, int tileSize);
	void BindVertices();
	
	void DrawObject(CommonProgram* shader, RenderData2D* data);
	void DrawMap(CommonProgram* shader, unsigned int textureID);
	void DrawMap(CommonProgram* shader, unsigned int textureID, int batchThing);


}
#endif