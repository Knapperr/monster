#ifndef MON_GL_RENDER_H
#define MON_GL_RENDER_H

#include "mon_shader.h"
#include "mon_texture.h"
#include "mon_assets.h"

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
	
	// TODO(ck): RenderData is the entities draw data I can
	// probably get rid of it and have an openGL struct that has the VAO, VBO, IBO 
	// inside of it. this is holding our textures and stuff i guess that would be part
	// of the entity?
	// I guess the opengl struct even keeps the textures and stuff in it too its controlling
	// and doing everything.

	struct RenderSetup
	{
		/*
		*  RenderSetup is for the common attributes that are shared among shaders
		*  these are found in CommonProgram cameraPosition
		*/
		float time;
		mat4 viewMatrix;
		mat4 projection;
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
		float uJump;
		float vJump;
		float tiling;
		float speed;
		float flowStrength;
		float flowOffset;
		float heightScale;
		float heightScaleModulated;
		float waveLength;
	};

	enum ProgramType
	{
		Common,
		Water
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
		int textureIndex;
		ProgramType programType;
		int lineWidth;
		bool visible;
		bool wireFrame;

		ProgramData programData;

		v3 color;
		v3 scale = v3(1.0f);
		mat4 worldMatrix;
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
		// Textures[30] -- these are loaded from the Assets.Images
		Texture textures[30];
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
		unsigned int meshIndex;
		unsigned int textureIndex;

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
	void LoadTexture(RenderData* data, int index, TextureType type, bool pixelArtTexture, int shaderID, std::string path, bool pixelTexture = true);
	void InitRenderer(OpenGL* gl);
	void UploadOpenGLMesh(Mesh* mesh);


	void BeginRender(Config* config, mat4 projection, mat4 view, int shaderID);
	void ViewPort(Rect* port);
	void CreateFramebuffer(Framebuffer* buffer);

	void LoadImpFile(RenderData* data);
	void LoadTextureFile(Texture* texture, Image* image, TextureType type, bool linearFilter = false, bool pixelArtTexture = false);


	void UseProgram(CommonProgram* program, RenderSetup setup);
	void UseProgram(WaterProgram* program, RenderSetup setup);

	// Debug lines
	void InitLineMesh(Mesh* mesh);
	void InitLine(Line* data);
	void DrawLine(OpenGL* gl, Line* data);

	// Render data 
	void InitInstancedData(InstancedData* data, int amount);
	void InitModel(RenderData* data);
	void InitBoundingBox(RenderData* data);

	void Draw(OpenGL* gl, Config* config, RenderSetup setup, float spriteAngleDegrees, RenderData* data, v3 pos, Camera* camera);

	void DrawBoundingBox(OpenGL* gl, RenderData* data, Camera* camera);
	void DrawTerrain(OpenGL* gl, RenderData* data, Light* light, Camera* camera);
	
	void DrawWater(RenderData* data, RenderSetup* setup, WaterProgram* waterData, Light* light, v3 pos, v3 scale, v3 camPos, unsigned int shaderID);

	void EndRender();


	static unsigned int AddTexture(OpenGL* gl)
	{
		unsigned int index = gl->textureCount++;

		MonGL::Texture* texture = &gl->textures[index];
		texture = {};

		return index;
	}

	static MonGL::Texture* GetTexture(OpenGL* gl, unsigned int index)
	{
		MonGL::Texture* t = 0;
		if ((index > 0) && (index < ArrayCount(gl->textures)))
		{
			t = &gl->textures[index];
		}
		return t;
	}


	//
	// 2d
	//

	void InitRenderer2D(OpenGL* gl);

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
#endif // MON_GL_RENDER_H


