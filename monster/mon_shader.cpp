#include "mon_shader.h"

#include <glad/glad.h>
#include <iostream>
#include <sstream>
#include <fstream>

namespace MonGL
{
	void LoadUniforms(CommonProgram* program)
	{
		int programID = program->handle;

		// NOTE(ck): should be renderer->getAttribLocation something like that
		program->model = glGetUniformLocation(programID, "model");
		program->view = glGetUniformLocation(programID, "view");
		program->projection = glGetUniformLocation(programID, "projection");
		
		program->viewPos = glGetUniformLocation(programID, "viewPos");
		program->lightPos = glGetUniformLocation(programID, "light.pos");
		program->lightAmbient = glGetUniformLocation(programID, "light.ambient");   
		program->lightDiffuse = glGetUniformLocation(programID, "light.diffuse");   
		program->lightSpecular = glGetUniformLocation(programID, "light.specular"); 

		program->colliderColor = glGetUniformLocation(programID, "colliderColor");
		program->textureDiffuse1 = glGetUniformLocation(programID, "texture_diffuse1");
		program->useTexture = glGetUniformLocation(programID, "useTexture");
		program->collider = glGetUniformLocation(programID, "collider");
		program->pixelTexture = glGetUniformLocation(programID, "pixelTexture");

		program->texCoordScale = glGetUniformLocation(programID, "texCoordScale");
		/*
		uniform Material material;
		uniform Light light;
		*/
	}

	void LoadUniforms(WaterProgram* program)
	{
		int programID = program->common.handle;
		program->time = glGetUniformLocation(programID, "time");
		program->waveLength	= glGetUniformLocation(programID, "waveLength");
		program->uJump = glGetUniformLocation(programID, "uJump");
		program->vJump = glGetUniformLocation(programID, "vJump");
		program->tiling	= glGetUniformLocation(programID, "tiling");
		program->speed	= glGetUniformLocation(programID, "speed");
		program->flowStrength = glGetUniformLocation(programID, "flowStrength");
		program->flowOffset = glGetUniformLocation(programID, "flowOffset");
		program->heightScale = glGetUniformLocation(programID, "heightScale");
		program->heightScaleModulated = glGetUniformLocation(programID, "heightScaleModulated");

		program->textureNormal1 = glGetUniformLocation(programID, "textureNormal1");
		program->textureNormal2 = glGetUniformLocation(programID, "textureNormal2");
	}

	void LoadUniforms(CubemapProgram* program)
	{
		int programID = program->common.handle;
		program->skybox = glGetUniformLocation(programID, "skybox");
		program->skyboxTextureIndex = 23;
	}


	unsigned int CompileShader(const char* source, GLenum shaderType)
	{
		// TODO(ck): Remove the ERROR_TYPE and use the shaderType to pass?
		unsigned int result = glCreateShader(shaderType);
		glShaderSource(result, 1, &source, NULL);
		glCompileShader(result);
		CheckCompileErrors(result, ERROR_TYPE::VERTEX);
		return result;
	}

	void Compile(CommonProgram* program, const char* vertexSource, const char* fragmentSource, const char* geometrySource)
	{
		unsigned int sVertex, sFragment, sGeometry;

		sVertex = CompileShader(vertexSource, GL_VERTEX_SHADER);
		sFragment = CompileShader(fragmentSource, GL_FRAGMENT_SHADER);
		if (geometrySource != nullptr)
			sGeometry = CompileShader(geometrySource, GL_GEOMETRY_SHADER);

		// Shader program
		program->handle = glCreateProgram();
		glAttachShader(program->handle, sVertex);
		glAttachShader(program->handle, sFragment);

		if (geometrySource != nullptr)
		{
			glAttachShader(program->handle, sGeometry);
		}

		glLinkProgram(program->handle);
		CheckCompileErrors(program->handle, ERROR_TYPE::PROGRAM);

		glDeleteShader(sVertex);
		glDeleteShader(sFragment);
		if (geometrySource != nullptr)
		{
			glDeleteShader(sGeometry);
		}
	}

	void LoadShader(CommonProgram* program, const char* vertexFile, const char* fragmentFile, const char* geometryFile)
	{
		std::string vertexCode;
		std::string fragmentCode;
		std::string geometryCode;

		try
		{
			std::ifstream vertexShaderFile(vertexFile);
			std::ifstream fragmentShaderFile(fragmentFile);
			std::stringstream vShaderStream;
			std::stringstream fShaderStream;

			// Read file's buffer contents into streams
			vShaderStream << vertexShaderFile.rdbuf();
			fShaderStream << fragmentShaderFile.rdbuf();

			vertexShaderFile.close();
			fragmentShaderFile.close();

			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();

			if (geometryFile != nullptr)
			{
				std::ifstream geometryShaderFile(geometryFile);
				std::stringstream gShaderStream;
				gShaderStream << geometryShaderFile.rdbuf();
				geometryShaderFile.close();
				geometryCode = gShaderStream.str();

			}
		}
		catch (std::exception e)
		{
			std::cout << "ERROR::SHADER: Failed to read the shader files\n";
		}

		const char* vShaderCode = vertexCode.c_str();
		const char* fShaderCode = fragmentCode.c_str();
		const char* gShaderCode = geometryCode.c_str();

		Compile(program, vShaderCode, fShaderCode, geometryFile != nullptr ? gShaderCode : nullptr);
		LoadUniforms(program);
	}

	void LoadShader(QuadBatchProgram* program, const char* vertexFile, const char* fragmentFile, const char* geometryFile)
	{
		LoadShader(&program->common, vertexFile, fragmentFile, geometryFile);
	}

	void LoadShader(WaterProgram* program, const char* vertexFile, const char* fragmentFile, const char* geometryFile)
	{
		LoadShader(&program->common, vertexFile, fragmentFile, geometryFile);
		LoadUniforms(program);
	}

	void LoadShader(CubemapProgram* program, const char* vertexFile, const char* fragmentFile, const char* geometryFile)
	{
		LoadShader(&program->common, vertexFile, fragmentFile, geometryFile);
		LoadUniforms(program);

		program->skyboxTextureIndex = 23;
	}
	
	void CheckCompileErrors(unsigned int object, ERROR_TYPE type)
	{
		const int LOG_SIZE = 1024;
		char infoLog[LOG_SIZE];

		int success;
		if (type != ERROR_TYPE::PROGRAM)
		{
			glGetShaderiv(object, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(object, LOG_SIZE, NULL, infoLog);
				std::cout << " | ERROR::SHADER: Compile time error: Type: " << "\n"
					<< infoLog << "\n --------------------------------------------------- \n";
			}
		}
		else
		{
			glGetProgramiv(object, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(object, LOG_SIZE, NULL, infoLog);
				std::cout << "| ERROR::Shader: Linker error: Type: " << "\n"
					<< infoLog << "\n------------------------------------------------\n";
			}
		}
	}

	void DeleteShader(CommonProgram* program)
	{
		glDeleteProgram(program->handle);
	}
}