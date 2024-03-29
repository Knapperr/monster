#ifndef MON_SHADER_H
#define MON_SHADER_H

namespace MonGL
{
	struct CommonProgram
	{
		int handle;		

		int model;
		int view;
		int projection;
		int viewPos;
		int lightPos;
		int lightAmbient;
		int lightDiffuse;
		int lightSpecular;
		int colliderColor;
		int useTexture;
		int collider;
		int pixelTexture;

		float texCoordScale;
		
		// TODO(ck): Samplers for textures
		// textures in the program
		//u32 SamplerCount;
		//GLuint Samplers[16];
		int textureDiffuse1;
	};

	struct QuadBatchProgram
	{
		CommonProgram common;

		int cameraRight_worldspace;
		int cameraUp_worldspace;
		int VP; // Model-View-Projection matrix, but without the Model (the position is in BillboardPos; the orientation depends on the camera)
		int billboardPos; // Position of the center of the billboard
		int billboardSize;
		int rotateBatch;
	};

	// TODO(ck): Match struct in the shader also make a 
	struct WaterProgram
	{
		CommonProgram common;
		int textureIndexNormal1;
		int textureIndexNormal2;

		// NOTE(ck): uniform ids for shader
		unsigned int time;
		unsigned int waveLength;
		unsigned int uJump;
		unsigned int vJump;
		unsigned int tiling;
		unsigned int speed;
		unsigned int flowStrength;
		unsigned int flowOffset;
		unsigned int heightScale;
		unsigned int heightScaleModulated;

		// texture uniforms
		unsigned int textureNormal1;
		unsigned int textureNormal2;
	};

	struct CubemapProgram
	{
		CommonProgram common;
		int skyboxTextureIndex;

		unsigned int skybox;
	};
	
	enum class ERROR_TYPE
	{
		VERTEX,
		FRAGMENT,
		GEOMETRY,
		PROGRAM
	};

	/*
		
		You can do full reflection on shader with glGet
		names, types, uniforms, attributes, samplers, everything

		you could use uniforms blocks/buffers and then share same struct between C code and GLSL code
		then do glMapBuffer for uniform buffer and memcpy all the data, to update it on GPU
	
	void GetUniforms(Shader* shader)
	{
		if (!shader->uniforms.empty())
		{
			shader->uniforms.clear();
		}

		// TODO(CK): we can use an array because we get the count right here
		int count;
		glGetProgramiv(shader->id, GL_ACTIVE_UNIFORMS, &count);

		int size;
		unsigned int type;
		const int bufSize = 28;
		char name[bufSize];
		int nameLength;

		for (int i = 0; i < count; i++)
		{
			glGetActiveUniform(shader->id, (GLuint)i, bufSize, &nameLength, &size, &type, name);
			shader->uniforms.push_back(uniform(name, shader->id));
		}
	}
	
	void SetUniforms(CommonProgram* program, RenderSetup)
	
	*/


	void LoadShader(CommonProgram* program, const char* vertexSource, const char* fragmentSource, const char* geometrySource = nullptr);
	void LoadShader(QuadBatchProgram* program, const char* vertexFile, const char* fragmentFile, const char* geometryFile = nullptr);
	void LoadShader(WaterProgram* program, const char* vertexFile, const char* fragmentFile, const char* geometryFile = nullptr);
	void LoadShader(CubemapProgram* program, const char* vertexFile, const char* fragmentFile, const char* geometryFile = nullptr);
	void CheckCompileErrors(unsigned int object, ERROR_TYPE type);
	void DeleteShader(CommonProgram* program);
}

#endif