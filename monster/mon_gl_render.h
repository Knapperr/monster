#ifndef MON_GL_RENDER_H
#define MON_GL_RENDER_H

#include "mon_shader.h"
#include "mon_texture.h"
#include "mon_assets.h"

#include <vector>
#include <stack>

#include <glad/glad.h>

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

	struct BatchVertex3D
	{
		// TODO(ck): smaller types on vertex? position=vec3, color=uint32, texCoord=short2
		v3 position;
		v3 normal;
		v2 texCoords;
		v3 worldPosition;
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
		bool visible; // this goes onto entity
		bool wireFrame;

		ProgramData programData;

		v3 color;
		v3 scale = v3(1.0f);
		v3 pos;
		float angleDegrees;
		mat4 worldMatrix;
	};

	struct RenderItem
	{
		int meshIndex;
		int textureIndex;


		float angleDegrees;

		float viewZ; // calculated while traversing entities
		int uniformBufferOffset; // calculated in the uniform buffer model block upload when traversing entities
	};

	struct Batch
	{
		unsigned int VAO;
		unsigned int VBO;
		unsigned int IBO;

		unsigned int UBO;
		std::vector<mat4> modelMatrices;

		Vertex3D* vertices;
		std::vector<Vertex3D> vertices_;
		std::vector<BatchVertex3D> vertices__;
		unsigned int* indices;
		int usedIndices;

		int verticeCount;
		int indiceCount;

		
		int sheetTextureIndex;
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
		
		std::vector<MonGL::Vertex> vertices;
		uint32_t* indices;
		int usedIndices;
		int maxVertices;
		int indicesLength;
		int quadCount;

		// sheet index
		int textureSheetIndex;
	};

	struct GLSubTexture
	{
		float sheetSize;
		int tileSize;
		int width;
		int height;

		// bottom left, bottom right, top right, top left
		v2 texCoords[4];
	};

	struct GLFrame
	{
		float duration;
		GLSubTexture subTexture;
	};

	struct GLSpriteAnimation
	{
		int textureAtlasIndex;
		float totalDuration;

		int frameIndex;
		float frameCounter;

		GLFrame frames[32];
		int frameCount;
	};

	struct GLSpriteAnimator
	{
		int textureAtlasIndex;

		int animationIndex;
		GLSpriteAnimation animations[8];
		int animationCount;
	};

	struct BatchItem
	{
		bool isPlayer = false;
		int animationIndex;
		int subTextureIndex;

		int tileSize;
		float spriteSize;
		float cameraZ;
		v3 worldPos;
	};

	struct BatchItem2D
	{
		bool isPlayer = false;
		int animationIndex;
		int subTextureIndex;

		int tileSize;
		float spriteSize;
		v2 worldPos;
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

	struct UniformObject
	{
		unsigned int gl_handle;

		GLubyte* buffer;
		int blockSize;
		int totalSize;
	};


	// TODO(ck): Put into mon_gl_debug.h
	struct DebugVertex
	{
		glm::vec4 pos;
		glm::vec4 colour;
	};

	struct LineBuffer
	{
		DebugVertex* vertices;
		unsigned int short* indices;
		// We need this to expand... for now set a limit but eventually check this before adding more
		// and then realloc the buffers??
		int totalVerticeSize;
		int totalIndiceSize;

		int offset = 0;
		int usedIndices = 0;
		int lastVerticeIndex = 0;
		int lastIndiceIndex = 0;

		// probably want the vbo and ubo in here
		unsigned int VAO;
		unsigned int IBO;
		unsigned int VBO;
	};

	//
	// Main Renderer 
	//
	struct OpenGL
	{
		UniformObject ubo;
		LineBuffer lineBuffer;

		Batch batch;
		Light lights[32];
		Texture textures[64]; // NOTE(ck): These use images from the asset pipeline
		GLSpriteAnimator spriteAnimators[32];

		Batch batches_[4];
		BatchData batches[4];
		//BatchItem batchItems[64]; 

		std::vector<BatchItem> batchItems_;

		std::vector<RenderItem> batchItems;
		//RenderData transparentItems[512];
		//RenderData opaqueItems[512]; 
		// or dynamic buffer
		std::vector<RenderItem> transparentItems;
		std::vector<RenderItem> opaqueItems;
		
		std::vector<BatchItem2D> batchItems2D;

		int lightCount;
		int textureCount;
		int spriteAnimationCount;
		int spriteAnimatorCount;
		int batchCount2D;
		int batchCount_;

		Cubemap cubemap;

		CommonProgram program;
		CommonProgram debugProgram;
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

	// debug
	void InitLineBuffer(LineBuffer* buffer, int totalVertSize, int totalIndiceSize);
	void InitGLBuffer(LineBuffer* buffer);
	void DrawLine(LineBuffer* buffer, glm::vec4 a, glm::vec4 b, glm::vec4 colour);
	void DrawLine2D(LineBuffer* buffer, v4 a, v4 b, v4 colour);
	void DrawBox(LineBuffer* buffer, glm::vec3 min, glm::vec3 max, glm::vec4 colour);
	void DrawBox2D(LineBuffer* buffer, glm::vec3 min, glm::vec3 max, glm::vec4 colour);

	void DrawFrustum(LineBuffer* buffer, glm::vec4 vertices[]);
	void ResetBuffer(LineBuffer* buffer);


	void SetBlockSize(UniformObject* ubo, int blockSize, int blockCount);
	void InitUniformObject(UniformObject* ubo, int blockSize, int blockCount);
	
	// TODO(ck): Remove std::string
	void LoadTexture(std::string name, Texture* texture, TextureType type, bool pixelArtTexture, Image* image);
	void InitRenderer(OpenGL* gl, int entityCount);
	void UploadGLMesh(Mesh* mesh);

	void UploadOpenGLMesh2D(Mesh2D* mesh);

	void BeginRender(OpenGL* gl, Config* config, mat4 projection, mat4 view, int shaderID);
	void ViewPort(Rect* port);

	void LoadTextureFile(std::string name, Texture* texture, Image* image, TextureType type, bool linearFilter = false, bool pixelArtTexture = false);


	void Render(OpenGL* gl, Camera* camera, RenderData* gridData, mat4 projection, mat4 view);
	void DrawCubeMap(OpenGL* gl, RenderSetup setup);

	void DrawBoundingBox(OpenGL* gl, RenderData* data, Camera* camera);
	void DrawTerrain(OpenGL* gl, RenderData* data, Camera* camera);
	
	// Batching
	// TODO(ck): Batch manager to deal with these...
	//  remove the AddBatch, GetBatch methods let the manager deal
	//  with it internally. public methods in manager
	void InitBatch(OpenGL* gl, int batchIndex);

	void FillBatch(Batch* batch, float posX, float posY, float posZ, int texOffsetX, int texOffsetY, int tileSize);
	void FillBatch(Batch* batch, float posX, float posY, float posZ, float camX, float camY, float camZ, GLSubTexture* subTexture, int tileSize);
	void BindBatchVertices(Batch* batch);
	void DrawBatch(OpenGL* gl, Batch* batch, v3 cameraRight, v3 cameraUp, bool rotateBatch);
	void SortBatch(std::vector<BatchItem>& batchItems);

	// Animations
	void UpdateSpriteAnimation(GLSpriteAnimation* animation, int animationIndex, float dt);


	void EndRender(OpenGL* gl);


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


	static Batch* GetBatch(OpenGL* gl, unsigned int index)
	{
		MonGL::Batch* b = 0;
		if ((index > 0) && (index < ArrayCount(gl->batches_)))
		{
			b = &gl->batches_[index];
		}
		return b;
	}

	static unsigned int AddBatch(OpenGL* gl)
	{
		unsigned int index = gl->batchCount_++;

		Batch* batch = &gl->batches_[index];
		batch = {};

		return index;
	}

	//
	// 2D
	//
	void InitRenderer2D(OpenGL* gl);
	void InitOpenGLBatchMesh(Mesh* mesh);

	void InitRenderData2D(RenderData2D* sprite, int size);
	
	void InitBatch(BatchData* batch, int tileAmount);
	void InitBatch(OpenGL* gl, int batchIndex, int tileAmount);
	void FillBatch(BatchData* batch, float sheetSize, int tileSize, float spriteSize, float worldX, float worldY, v2 textureOffset, v2 cameraPos);	
	void FillBatch(BatchData* batch, float sheetSize, int tileSize, float spriteSize, float worldX, float worldY, GLSubTexture subTexture, v2 cameraPos);
	void BindVertices(BatchData* batch);
	
	void SortBatch2D(std::vector<BatchItem2D>& batchItems);

	void DrawObject(CommonProgram* shader, RenderData2D* data, v2 cameraPos);
	void DrawBatch(BatchData* batch, CommonProgram* shader, unsigned int textureID, bool wireFrame);

	

	static BatchData* GetBatch2D(OpenGL* gl, unsigned int index)
	{
		MonGL::BatchData* b = 0;
		if ((index > 0) && (index < ArrayCount(gl->batches)))
		{
			b = &gl->batches[index];
		}
		return b;
	}

	static unsigned int AddBatch2D(OpenGL* gl)
	{
		unsigned int index = gl->batchCount2D++;

		BatchData *batch = &gl->batches[index];
		batch = {};

		return index;
	}
}
#endif // MON_GL_RENDER_H


