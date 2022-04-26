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
		RenderType type;

		Vertex3D* vertices;
		unsigned int* indices;
	};

	struct RenderSetup
	{
		/*
		*  RenderSetup is for the common attributes that are shared among shaders
		*  these are found in CommonProgram
			model ???
			viewPosition,
			cameraPosition
		*/
	};

	struct Material {
		v3 ambient;
		v3 diffuse;
		v3 specular;
		float shininess;
	};

	struct ProgramData
	{
		// water uniform data
		float tiling;
		float speed;
		float flowStrength;
		float flowOffset;
		float heightScale;
		float heightScaleModulated;
	};

	// TODO(ck): Better name for this
	// the mesh is inside that is technically the data to render
	// is this more of a render command or something?
	// renderoptions 
	// drawdata
	// Model? its holding the mesh and data which is often just
	// called a Model
	// glModel ?
	struct RenderData
	{
		// TODO(ck): Index for the OpenGL meshes 
		int meshIndex;
		Mesh mesh;
		Material materials[10];
		ProgramData programData;
		int programType;

		int lineWidth;

		v3 color;
		v3 scale = v3(1.0f);
		mat4 worldMatrix;

		std::string texturePath;
		int selectedTexture;
	
		// TODO(ck): hold array of indexes to texture resources
		Texture textures[4];

		bool visible;
	};

	struct InstancedData
	{
		mat4* matrices;
		int amount = 1000;
		RenderData renderData;
	};

	struct Line
	{
		v3 pos;
		RenderData data;
	};

	
	//
	// opengl renderer data is held in here
	//
	/*
		Enlighten moment about data oriented.
		this is the beauty of more data oriented coding. we have all of our data in the 
		main renderer. The data is all here and the renderdata that a game object acceses 
		just gets everything the same way as a database. this makes it super easy to hook up
		sql lite configs to this
		
		This is the main renderer that holds all of our data

		resource manager for this?
	*/
	struct OpenGL
	{
		// Data that renderdata can hold
		Mesh meshes[10]; // TODO(ck): SQLite config for size
		Texture textures[30]; // TODO(ck): SQLite config for size
		int meshCount;
		int textureCount;

		CommonProgram program;
		WaterProgram waterProgram;
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

	//
	// Renderer 
	//
	void LoadTexture(RenderData* data, int index, TextureType type, int shaderID, std::string path, bool pixelTexture = true);
	void LoadTexture(Texture* texture, TextureType type, int shaderID, std::string path, bool pixelTexture = true);
	void InitRenderer(OpenGL* gl);


	void BeginRender(Config* config, mat4 projection, mat4 view, int shaderID);
	void ViewPort(Rect* port);
	void CreateFramebuffer(Framebuffer* buffer);

	void LoadImpFile(RenderData* data);

	void UseProgram(CommonProgram* program, RenderSetup setup);
	void UseProgram(WaterProgram* program, RenderSetup setup);

	// Debug lines
	void InitLine(Line* data);
	void DrawLine(Line* data, unsigned int shaderID);

	// Models and assets
	void InitInstancedData(InstancedData* data, int amount);
	void InitQuad(Mesh* mesh, bool tangents = false);
	void InitQuad(RenderData* data);
	void InitCube(Mesh* mesh);
	void InitModel(RenderData* data);
	void InitModel(Mesh* mesh, const char* fileName);
	void InitBoundingBox(RenderData* data);
	void InitGrid(RenderData* data, int xSize, int zSize, float* heightMap);
	void InitGrid(Mesh* mesh, int xSize, int zSize);


	void Draw(OpenGL* gl, Config* config, float spriteAngleDegrees, RenderData* data, v3 pos, Camera* camera);
	void DrawBoundingBox(RenderData* data, Camera* camera, unsigned int shaderID);
	void DrawTerrain(unsigned int shaderID, RenderData* data, Light* light, Camera* camera, bool wireFrame);
	
	void DrawWater(RenderData* data, RenderSetup* setup, WaterProgram* waterData, Light* light, v3 pos, v3 scale, v3 camPos, unsigned int shaderID);

	void EndRender();

	static unsigned int AddMesh(OpenGL* gl)
	{
		unsigned int index = gl->meshCount++;

		Mesh* mesh = &gl->meshes[index];
		mesh = {};

		return index;
	}

	static Mesh* GetMesh(OpenGL* gl, unsigned int index)
	{
		Mesh* mesh = 0;
		if ((index > 0) && (index < ArrayCount(gl->meshes)))
		{
			mesh = &gl->meshes[index];
		}
		return mesh;
	}

	static unsigned int AddTexture(OpenGL* gl)
	{
		unsigned int index = gl->textureCount++;

		Texture* texture = &gl->textures[index];
		texture = {};

		return index;
	}

	static Texture* GetTexture(OpenGL* gl, unsigned int index)
	{
		Texture* t = 0;
		if ((index > 0) && (index < ArrayCount(gl->textures)))
		{
			t = &gl->textures[index];
		}
		return t;
	}


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