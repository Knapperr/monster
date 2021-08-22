#include "mon_shader.h"

#include <glad/glad.h>
#include <iostream>
#include <sstream>
#include <fstream>

namespace MonGL
{

	unsigned int CompileShader(const char* source, GLenum shaderType)
	{
		// TODO(ck): Remove the ERROR_TYPE and use the shaderType to pass?
		unsigned int result = glCreateShader(shaderType);
		glShaderSource(result, 1, &source, NULL);
		glCompileShader(result);
		CheckCompileErrors(result, ERROR_TYPE::VERTEX);
		return result;
	}

	void Compile(Shader* shader, const char* vertexSource, const char* fragmentSource, const char* geometrySource)
	{
		unsigned int sVertex, sFragment, sGeometry;

		sVertex = CompileShader(vertexSource, GL_VERTEX_SHADER);
		sFragment = CompileShader(fragmentSource, GL_FRAGMENT_SHADER);
		if (geometrySource != nullptr)
			sGeometry = CompileShader(geometrySource, GL_GEOMETRY_SHADER);

		// Shader program
		shader->id = glCreateProgram();
		glAttachShader(shader->id, sVertex);
		glAttachShader(shader->id, sFragment);

		if (geometrySource != nullptr)
		{
			glAttachShader(shader->id, sGeometry);
		}

		glLinkProgram(shader->id);
		CheckCompileErrors(shader->id, ERROR_TYPE::PROGRAM);

		glDeleteShader(sVertex);
		glDeleteShader(sFragment);
		if (geometrySource != nullptr)
		{
			glDeleteShader(sGeometry);
		}
	}

	void LoadShader(Shader* shader, const char* vertexFile, const char* fragmentFile, const char* geometryFile)
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

		Compile(shader, vShaderCode, fShaderCode, geometryFile != nullptr ? gShaderCode : nullptr);
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

	void DeleteShader(Shader* shader)
	{
		glDeleteProgram(shader->id);
	}
}