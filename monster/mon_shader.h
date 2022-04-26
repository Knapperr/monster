#ifndef MON_SHADER_H
#define MON_SHADER_H

namespace MonGL
{
	struct CommonProgram
	{
		int handle;
		// TODO(ck): these are just the ids of the common shader program
		
		/*
			viewPos
			model
			useTexture
			pixelTexture
			colliderColor
			collider

			// textures in the program
			u32 SamplerCount;
			GLuint Samplers[16];
		*/
	
	};

	// TODO(ck): Match struct in the shader also make a 
	struct WaterProgram
	{
		CommonProgram common;

		// TODO(ck): uniform ids for shader
		int tiling;
		int speed;
		int flowStrength;
		int flowOffset;
		int heightScale;
		int heightScaleModulated;
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
	void CheckCompileErrors(unsigned int object, ERROR_TYPE type);
	void DeleteShader(CommonProgram* program);
}

#endif