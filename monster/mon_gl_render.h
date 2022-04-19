#ifndef MON_GL_RENDER_H
#define MON_GL_RENDER_H

#include "mon_shader.h"
#include "mon_texture.h"
#include "mon_camera.h"

#include <vector>
#include <stack>

namespace MonGL
{
	using namespace Mon;

	//
	//	3D structs
	//

	struct Framebuffer
	{
		unsigned int handle;
		int width;
		int height;
		Texture texture[2];
	};

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
		// TODO(ck): smaller types on vertex? position=vec3, color=uint32, texCoord=short2
		v3 position;
		v3 normal;
		v2 texCoords;
		v3 tangent;
		v3 bitangent;
	};

	enum class RenderType
	{
		None,
		Model,
		Quad,
		Cube,
		Debug,
	};
	
	// TODO(ck): RenderData is the entities draw data I can
	// probably get rid of it and have an openGL struct that has the VAO, VBO, IBO 
	// inside of it. this is holding our textures and stuff i guess that would be part
	// of the entity?
	// I guess the opengl struct even keeps the textures and stuff in it too its controlling
	// and doing everything.

	struct Mesh
	{
		unsigned int VAO;
		unsigned int VBO;
		unsigned int IBO;
		int verticeCount;
		int indiceCount;
	};

	// for now we need the RenderData to hold our vertices and textures though.
	struct RenderData
	{
		Mesh mesh;
		// TODO(ck): Index into shader ... this should get the correct shader
		// and then go through its uniforms and activate them using the Program
		// and the render setup
		// the render setup has everything in it so it just needs to find the right 
		// program and then use the RenderData setup
		/*
		    UseProgramBegin(OpenGL, &Prog->Common);
    
			OpenGL->glUniformMatrix4fv(Prog->TransformID, 1, GL_TRUE, Setup->Proj.E[0]);
			OpenGL->glUniform3fv(Prog->CameraP, 1, Setup->CameraP.E);
			OpenGL->glUniform3fv(Prog->FogDirection, 1, Setup->FogDirection.E);
			OpenGL->glUniform3fv(Prog->FogColor, 1, Setup->FogColor.E);
			OpenGL->glUniform1f(Prog->FogStartDistance, Setup->FogStartDistance);
		
			you can see here it just calls the programs uniforms and then the setup just activates it
			our setup is going to have everything in it because who cares? if we start having a ton
			of unique shaders we can also differentiate the setups as well.
		
		*/
		RenderSetup setup;
		int shaderIndex;

		// NOTE(ck): Vertices and indices are outside of the Mesh as they are uploaded to it.
		Vertex3D* vertices;
		unsigned int* indices;
		int lineWidth;

		v3 color;
		v3 scale = v3(1.0f);
		mat4 worldMatrix;

		std::string texturePath;
		int selectedTexture;
	
		// TODO(ck): hold id to an array of textures
		Texture textures[4];
		Material mat;

		// Note(ck): For saving and loading scene files
		RenderType type;

		bool visible;
	};

	// Change to batch data
	struct BatchData3D
	{
		unsigned int VAO;
		unsigned int VBO;
		unsigned int EBO;
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

	struct RenderSetup
	{
		/*
			model ??? 
			viewPosition,
			cameraPosition
		*/

		// Water Data
		float tiling;
		float speed;
		float flowStrength;
		float flowOffset;
		float heightScale;
		float heightScaleModulated;
	};

	struct Line
	{
		v3 pos;
		RenderData data;
	};

	//
	// 2D structs 
	//
	struct Vertex
	{
		// TODO(ck): smaller types on vertex? position=vec3, color=uint32, texCoord=short2
		v3 position;
		v3 color;
		v2 texCoords;
	};

	struct BatchData
	{
		Mesh mesh;
		mat4 matrix = mat4(1.0f);
		std::stack<mat4> matrixStack;

		uint32_t* indices;
		int maxVertices;
		int indicesLength;
		int quadCount;
	};

	struct RenderData2D
	{
		Texture texture;
		unsigned int VAO;
		unsigned int VBO;

		// TODO(ck): Should this use a point?? probably
		//Point pos;
		v2 pos;
		v3 color;
		v2 size;
		bool wireFrame;

		std::vector<Vertex> vertices;
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

	// ResourceManager
	//	shaders[]
	//	Textures[]

	void BeginRender(Config* config, mat4 projection, mat4 view, int shaderID);
	void ViewPort(Rect* port);
	void CreateFramebuffer(Framebuffer* buffer);

	void LoadImpFile(RenderData* data);

	// Debug lines
	void InitLine(Line* data);
	void DrawLine(Line* data, unsigned int shaderID);

	// Models and assets
	void InitInstancedData(InstancedData* data, int amount);
	void InitQuad(RenderData* data, bool tangents = false);
	void InitCube(RenderData* data);
	void InitModel(RenderData* data);
	void InitBoundingBox(RenderData* data);
	void InitDistortedWater(RenderData* renderData, RenderSetup* setup);
	void InitGrid(RenderData* data, int xSize, int zSize, float* heightMap);
	void LoadTexture(RenderData* data, int index, TextureType type, int shaderID, std::string path, bool pixelTexture = true);
	
	void Draw(Config* config, float spriteAngleDegrees, RenderData* data, v3 pos, Camera* camera,
			  unsigned int shaderID, int selectedTexture = 0);	
	void DrawBoundingBox(RenderData* data, Camera* camera, unsigned int shaderID);
	void DrawTerrain(unsigned int shaderID, RenderData* data, Light* light, Camera* camera, bool wireFrame);
	
	void DrawWater(RenderData* data, RenderSetup* setup, WaterDataProgram* waterData, Light* light, v3 pos, v3 scale, v3 camPos, unsigned int shaderID);

	void EndRender();


	//
	// 2d
	//

	void InitOpenGLBatchMesh(Mesh* mesh);

	void PushMatrix(BatchData* batch, mat4 matrix);
	void PopMatrix(BatchData* batch);

	void InitLine2D(RenderData2D* data);
	void DrawLine2D(RenderData2D* data);

	void InitRenderData2D(RenderData2D* sprite, int size);
	
	void FillTileVertices(RenderData2D* sprite, int tileOffsetX, int tileOffsetY, float tileXPos, float tileYPos, int tileSize);
	void InitTileMap(int tileAmount);
	void FillBatch(int tileOffsetX, int tileOffsetY, float tileXPos, float tileYPos, int tileSize);
	void BindVertices();
	
	void DrawObject(CommonProgram* shader, RenderData2D* data);
	void DrawMap(CommonProgram* shader, RenderData2D* sprite, unsigned int textureID);
	void DrawMap(CommonProgram* shader, unsigned int textureID, bool wireFrame);
	void DrawMap(CommonProgram* shader, unsigned int textureID, int batchThing);


}
#endif