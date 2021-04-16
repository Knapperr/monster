#ifndef MON_SHADER_H
#define MON_SHADER_H

struct Shader
{
	int id;
};


enum class ERROR_TYPE
{
	VERTEX,
	FRAGMENT,
	GEOMETRY,
	PROGRAM
};

void LoadShader(Shader* shader, const char* vertexSource, const char* fragmentSource, const char* geometrySource = nullptr);
void CheckCompileErrors(unsigned int object, ERROR_TYPE type);
void DeleteShader(Shader *shader);

#endif