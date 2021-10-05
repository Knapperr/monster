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
	struct WaterDataProgram
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

	void LoadShader(CommonProgram* program, const char* vertexSource, const char* fragmentSource, const char* geometrySource = nullptr);
	void CheckCompileErrors(unsigned int object, ERROR_TYPE type);
	void DeleteShader(CommonProgram* program);
}

#endif