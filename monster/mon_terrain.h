#ifndef MON_TERRAIN_H
#define MON_TERRAIN_H

#include "mon_shader.h"
#include "mon_gl_render.h"

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

	//float barryCentric(v3 p1, v3 p2, v3 p3, v2 pos);
	//float getHeight(int x, int z);
	//float lookUpHeight(int x, int z);
	//void generate();
	//void GenerateGrass();
};


#endif