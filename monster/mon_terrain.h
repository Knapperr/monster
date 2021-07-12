#ifndef MON_TERRAIN_H
#define MON_TERRAIN_H

#include "mon_shader.h"
#include "mon_gl_render.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Terrain
{
public:
	float x;
	float z;

	int selectedTextureId;
	bool wireFrame;
	bool drawTexture;

	//Grass grass;
	MonShader::Shader shader;
	MonGL::RenderData mesh;
	// Shader* grassShader;
	int* textureIds;

	Terrain(int gridX, int gridZ);
	~Terrain();

	//float barryCentric(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec2 pos);
	//float getHeight(int x, int z);
	//float lookUpHeight(int x, int z);
	//void generate();
	//void GenerateGrass();
};


#endif