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

	enum class LightType
	{
		Point,
		Directional,
		Spot
	};

	struct Light
	{
		const char* id;

		v3 pos;
		v3 ambient;
		v3 diffuse;
		v3 specular;

		bool attachedToEntity;
		LightType type;
	};

	struct Config
	{
		float spriteAngleDegrees;
		Rect viewPort;
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

	// RenderSetup is for the common attributes that are shared among these are found in CommonProgram
	struct RenderSetup
	{
		float materialShininess;
		float time;
		mat4 viewMatrix;
		mat4 projection;
	};

	struct Material
	{
		v3 ambient;
		v3 diffuse;
		v3 specular;
		float shininess;
	};

	struct ProgramData
	{
		// common uniform data
		float texCoordScale;

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
	struct RenderData
	{
		// TODO(ck): Index for the mesh from g_Assets
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

	struct Batch
	{
		unsigned int VAO;
		unsigned int VBO;
		unsigned int IBO;

		Vertex3D* vertices;
		std::vector<Vertex3D> vertices_;
		unsigned int* indices;
		int usedIndices;

		int verticeCount;
		int indiceCount;
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

	struct FramebufferGL
	{
		unsigned int handle;
	};

	struct Cubemap
	{
		unsigned int VAO;
		unsigned int VBO;
	};


	//
	// 2D structs 
	//
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
		unsigned int IBO;
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

		v2 pos;
		v3 color;
		v2 size;
		bool wireFrame;

		std::vector<Vertex> vertices;
	};

	//
	// Main Renderer 
	//
	struct OpenGL
	{
		// TODO(ck): keep the batches separated for now until
		// we know that they work properly then we will combine for the grid
		// and the 
		Batch batch;
		Batch gridBatch;
		Light lights[32];
		Texture textures[32]; // These use images from the asset pipeline
		BatchData batches[4];
		int lightCount;
		int textureCount;
		int batchCount;

		Cubemap cubemap;

		CommonProgram program;
		QuadBatchProgram quadProgram;
		WaterProgram waterProgram;
		CubemapProgram cubemapProgram;

		// Separate this out from the renderer?
		FramebufferGL buffer;
		FramebufferGL textureColorbuffer;
	};


	// TODO(ck): TODO(ck): Somehow put into game so we can call from gui
	// can have a getter method that retrieves the globalDrawCalls from here
	extern int globalDrawCalls;

	//
	// Renderer 
	//
	
	// TODO(ck): Remove std::string
	void LoadTexture(std::string name, Texture* texture, TextureType type, bool pixelArtTexture, Image* image);
	void InitRenderer(OpenGL* gl);
	void UploadOpenGLMesh(Mesh* mesh);
	void UploadOpenGLMesh2D(Mesh2D* mesh);

	void BeginRender(OpenGL* gl, Config* config, mat4 projection, mat4 view, int shaderID);
	void ViewPort(Rect* port);

	void LoadImpFile(RenderData* data);
	// TODO(ck): Remove std::string
	void LoadTextureFile(std::string name, Texture* texture, Image* image, TextureType type, bool linearFilter = false, bool pixelArtTexture = false);

	void UseProgram(CommonProgram* program);
	void UseProgram(CommonProgram* program, RenderSetup setup);
	void UseProgram(QuadBatchProgram* program, RenderSetup setup);
	void UseProgram(WaterProgram* program, RenderSetup setup);

	// Debug lines
	void InitLineMesh(Mesh* mesh);
	void InitLine(Line* data);
	void DrawLine(OpenGL* gl, Line* data);

	// Render data 
	void InitBatch(OpenGL* gl);
	void BindBatchVertices(OpenGL* gl);
	void FillBatch(OpenGL* gl);
	void DrawBatch(OpenGL* gl);

	void InitInstancedData(InstancedData* data, int amount);
	void SetModel(RenderData* data);
	void SetBoundingBox(RenderData* data);

	void Draw(OpenGL* gl, Config* config, RenderSetup setup, float spriteAngleDegrees, RenderData* data, v3 pos, Camera* camera);
	void DrawLights(OpenGL* gl);
	void DrawCubeMap(OpenGL* gl, RenderSetup setup);

	void DrawBoundingBox(OpenGL* gl, RenderData* data, Camera* camera);
	void DrawTerrain(OpenGL* gl, RenderData* data, Camera* camera);
	
	void InitBatch(OpenGL* gl);
	void FillBatch(OpenGL* gl);

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

	static unsigned int AddLight(OpenGL* gl)
	{
		unsigned int index = gl->lightCount++;

		Light* light = &gl->lights[index];
		light = {};

		return index;
	}

	static Light* GetLight(OpenGL* gl, unsigned int index)
	{
		MonGL::Light* l = 0;
		if ((index > 0) && (index < ArrayCount(gl->lights)))
		{
			l = &gl->lights[index];
		}
		return l;
	}

	static BatchData* GetBatch(OpenGL* gl, unsigned int index)
	{
		MonGL::BatchData* b = 0;
		if ((index > 0) && (index < ArrayCount(gl->batches)))
		{
			b = &gl->batches[index];
		}
		return b;
	}

	static unsigned int AddBatch(OpenGL* gl)
	{
		unsigned int index = gl->batchCount++;

		BatchData* batch = &gl->batches[index];
		batch = {};

		return index;
	}



	//
	// 2D
	//
	void InitRenderer2D(OpenGL* gl);
	void InitOpenGLBatchMesh(Mesh* mesh);

	void PushMatrix(BatchData* batch, mat4 matrix);
	void PopMatrix(BatchData* batch);

	void InitLine2D(RenderData2D* data);
	void DrawLine2D(RenderData2D* data);

	void InitRenderData2D(RenderData2D* sprite, int size);
	
	void InitTileMap(int tileAmount); // This just inits the batch the same way InitBatch does
	void InitBatch(int tileAmount);
	void InitBatch(BatchData* batch, int tileAmount);
	void InitBatch(OpenGL* gl, int batchIndex, int tileAmount);
	void FillBatch(int tileOffsetX, int tileOffsetY, float tileXPos, float tileYPos, int tileSize, v2 cameraPos);
	void FillBatch(float sheetSize, int tileSize, float worldX, float worldY, v2 textureOffset, v2 cameraPos);
	void BindVertices();
	void BindVertices(BatchData* batch);
	
	void DrawObject(CommonProgram* shader, RenderData2D* data, v2 cameraPos);
	void DrawMap(CommonProgram* shader, v2 cameraPos, unsigned int textureID, bool wireFrame);
	void DrawMap(CommonProgram* shader, unsigned int textureID, int batchThing);
	void DrawBatch(CommonProgram* shader, unsigned int textureID, bool wireFrame);

}
#endif // MON_GL_RENDER_H


