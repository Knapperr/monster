#include "mon_gl_render.h"

#include <glad/glad.h>

namespace MonGL
{
	// TODO(ck): TODO(ck): Somehow put into game so we can call from gui
	// can have a getter method that retrieves the globalDrawCalls from here
	int globalDrawCalls = 0;

	void LoadTexture(RenderData* data, int index, TextureType type, int shaderID, std::string path, bool pixelTexture)
	{
		data->texturePath = path;
		Texture text = {};
		LoadTextureFile(&text, path.c_str(), type, false, true, true, pixelTexture);
		data->textures[index] = text;
		data->selectedTexture = 0;
		glUniform1i(glGetUniformLocation(shaderID, "texture_diffuse1"), 0);
	}

	void LoadTexture(Texture* texture, TextureType type, int shaderID, std::string path, bool pixelTexture)
	{
		LoadTextureFile(texture, path.c_str(), type, false, true, true, pixelTexture);
		glUniform1i(glGetUniformLocation(shaderID, "texture_diffuse1"), 0);
	}

#include <fstream>
#include <string>
	void LoadImpFile(Mesh* mesh, const char* fileName)
	{
		std::ifstream file(fileName);
		file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		int index;
		try
		{
			if (!file.is_open())
			{
				Mon::Log::print("Failure to open file");
			}

			std::string line;
			while (file >> line)
			{
				// TODO(ck): Remove name from .imp file shouldn't have a name
				// its just triangles that the entity is quickly loading
				std::string nameLine = line;
				file >> mesh->verticeCount;
				file >> mesh->indiceCount;
				break;
			}

			mesh->vertices = new MonGL::Vertex3D[mesh->verticeCount];
			mesh->indices = new unsigned int[mesh->indiceCount];

			while (file >> line)
			{
				for (int i = 0; i < mesh->verticeCount; ++i)
				{
					index = i;

					if (i == 0)
						mesh->vertices[i].position.x = std::stof(line);
					else
						file >> mesh->vertices[i].position.x;

					file >> mesh->vertices[i].position.y;
					file >> mesh->vertices[i].position.z;
					file >> mesh->vertices[i].normal.x;
					file >> mesh->vertices[i].normal.y;
					file >> mesh->vertices[i].normal.z;
					file >> mesh->vertices[i].texCoords.x;
					file >> mesh->vertices[i].texCoords.y;
					file >> mesh->vertices[i].tangent.x;
					file >> mesh->vertices[i].tangent.y;
					file >> mesh->vertices[i].tangent.z;
					file >> mesh->vertices[i].bitangent.x;
					file >> mesh->vertices[i].bitangent.y;
					file >> mesh->vertices[i].bitangent.z;
				}

				for (int j = 0; j < mesh->indiceCount; ++j)
				{
					index = j;
					file >> mesh->indices[j];
				}

				// finished
				break;
			}
		}
		catch (std::ifstream::failure& ex)
		{
			Mon::Log::print("File read failed");
			Mon::Log::print(ex.what());
			int x = index;
		}

		file.close();
	}

	void UploadOpenGLMesh(Mesh* mesh)
	{
		glGenVertexArrays(1, &mesh->VAO);
		glGenBuffers(1, &mesh->VBO);

		glBindVertexArray(mesh->VAO);
		glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
		glBufferData(GL_ARRAY_BUFFER, mesh->verticeCount * sizeof(Vertex3D), mesh->vertices, GL_STATIC_DRAW);

		if (mesh->indiceCount > 0)
		{
			glGenBuffers(1, &mesh->IBO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->IBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indiceCount * sizeof(unsigned int), mesh->indices, GL_STATIC_DRAW);
		}

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, normal));

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, texCoords));

		if (mesh->type == RenderType::Model)
		{
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, tangent));

			glEnableVertexAttribArray(4);
			glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, bitangent));
		}

#if 0
		if (tangents)
		{
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, tangent));

			glEnableVertexAttribArray(4);
			glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, bitangent));
		}
#endif

		// unbind
		glBindVertexArray(0);
	}

	///
	/// [BEGIN] Renderer
	///	
	/*
		This needs to be able to init DirectX and other platforms
		Eventually I will add a layer above opengl
	*/
	void InitRenderer(OpenGL* gl)
	{
		/*
		TODO(ck): Load from a configuration file
		should only load meshses in current chunk or level
		*/

		// Data that renderdata can hold
		//Mesh meshes[10]; // TODO(ck): SQLite config for size
		//Texture textures[30]; // TODO(ck): SQLite config for size

		// TODO(ck): Wrangle up the data
		// go around the code base and find out where these are all loaded and load them in here
		// everything must have an index into these things
		gl->program = {};
		gl->waterProgram = {};
		MonGL::LoadShader(&gl->program, "res/shaders/vert_colors.glsl", "res/shaders/frag_colors.glsl", NULL);
		MonGL::LoadShader(&gl->waterProgram.common, "res/shaders/vert_water.glsl", "res/shaders/frag_water.glsl", NULL);
		// TODO(ck): Do not need this now?
		//state->mainShaderID = state->shader.handle;

		/*
		// might need dynamic mesh for animations 
		// if a vao needs to change it has to be updated
		// and these are going to stay the same

		// quad mesh - only need one because everything is using it
		we can batch later 
		*/
		//empty mesh #0
		AddMesh(gl);

		// quad mesh #1
		AddMesh(gl);
		Mesh* quadMesh = GetMesh(gl, 1);
		MonGL::InitQuad(quadMesh);

		//cube mesh #2
		AddMesh(gl);
		Mesh* cubeMesh = GetMesh(gl, 2);
		MonGL::InitCube(cubeMesh);
		
		//grass mesh #3 
		AddMesh(gl);
		Mesh* grassMesh = GetMesh(gl, 3);
		InitModel(grassMesh, "test_grass.imp");

		// This is for the terrain
		//grid mesh #4 ??
		AddMesh(gl);
		Mesh* gridMesh = GetMesh(gl, 4);
		InitGrid(gridMesh, 64, 64);

		// debug meshes
		// collider mesh
		AddMesh(gl);
		Mesh* collider = GetMesh(gl, 5);
		InitBoundingBoxMesh(collider);

		// line mesh
		AddMesh(gl);
		Mesh* line = GetMesh(gl, 6);
		InitLineMesh(line);

		// empty #0 for texture
		AddTexture(gl);
		
		//
		// TODO(ck): Need a texture atlas rather than loading all of these
		// textures for the entities
		//

		AddTexture(gl);
		Texture* t1 = GetTexture(gl, 1);
		AddTexture(gl);
		Texture* t2 = GetTexture(gl, 2);
		AddTexture(gl);
		Texture* t3 = GetTexture(gl, 3);
		AddTexture(gl);
		Texture* t4 = GetTexture(gl, 4);
		LoadTexture(t1, MonGL::TextureType::Diffuse, gl->program.handle, "res/textures/ch_witch.png");
		LoadTexture(t2, MonGL::TextureType::Diffuse, gl->program.handle, "res/textures/p1.png");
		LoadTexture(t3, MonGL::TextureType::Diffuse, gl->program.handle, "res/textures/p1SIDE.png");
		LoadTexture(t4, MonGL::TextureType::Diffuse, gl->program.handle, "res/textures/p1BACK.png");

		AddTexture(gl);
		Texture* t5 = GetTexture(gl, 5);
		LoadTexture(t5, MonGL::TextureType::Diffuse, gl->program.handle, "res/textures/water/ripples-derivative-height.png");
		AddTexture(gl);
		Texture* t6 = GetTexture(gl, 6);
		LoadTexture(t6, MonGL::TextureType::Normal, gl->program.handle, "res/textures/water/flow-speed-noise.png");
		
		// more textures 
	
		// minion

		// tree
		
		// flower

		// terrain grid textures


	}

 
	///
	/// [END] 
	///	

	///
	/// [BEGIN] Shader Programs
	/// 


	void UseProgram(CommonProgram* program, RenderSetup setup)
	{
		glUseProgram(program->handle);

		//bool useTexture = (ArrayCount(data->textures) > 0);
		//glUniform1i(glGetUniformLocation(shaderID, "useTexture"), useTexture);
		//glUniform1i(glGetUniformLocation(shaderID, "pixelTexture"), useTexture);
		//// bind textures on corresponding texture units
		////glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, data->textures[selectedTexture].id);

		////glUniform3fv(glGetUniformLocation(shaderID, "colliderColor"), 1, &data->color[0]);
		//glUniform1i(glGetUniformLocation(shaderID, "collider"), false);

		//OpenGL->glUseProgram(Prog->ProgHandle);
		// glUseProgram(program->handle);

		//UseProgramBegin(OpenGL, &Prog->Common);

		//OpenGL->glUniformMatrix4fv(Prog->TransformID, 1, GL_TRUE, Setup->Proj.E[0]);
		//OpenGL->glUniform3fv(Prog->CameraP, 1, Setup->CameraP.E);
		//OpenGL->glUniform3fv(Prog->FogDirection, 1, Setup->FogDirection.E);
		//OpenGL->glUniform3fv(Prog->FogColor, 1, Setup->FogColor.E);
		//OpenGL->glUniform1f(Prog->FogStartDistance, Setup->FogStartDistance);
		//OpenGL->glUniform1f(Prog->FogEndDistance, Setup->FogEndDistance);
		//OpenGL->glUniform1f(Prog->ClipAlphaStartDistance, Setup->ClipAlphaStartDistance);
		//OpenGL->glUniform1f(Prog->ClipAlphaEndDistance, Setup->ClipAlphaEndDistance);
		//OpenGL->glUniform1f(Prog->AlphaThreshold, AlphaThreshold);
		//OpenGL->glUniform3fv(Prog->VoxelMinCorner, 1, Commands->LightingVoxelMinCorner.E);
		//OpenGL->glUniform3fv(Prog->VoxelInvTotalDim, 1, Commands->LightingVoxelInvTotalDim.E);

	}

	void UseProgram(WaterProgram* program, RenderSetup setup)
	{
		UseProgram(program, setup);

		// TODO(ck):
		// somehow get entity's water component data into here
		// Once we solve this we will have a semi generic way of writing shaders
		/*
		need to call the glUniform1f(program->)

		maybe we can have a render setup that gets changed by each entity
		when it comes through
		
		*/
	
	}

	///
	/// [BEGIN] Init RenderData
	///

	void InitQuad(Mesh* mesh, bool tangents)
	{
		int verticeCount = 4;
		// TODO(ck): Memory Allocation
		mesh->vertices = new Vertex3D[verticeCount];
		mesh->vertices[0].position = v3(0.5f, 0.5f, 0.0f);
		mesh->vertices[0].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[0].texCoords = v2(1.0f, 1.0f);

		mesh->vertices[1].position = v3(0.5f, -0.5f, 0.0f);
		mesh->vertices[1].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[1].texCoords = v2(1.0f, 0.0f);

		mesh->vertices[2].position = v3(-0.5f, -0.5f, 0.0f);
		mesh->vertices[2].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[2].texCoords = v2(0.0f, 0.0f);

		mesh->vertices[3].position = v3(-0.5f, 0.5f, 0.0f);
		mesh->vertices[3].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[3].texCoords = v2(0.0f, 1.0f);

		// TODO(ck): split out
		if (tangents)
		{
			//data->vertices.clear();
			v3 pos1(0.5f, 0.5f, 0.0f);
			v3 pos2(0.5f, -0.5f, 0.0f);
			v3 pos3(-0.5f, -0.5f, 0.0f);
			v3 pos4(-0.5f, 0.5f, 0.0f);

			v2 uv1(1.0f, 1.0f);
			v2 uv2(1.0f, 0.0f);
			v2 uv3(0.0f, 0.0f);
			v2 uv4(0.0f, 1.0f);

			v3 normal(1.0f, 1.0f, 1.0f);

			v3 edge1 = pos2 - pos1;
			v3 edge2 = pos3 - pos1;
			v2 deltaUV1 = uv2 - uv1;
			v2 deltaUV2 = uv3 - uv1;

			float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

			//v3 tangent1, bitangent1;
			//v3 tangent2, bitangent2;


			f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
		}

		int indiceCount = 6;
		mesh->indices = new unsigned int[indiceCount];
		mesh->indices[0] = 0;
		mesh->indices[1] = 1;
		mesh->indices[2] = 3;
		mesh->indices[3] = 1;
		mesh->indices[4] = 2;
		mesh->indices[5] = 3;

		mesh->verticeCount = verticeCount;
		mesh->indiceCount = indiceCount;
		mesh->type = RenderType::Quad;
		UploadOpenGLMesh(mesh);
	}


	void InitBatchData(int amount)
	{
		// These will be figured out after looping our tilemap and pushing quads
		// TODO(ck): Need to be able to choose amount of vertices and indices
		int usedVertices = 1;
		const int quadCount = 2000;
		const int maxVertices = quadCount * 4;
		const int indicesLength = quadCount * 6;

		// TODO(ck): MEMORY 
		//batch = new BatchData();

		//glGenVertexArrays(1, &batch->VAO);
		//glBindVertexArray(batch->VAO);

		//glGenBuffers(1, &batch->VBO);
		//glBindBuffer(GL_ARRAY_BUFFER, batch->VBO);
		glBufferData(GL_ARRAY_BUFFER, maxVertices * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

		// position attribute
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
		// color attribute
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
		// texture coord attribute
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

		// TODO(ck): Memory management
		uint32_t* tileIndices = new uint32_t[indicesLength];
		int offset = 0;
		for (int i = 0; i < indicesLength; i += 6)
		{
			tileIndices[i + 0] = 0 + offset;
			tileIndices[i + 1] = 1 + offset;
			tileIndices[i + 2] = 2 + offset;

			tileIndices[i + 3] = 2 + offset;
			tileIndices[i + 4] = 3 + offset;
			tileIndices[i + 5] = 0 + offset;

			offset += 4;
		}

		unsigned int EBO;
		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesLength * sizeof(tileIndices), tileIndices, GL_DYNAMIC_DRAW);

		delete[] tileIndices;
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	};

	void InitBoundingBoxMesh(Mesh* mesh)
	{
		int verticeCount = 8;
		// TODO(ck): Memory Allocation
		mesh->vertices = new Vertex3D[verticeCount];
		mesh->vertices[0].position = v3(-0.5, -0.5, -0.5);
		mesh->vertices[0].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[0].texCoords = v2(0.0f, 0.0f);
		mesh->vertices[1].position = v3(0.5, -0.5, -0.5);
		mesh->vertices[1].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[1].texCoords = v2(0.0f, 0.0f);
		mesh->vertices[2].position = v3(0.5, 0.5, -0.5);
		mesh->vertices[2].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[2].texCoords = v2(0.0f, 0.0f);
		mesh->vertices[3].position = v3(-0.5, 0.5, -0.5);
		mesh->vertices[3].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[3].texCoords = v2(0.0f, 0.0f);

		mesh->vertices[4].position = v3(-0.5, -0.5, 0.5);
		mesh->vertices[4].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[4].texCoords = v2(0.0f, 0.0f);
		mesh->vertices[5].position = v3(0.5, -0.5, 0.5);
		mesh->vertices[5].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[5].texCoords = v2(0.0f, 0.0f);
		mesh->vertices[6].position = v3(0.5, 0.5, 0.5);
		mesh->vertices[6].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[6].texCoords = v2(0.0f, 0.0f);
		mesh->vertices[7].position = v3(-0.5, 0.5, 0.5);
		mesh->vertices[7].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[7].texCoords = v2(0.0f, 0.0f);

		GLushort elements[] = {
			0, 1, 2, 3, // front 
			4, 5, 6, 7, // back
			0, 4, 1, 5, 2, 6, 3, 7 // back
		};
		int indiceCount = sizeof(elements) / sizeof(elements[0]);
		mesh->indices = (unsigned int*)elements;

		// Set world matrix to be the same size as the bounding box
		//data->worldMatrix = GetTransform(&data->size);

		mesh->verticeCount = verticeCount;
		mesh->indiceCount = indiceCount;
		UploadOpenGLMesh(mesh);
	}

	void InitBoundingBox(RenderData* data)
	{
		data->lineWidth = 2;
		data->color = v3(0.7f, 0.15f, 0.4f);
		data->meshIndex = 5;
		data->visible = true;
	}


	void InitCube(Mesh* mesh)
	{
		// Load from .vt file (need to do efficient as possible)
		// maybe dont need to do this but?? tilemap does a quad and its a huge
		// cubes can just be created with a macro PUSH_CUBE PUSH_QUAD x4?
		int verticeCount = 36;
		// TODO(ck): Memory Allocation
		mesh->vertices = new Vertex3D[verticeCount];
		
		// TODO(ck): ADD QUAD method
		mesh->vertices[0].position = v3(-0.5f, -0.5f, -0.5f);
		mesh->vertices[0].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[0].texCoords = v2(0.0f, 0.0f);
		mesh->vertices[1].position = v3(0.5f, -0.5f, -0.5f);
		mesh->vertices[1].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[1].texCoords = v2(1.0f, 0.0f);
		mesh->vertices[2].position = v3(0.5f, 0.5f, -0.5f);
		mesh->vertices[2].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[2].texCoords = v2(1.0f, 1.0f);
		mesh->vertices[3].position = v3(0.5f, 0.5f, -0.5f);
		mesh->vertices[3].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[3].texCoords = v2(1.0f, 1.0f);
		mesh->vertices[4].position = v3(-0.5f, 0.5f, -0.5f);
		mesh->vertices[4].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[4].texCoords = v2(0.0f, 1.0f);
		mesh->vertices[5].position = v3(-0.5f, -0.5f, -0.5f);
		mesh->vertices[5].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[5].texCoords = v2(0.0f, 0.0f);
		  
		mesh->vertices[6].position = v3(-0.5f, -0.5f, 0.5f);
		mesh->vertices[6].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[6].texCoords = v2(0.0f, 0.0f);
		mesh->vertices[7].position = v3(0.5f, -0.5f, 0.5f);
		mesh->vertices[7].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[7].texCoords = v2(1.0f, 0.0f);
		mesh->vertices[8].position = v3(0.5f, 0.5f, 0.5f);
		mesh->vertices[8].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[8].texCoords = v2(1.0f, 1.0f);
		mesh->vertices[9].position = v3(0.5f, 0.5f, 0.5f);
		mesh->vertices[9].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[9].texCoords = v2(1.0f, 1.0f);
		mesh->vertices[10].position = v3(-0.5f, 0.5f, 0.5f);
		mesh->vertices[10].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[10].texCoords = v2(0.0f, 1.0f);
		mesh->vertices[11].position = v3(-0.5f, -0.5f, 0.5f);
		mesh->vertices[11].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[11].texCoords = v2(0.0f, 0.0f);
		
		mesh->vertices[12].position = v3(-0.5f, 0.5f, 0.5f);
		mesh->vertices[12].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[12].texCoords = v2(1.0f, 0.0f);
		mesh->vertices[13].position = v3(-0.5f, 0.5f, -0.5f);
		mesh->vertices[13].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[13].texCoords = v2(1.0f, 1.0f);
		mesh->vertices[14].position = v3(-0.5f, -0.5f, -0.5f);
		mesh->vertices[14].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[14].texCoords = v2(0.0f, 1.0f);
		mesh->vertices[15].position = v3(-0.5f, -0.5f, -0.5f);
		mesh->vertices[15].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[15].texCoords = v2(0.0f, 1.0f);
		mesh->vertices[16].position = v3(-0.5f, -0.5f, 0.5f);
		mesh->vertices[16].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[16].texCoords = v2(0.0f, 0.0f);
		mesh->vertices[17].position = v3(-0.5f, 0.5f, 0.5f);
		mesh->vertices[17].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[17].texCoords = v2(1.0f, 0.0f);

		mesh->vertices[18].position = v3(0.5f, 0.5f, 0.5f);
		mesh->vertices[18].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[18].texCoords = v2(1.0f, 0.0f);
		mesh->vertices[19].position = v3(0.5f, 0.5f, -0.5f);
		mesh->vertices[19].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[19].texCoords = v2(1.0f, 1.0f);
		mesh->vertices[20].position = v3(0.5f, -0.5f, -0.5f);
		mesh->vertices[20].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[20].texCoords = v2(0.0f, 1.0f);
		mesh->vertices[21].position = v3(0.5f, -0.5f, -0.5f);
		mesh->vertices[21].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[21].texCoords = v2(0.0f, 1.0f);
		mesh->vertices[22].position = v3(0.5f, -0.5f, 0.5f);
		mesh->vertices[22].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[22].texCoords = v2(0.0f, 0.0f);
		mesh->vertices[23].position = v3(0.5f, 0.5f, 0.5f);
		mesh->vertices[23].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[23].texCoords = v2(1.0f, 0.0f);

		mesh->vertices[24].position = v3(-0.5f, -0.5f, -0.5f);
		mesh->vertices[24].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[24].texCoords = v2(0.0f, 1.0f);
		mesh->vertices[25].position = v3(0.5f, -0.5f, -0.5f);
		mesh->vertices[25].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[25].texCoords = v2(1.0f, 1.0f);
		mesh->vertices[26].position = v3(0.5f, -0.5f, 0.5f);
		mesh->vertices[26].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[26].texCoords = v2(1.0f, 0.0f);
		mesh->vertices[27].position = v3(0.5f, -0.5f, 0.5f);
		mesh->vertices[27].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[27].texCoords = v2(1.0f, 0.0f);
		mesh->vertices[28].position = v3(-0.5f, -0.5f, 0.5f);
		mesh->vertices[28].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[28].texCoords = v2(0.0f, 0.0f);
		mesh->vertices[29].position = v3(-0.5f, -0.5f, -0.5f);
		mesh->vertices[29].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[29].texCoords = v2(0.0f, 1.0f);

		mesh->vertices[30].position = v3(-0.5f, 0.5f, -0.5f);
		mesh->vertices[30].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[30].texCoords = v2(0.0f, 1.0f);
		mesh->vertices[31].position = v3(0.5f, 0.5f, -0.5f);
		mesh->vertices[31].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[31].texCoords = v2(1.0f, 1.0f);
		mesh->vertices[32].position = v3(0.5f, 0.5f, 0.5f);
		mesh->vertices[32].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[32].texCoords = v2(1.0f, 0.0f);
		mesh->vertices[33].position = v3(0.5f, 0.5f, 0.5f);
		mesh->vertices[33].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[33].texCoords = v2(1.0f, 0.0f);
		mesh->vertices[34].position = v3(-0.5f, 0.5f, 0.5f);
		mesh->vertices[34].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[34].texCoords = v2(0.0f, 0.0f);
		mesh->vertices[35].position = v3(-0.5f, 0.5f, -0.5f);
		mesh->vertices[35].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[35].texCoords = v2(0.0f, 1.0f);


		mesh->type = RenderType::Cube;
		mesh->verticeCount = verticeCount;
		mesh->indiceCount = 0;
		UploadOpenGLMesh(mesh);
	}

	void InitCube(RenderData* data)
	{
		data->visible = true;
		data->scale = v3(1.0f);
		data->meshIndex = 2;
	}

	void InitModel(Mesh* mesh, const char* fileName)
	{
		LoadImpFile(mesh, fileName);
		mesh->type = RenderType::Model;
		UploadOpenGLMesh(mesh);
	}

	// TODO(ck): can remove this?
	// NOTE(ck): Assume the vertices and indices have been loaded
	void InitModel(RenderData* data)
	{
		data->scale = v3(1.0f);
		data->visible = true;
	}

	void InitInstancedData(InstancedData* data, int amount)
	{
		data->amount = amount;
		InitModel(&data->renderData);
		data->matrices = new mat4[data->amount];
	}

	void InitGrid(Mesh* mesh, int xSize, int zSize)
	{
		// TODO(ck): 
/*
	Keep this as basic grid but make a new grid that is batched?
	can create the Grid struct and then have a batcher that it uploads
	itself too. that way we can start creating terrain?

	each cell is about the same size as 16 pixel texture wide... is there a way to define
	this in our vertices?

	can try and use ArrayTextures for this as well so there is no texture bleeding???
	we can get that DS game look using this


*/
		int verticeCount = (xSize + 1) * (zSize + 1);
		mesh->vertices = new Vertex3D[verticeCount];
		for (int index = 0, z = 0; z <= zSize; z++)
		{
			for (int x = 0; x <= xSize; x++, index++)
			{
				mesh->vertices[index] = {};
				mesh->vertices[index].position.x = x;
				mesh->vertices[index].position.y = -0.5f;
				mesh->vertices[index].position.z = z;

				mesh->vertices[index].normal.x = 0;
				mesh->vertices[index].normal.y = 1;
				mesh->vertices[index].normal.z = 0;

				mesh->vertices[index].texCoords.x = (float)x / (float)xSize;
				mesh->vertices[index].texCoords.y = (float)z / (float)zSize;
			}
		}

		/*
		*   NOTE(ck):
			build our grid using this method?

			for (int index = 0; i < sizeof(Grid); ++i)
			{
				data->vertices[index].position.x = grid[index].x;
				data->vertices[index].position.y = -0.5;
				data->vertices[index].position.z = grid[index].z;

			}


			NOTE(ck): This is from RPG Paper Maker they use a square size
			void Grid::initializeVertices(int w, int h, int squareSize) {
				m_vertices.clear();

				float w_f = (float)w, h_f = (float)h, squareSize_f = (float)squareSize;

				for (int i = 0; i <= w; i++){
					m_vertices.push_back(QVector3D((i * squareSize_f), 0.0f, 0.0f));
					m_vertices.push_back(QVector3D((i * squareSize_f), 0.0f,
												   squareSize_f * h_f));
				}
				for (int i = 0; i <= h; i++){
					m_vertices.push_back(QVector3D(0.0f, 0.0f,(i*squareSize_f)));
					m_vertices.push_back(QVector3D(squareSize_f * w_f, 0.0f,
												   (i * squareSize_f)));
				}
			}

			NOTE(ck): looks like they are adding 0.5 to the world position just like i am in the verts
			I wonder why they are doing the grid size like that? why does mine work?

			void Grid::paintGL(QMatrix4x4& modelviewProjection, int y) {
				m_program->bind();
				m_program->setUniformValue(u_modelviewProjection, modelviewProjection);
				m_program->setUniformValue(u_yPosition, y + 0.5f);
				{
				  m_vao.bind();
				  glDrawArrays(GL_LINES, 0, m_vertices.size());
				  m_vao.release();
				}
				m_program->release();
			}


		*/

		int indiceCount = xSize * zSize * 6;
		mesh->indices = new unsigned int[indiceCount];
		for (int ti = 0, vi = 0, z = 0; z < zSize; z++, vi++)
		{
			for (int x = 0; x < xSize; x++, ti += 6, vi++)
			{
				mesh->indices[ti] = vi;
				mesh->indices[ti + 3] = mesh->indices[ti + 2] = vi + 1;
				mesh->indices[ti + 4] = mesh->indices[ti + 1] = vi + xSize + 1;
				mesh->indices[ti + 5] = vi + xSize + 2;
			}
		}

		mesh->verticeCount = verticeCount;
		mesh->indiceCount = indiceCount;
		UploadOpenGLMesh(mesh);

		//data->mat = {};
		//data->mat.ambient = v3(1.0f, 0.5f, 0.6f);
		//data->mat.diffuse = v3(1.0f, 0.5f, 0.31f);
		//data->mat.specular = v3(0.5f, 0.5f, 0.5f);
		//data->mat.shininess = 32.0f;
		
		
		// Set texture indexes
		//std::string textPath = "res/textures/terrain/1024multi.png";
		//Texture text = {};
		//data->textures[0] = text;
		//LoadTextureFile(&data->textures[0], textPath.c_str(), TextureType::Diffuse, false, false, true, false);

		//textPath = "res/textures/terrain/grass.jpg";
		//Texture text1 = {};
		//data->textures[1] = text1;
		//LoadTextureFile(&data->textures[1], textPath.c_str(), TextureType::Diffuse, false, false, true, false);

		//textPath = "res/textures/terrain/pix_grass.png";
		//Texture text2 = {};
		//data->textures[2] = text2;
		//LoadTextureFile(&data->textures[2], textPath.c_str(), TextureType::Diffuse, false, false, true, true);

		//textPath = "res/textures/terrain/snow.jpg";
		//Texture text3 = {};
		//data->textures[3] = text3;
		//LoadTextureFile(&data->textures[3], textPath.c_str(), TextureType::Diffuse, false, false, true, false);

	}

	/// 
	/// [END] INIT 
	/// 

	/// 
	/// [BEGIN] begin render 
	/// 

	void BeginRender(Config* config, mat4 projection, mat4 view, int shaderID)
	{
		MonGL::ViewPort(&config->viewPort);
		glUniformMatrix4fv(glGetUniformLocation(shaderID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(shaderID, "view"), 1, GL_FALSE, glm::value_ptr(view));

		globalDrawCalls = 0;
	}

	///
	/// [END] BEGIN RENDER
	/// 

	///
	/// [BEGIN] Debug Drawing
	///

	void InitLineMesh(Mesh* mesh)
	{
		int verticeCount = 2;
		mesh->vertices = new Vertex3D[verticeCount];

		mesh->vertices[0].position = v3(0.0f, 0.0f, 1.0f);
		mesh->vertices[0].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[0].texCoords = v2(1.0f, 1.0f);

		mesh->vertices[1].position = v3(0.0f, 0.0f, 50.0f);
		mesh->vertices[1].normal = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[1].texCoords = v2(1.0f, 1.0f);

		mesh->verticeCount = verticeCount;
		mesh->indiceCount = 0;
		mesh->type = RenderType::Debug;
		UploadOpenGLMesh(mesh);
	}

	void InitLine(Line* line)
	{
		// TODO(ck): Memory Allocation
		line->data.meshIndex = 6;
		line->data.color = v3(0.1f, 0.5f, 1.0f);
		line->data.visible = true;
	}

	void DrawLine(OpenGL* gl, Line* line)
	{
		Mesh* mesh = GetMesh(gl, line->data.meshIndex);
		unsigned int shaderID = gl->program.handle;


		line->data.worldMatrix = mat4(1.0f);
		line->data.worldMatrix = glm::translate(line->data.worldMatrix, line->pos);

		glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, glm::value_ptr(line->data.worldMatrix));

		// Use for now
		glUniform3fv(glGetUniformLocation(shaderID, "colliderColor"), 1, &line->data.color[0]);
		glUniform1i(glGetUniformLocation(shaderID, "useTexture"), false);
		glUniform1i(glGetUniformLocation(shaderID, "pixelTexture"), false);
		glUniform1i(glGetUniformLocation(shaderID, "collider"), true);

		//glBindVertexArray(mesh->VAO);
		glDrawArrays(GL_LINES, 0, 2);

		globalDrawCalls++;
	}


	void DrawBoundingBox(OpenGL* gl, RenderData* data, Camera* camera)
	{
		if (false == data->visible)
			return;

		Mesh* mesh = GetMesh(gl, data->meshIndex);
		unsigned int shaderID = gl->program.handle;

		glUniform3fv(glGetUniformLocation(shaderID, "viewPos"), 1, &camera->pos[0]);

		glUniformMatrix4fv(glGetUniformLocation(shaderID, "projection"), 1, GL_FALSE, glm::value_ptr(Projection(camera)));
		glUniformMatrix4fv(glGetUniformLocation(shaderID, "view"), 1, GL_FALSE, glm::value_ptr(ViewMatrix(camera)));

		glUniform1i(glGetUniformLocation(shaderID, "useTexture"), false);
		glUniform3fv(glGetUniformLocation(shaderID, "colliderColor"), 1, &data->color[0]);
		glUniform1i(glGetUniformLocation(shaderID, "collider"), true);

		glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, glm::value_ptr(data->worldMatrix));
		glBindVertexArray(mesh->VAO);

		glEnable(GL_LINE_SMOOTH);
		glLineWidth((float)data->lineWidth);
		glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, 0);

		glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, (GLvoid*)(4 * sizeof(GLushort)));
		glDrawElements(GL_LINES, 8, GL_UNSIGNED_SHORT, (GLvoid*)(8 * sizeof(GLushort)));

		globalDrawCalls++;
	}

	///
	/// [END] Debug Drawing 
	/// 

	///
	/// [BEGIN]  Draw Mesh with RenderData 
	///

	// TODO(ck): Maybe pass OpenGL to this then we have all the data?
	void Draw(OpenGL* gl, Config* config, float spriteAngleDegrees, RenderData* data, v3 pos, Camera* camera)
	{
		Mesh* mesh = GetMesh(gl, data->meshIndex);

		// which program to use is determined by renderData program type?
		// gl->program.handle

		bool useTexture = (ArrayCount(data->textures) > 0);
		useTexture = true;
		glUniform1i(glGetUniformLocation(gl->program.handle, "useTexture"), useTexture);
		glUniform1i(glGetUniformLocation(gl->program.handle, "pixelTexture"), useTexture);
		// bind textures on corresponding texture units
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, data->textures[data->selectedTexture].id);

		//glUniform3fv(glGetUniformLocation(shaderID, "colliderColor"), 1, &data->color[0]);
		glUniform1i(glGetUniformLocation(gl->program.handle, "collider"), false);

		// TODO(ck):
		// All uniforms get called in 

		// struct OpenGL
		// for holding our programs(shaders) 
		/*
		I have no idea the best way to do this...

		 RenderSetup is for things that are common between
		 all entities
		 these are things that are the same between all entities we
		 dont want them to all have one

		 commands are what the entities create i believe that are unique between
		 them when drawing. Still need to define when to use the water program
		 over the

			switch data->programType
				case 1:
					UseProgram(gl->common, setup)
				case 2:
					UseProgram(gl->water, setup, data->water) ?? something like this
				// i want it to be a material
				could have data->material[0]
		*/

		data->worldMatrix = mat4(1.0f);
		data->worldMatrix = glm::translate(data->worldMatrix, pos);
		if (mesh->indiceCount > 0)
		{
			data->worldMatrix = glm::rotate(data->worldMatrix, glm::radians(spriteAngleDegrees), v3{ 1.0f, 0.0f, 0.0f });
		}
		data->worldMatrix = glm::scale(data->worldMatrix, data->scale);
		glUniformMatrix4fv(glGetUniformLocation(gl->program.handle, "model"), 1, GL_FALSE, glm::value_ptr(data->worldMatrix));

		glBindVertexArray(mesh->VAO);
		if (mesh->indiceCount > 0)
		{
			glDrawElements(GL_TRIANGLES, mesh->indiceCount, GL_UNSIGNED_INT, 0);
		}
		else
		{
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		globalDrawCalls++;
	}

	void DrawWater(RenderData* data, RenderSetup* setup, WaterProgram* waterData, Light* light, v3 pos, v3 scale, v3 camPos, unsigned int shaderID)
	{

		// ==============================================================================
		//glUniform3fv(glGetUniformLocation(shaderID, "material.ambient"), 1, &data->mat.ambient[0]);
		//glUniform3fv(glGetUniformLocation(shaderID, "material.diffuse"), 1, &data->mat.diffuse[0]);
		//glUniform3fv(glGetUniformLocation(shaderID, "material.specular"), 1, &data->mat.specular[0]);
		//glUniform1f(glGetUniformLocation(shaderID, "material.shininess"), data->mat.shininess);

		// TODO(ck): Move to begin render. MonGL::beginRender(&cam);

		unsigned int diffuseNr = 1;
		unsigned int specularNr = 1;
		unsigned int normalNr = 1;
		unsigned int heightNr = 1;

		// Textures
		// ==================================================================================
		for (unsigned int i = 0; i < ArrayCount(data->textures); ++i)
		{
			glActiveTexture(GL_TEXTURE0 + i); // activate the proper texture unit before binding
		//	// retrieve texture number (the N in diffuse_TextureN)
			std::string number;
			std::string name = "texture_";
			switch (data->textures[i].type)
			{
			case TextureType::Diffuse:
				number = std::to_string(diffuseNr++);
				name += "diffuse";
				break;
			case TextureType::Specular:
				number = std::to_string(specularNr++);
				name += "specular";
				break;
			case TextureType::Normal:
				number = std::to_string(normalNr++);
				name += "normal";
				break;
			case TextureType::Height:
				number = std::to_string(heightNr++);
				name += "height";
			default:
				number = std::to_string(diffuseNr++);
				name += "diffuse";
				break;
			}

			// now set the sampler to the correct texture unit
			glUniform1i(glGetUniformLocation(shaderID, (name + number).c_str()), i);
			// and finally bind the texture		
			glBindTexture(GL_TEXTURE_2D, data->textures[i].id);
		}

		// ==================================================================================

		//glUniform1f(GetLoc(waterShader, "time"), (float)glfwGetTime());
		glUniform3fv(glGetUniformLocation(shaderID, "lightPos"), 1, &light->pos[0]);
		glUniform3fv(glGetUniformLocation(shaderID, "viewPos"), 1, &camPos[0]);

		glUniform1f(glGetUniformLocation(shaderID, "uJump"), 0.25f);	// water->uJump
		glUniform1f(glGetUniformLocation(shaderID, "vJump"), 0.25f);	// water->vJump
		glUniform1f(glGetUniformLocation(shaderID, "tiling"), (float)waterData->tiling);	// water->tiling
		glUniform1f(glGetUniformLocation(shaderID, "speed"), (float)waterData->speed);	// water->speed 
		glUniform1f(glGetUniformLocation(shaderID, "flowStrength"), (float)waterData->flowStrength); // water->flowStrength
		glUniform1f(glGetUniformLocation(shaderID, "flowOffset"), (float)waterData->flowOffset);	// water->flowOffset
		glUniform1f(glGetUniformLocation(shaderID, "heightScale"), (float)waterData->heightScale);	// water->heightScale
		glUniform1f(glGetUniformLocation(shaderID, "heightScaleModulated"), (float)waterData->heightScaleModulated); // water->heightScaleModulated
		//glUniform1f(glGetUniformLocation(shaderID, "waveLength"), waterData->waveLength);	//  water->waveLength

		// ==============================================================================

		// TODO(ck): Does this happen at the end of update. the data gets its mat4 updated 
		// and then we can just call glUniformMatrix on this
		mat4 model = mat4(1.0f);
		model = glm::translate(model, pos);
		//model = glm::rotate(model, glm::radians(config->angleDegrees), v3{ 1.0f, 0.0f, 0.0f });
		//model = glm::scale(model, scale);
		glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, glm::value_ptr(model));
		//glBindVertexArray(mesh.VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// Always good practice to set everything back to defaults once configured
		// NOTE(CK): bind texture must be AFTER glActiveTexture or it will not unbind properly
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void DrawTerrain(OpenGL* gl, RenderData* data, Light* light, Camera* camera, bool wireFrame)
	{
		mat4 matModel = mat4(1.0f);
		Mesh* mesh = GetMesh(gl, data->meshIndex);
		unsigned int shaderID = gl->program.handle;

		glUniformMatrix4fv(glGetUniformLocation(shaderID, "projection"), 1, GL_FALSE, glm::value_ptr(Projection(camera)));
		glUniformMatrix4fv(glGetUniformLocation(shaderID, "view"), 1, GL_FALSE, glm::value_ptr(ViewMatrix(camera)));

		glUniform3fv(glGetUniformLocation(shaderID, "light.pos"), 1, &light->pos[0]);
		glUniform3fv(glGetUniformLocation(shaderID, "light.ambient"), 1, &light->ambient[0]);
		glUniform3fv(glGetUniformLocation(shaderID, "light.diffuse"), 1, &light->diffuse[0]);
		glUniform3fv(glGetUniformLocation(shaderID, "light.specular"), 1, &light->specular[0]);

		//glUniform3fv(glGetUniformLocation(shaderID, "material.ambient"), 1, &data->mat.ambient[0]);
		//glUniform3fv(glGetUniformLocation(shaderID, "material.diffuse"), 1, &data->mat.diffuse[0]);
		//glUniform3fv(glGetUniformLocation(shaderID, "material.specular"), 1, &data->mat.specular[0]);
		//glUniform1f(glGetUniformLocation(shaderID, "material.shininess"), data->mat.shininess);

		glUniform3fv(glGetUniformLocation(shaderID, "viewPos"), 1, &camera->pos[0]);

		//mat4 matTranslate = glm::translate(mat4(1.0f), v3(terrain->x, 0.0f, terrain->z));
		//mat4 matTranslate = glm::translate(mat4(1.0f), v3(0.0f, 0.0f, 0.0f));
		//matModel = matModel * matTranslate;

		glUniform1i(glGetUniformLocation(shaderID, "useTexture"), true);
		glUniform1i(glGetUniformLocation(shaderID, "pixelTexture"), true);


		//glBindTexture(GL_TEXTURE_2D, terrain->selectedTextureId);
		glBindTexture(GL_TEXTURE_2D, data->textures[data->selectedTexture].id);

		glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, glm::value_ptr(matModel));
		glBindVertexArray(mesh->VAO);

		wireFrame ?
			glDrawElements(GL_LINES, mesh->indiceCount, GL_UNSIGNED_INT, 0)
			: glDrawElements(GL_TRIANGLES, mesh->indiceCount, GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);
		// Always good practice to set everything back to defaults once configured
		// NOTE(CK): bind texture must be AFTER glActiveTexture or it will not unbind properly
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);

		globalDrawCalls++;
	}

	///
	///  [END] Draw RenderData  
	///

	///
	///	[BEGIN] End Rendering
	///

	void EndRender()
	{
		assert(globalDrawCalls > 0);
	}

	///
	///	[END] End Rendering
	///

	// ==================================================================

	/// 
	/// [BEGIN] utility methods 
	/// 

	void ViewPort(Rect* port)
	{
		glViewport((int)port->x, (int)port->y, (int)port->w, (int)port->h);
		return;
	}

	///
	/// [END] utility methods
	/// 

	//void cleanUp(Sprite* sprite)
	//{
	//	glDeleteVertexArrays(1, &sprite->VAO);
	//	glDeleteBuffers(1, &sprite->VBO);
	//	//glDeleteBuffers(1, &EBO);
	//	//glDeleteProgram(shaderProgram);
	//}

			// TODO(ck): clean up render data 
	//glDeleteVertexArrays(1, &player.data.VAO);
	//glDeleteBuffers(1, &player.data.VBO);

	//MonGL::DeleteShader(&shader);



	///
	/// 2D
	///

	BatchData* batch;
	std::vector<Vertex> tileVertices;
	int usedIndices = 0;

	void InitOpenGLBatchMesh(BatchData* data)
	{
		int usedVertices = 1;
		const int quadCount = 2000;
		const int maxVertices = quadCount * 4;
		const int indicesLength = quadCount * 6;

		Mesh* mesh = &data->mesh;

		glGenVertexArrays(1, &mesh->VAO);
		glBindVertexArray(mesh->VAO);

		glGenBuffers(1, &mesh->VBO);
		glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
		glBufferData(GL_ARRAY_BUFFER, maxVertices * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

		glGenBuffers(1, &mesh->IBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesLength * sizeof(data->indices), data->indices, GL_DYNAMIC_DRAW);

		// position attribute
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
		// color attribute
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
		// texture coord attribute
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}


	void PushMatrix(BatchData* batch, mat4 matrix)
	{
		batch->matrixStack.push(batch->matrix);
		batch->matrix = matrix * batch->matrix;
		
	}

	void PopMatrix(BatchData* batch)
	{
		// mat4 was = batch->matrix;
		batch->matrix = batch->matrixStack.top();
		batch->matrixStack.pop();
		// return  was 
	}

	void InitRenderData2D(RenderData2D* sprite, int size)
	{
		// TODO(ck): Switch to fill batch for this... can not rely on model matrix anymore if using a batch
		// need to update the positions and the texture coordinates manually each frame.
		int x = 0;
		int y = 0;

		// NOTE(ck): TODO(ck):
		// size should be equal to 1 
		// 1 unit should equal 16 pixels which is the size of a normal quad in monster 2d
		Vertex vertices[4];
		vertices[0] = {};
		vertices[0].position = v3(x, y, 0.0f);
		vertices[0].color = v3(1.0f, 0.0f, 0.0f);
		vertices[0].texCoords = v2(0.0f, 0.0f);

		vertices[1] = {};
		vertices[1].position = v3(x + size, y, 0.0f);
		vertices[1].color = v3(1.0f, 0.0, 0.0f);
		vertices[1].texCoords = v2(1.0f, 0.0f);

		vertices[2] = {};
		vertices[2].position = v3(x + size, y + size, 0.0f);
		vertices[2].color = v3(1.0f, 1.0f, 1.0f);
		vertices[2].texCoords = v2(1.0f, 1.0f);

		vertices[3] = {};
		vertices[3].position = v3(x, y + size, 0.0f);
		vertices[3].color = v3(1.0f, 1.0f, 1.0f);
		vertices[3].texCoords = v2(0.0f, 1.0f);

		unsigned int indices[] = {
			0, 1, 3,
			1, 2, 3
		};

		unsigned int EBO;
		glGenVertexArrays(1, &sprite->VAO);
		glGenBuffers(1, &sprite->VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(sprite->VAO);
		glBindBuffer(GL_ARRAY_BUFFER, sprite->VBO);

		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,  sizeof(Vertex), (GLvoid*)0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		sprite->color = v3(1.0f);
		sprite->pos = {};
	}

	void InitAABB(RenderData2D* sprite)
	{

	}

	void InitTileMap(int tileAmount)
	{		 
		// TODO(ck): MEMORY - TEST 
		batch = new BatchData();
		// These will be figured out after looping our tilemap and pushing quads
		// TODO(ck): Need to be able to choose amount of vertices and indices
		batch->quadCount = 2000;
		batch->maxVertices = batch->quadCount * 4;
		batch->indicesLength = batch->quadCount * 6;

		// TODO(ck): Memory management
		batch->indices = new uint32_t[batch->indicesLength];
		int offset = 0;
		for (int i = 0; i < batch->indicesLength; i += 6)
		{
			batch->indices[i + 0] = 0 + offset;
			batch->indices[i + 1] = 1 + offset;
			batch->indices[i + 2] = 2 + offset;

			batch->indices[i + 3] = 2 + offset;
			batch->indices[i + 4] = 3 + offset;
			batch->indices[i + 5] = 0 + offset;

			offset += 4;
		}
		
		batch->mesh = {};
		InitOpenGLBatchMesh(batch);
	};

	void FillTileVertices(RenderData2D* sprite, int tileOffsetX, int tileOffsetY, float tileXPos, float tileYPos, int tileSize)
	{
		float sheetSize = 256.0f;
		int spriteSize = 16;

		// Texture coords
		float topRightX = ((tileOffsetX + 1) * spriteSize) / sheetSize;
		float topRightY = ((tileOffsetY + 1) * spriteSize) / sheetSize;
		float topLeftX = (tileOffsetX * spriteSize) / sheetSize;
		float topLeftY = ((tileOffsetY + 1) * spriteSize) / sheetSize;
		float bottomLeftX = (tileOffsetX * spriteSize) / sheetSize;
		float bottomLeftY = (tileOffsetY * spriteSize) / sheetSize;
		float bottomRightX = ((tileOffsetX + 1) * spriteSize) / sheetSize;
		float bottomRightY = (tileOffsetY * spriteSize) / sheetSize;

		// TODO(ck): pushQuad(pos, color, texcoords)		
		float x = tileXPos;
		float y = tileYPos;
		int size = tileSize;
		Vertex vec0 = {
			v3(x, y, 0.0f),
			v3(1.0f, 0.0f, 0.0f),
			v2(bottomLeftX, bottomLeftY)
		};

		Vertex vec1 = {
			v3(x + size, y, 0.0f),
			v3(0.0f, 1.0f, 0.0f),
			v2(bottomRightX, bottomRightY)
		};

		Vertex vec2 = {
			v3(x + size, y + size, 0.0f),
			v3(0.0f, 0.0f, 1.0f),
			v2(topRightX, topRightY)
		};

		Vertex vec3 = {
			v3(x, y + size, 0.0f),
			v3(1.0f, 1.0f, 0.0f),
			v2(topLeftX, topLeftY)
		};

		usedIndices += 6;
		sprite->vertices.push_back(vec0);
		sprite->vertices.push_back(vec1);
		sprite->vertices.push_back(vec2);
		sprite->vertices.push_back(vec3);
	}

	void FillBatch(int tileOffsetX, int tileOffsetY, float tileXPos, float tileYPos, int tileSize)
	{		
		float sheetSize = 256.0f;

		// Texture coords
		float topRightX		= ((tileOffsetX + 1) * tileSize) / sheetSize;
		float topRightY		= ((tileOffsetY + 1) * tileSize) / sheetSize;
		float topLeftX		= (tileOffsetX * tileSize) / sheetSize;
		float topLeftY		= ((tileOffsetY + 1) * tileSize) / sheetSize;
		float bottomLeftX	= (tileOffsetX * tileSize) / sheetSize;
		float bottomLeftY   = (tileOffsetY * tileSize) / sheetSize;
		float bottomRightX	= ((tileOffsetX + 1) * tileSize) / sheetSize;
		float bottomRightY	= (tileOffsetY * tileSize) / sheetSize;

		// TODO(ck): pushQuad(pos, color, texcoords)		
		float x = tileXPos * tileSize;
		float y = tileYPos * tileSize;
		int size = tileSize;
		Vertex vec0 = {
			v3(x, y, 0.0f),
			v3(1.0f, 0.0f, 0.0f),
			v2(bottomLeftX, bottomLeftY)	
		};

		Vertex vec1 = {
			v3(x + size, y, 0.0f),
			v3(0.0f, 1.0f, 0.0f),
			v2(bottomRightX, bottomRightY)
		};

		Vertex vec2 = {
			v3(x + size, y + size, 0.0f),
			v3(0.0f, 0.0f, 1.0f),
			v2(topRightX, topRightY)
		};

		Vertex vec3 = {
			v3(x, y + size, 0.0f),
			v3(1.0f, 1.0f, 0.0f),
			v2(topLeftX, topLeftY)
		};

		usedIndices += 6;
		tileVertices.push_back(vec0);
		tileVertices.push_back(vec1);
		tileVertices.push_back(vec2);
		tileVertices.push_back(vec3);
	}

	void BindVertices()
	{
		// 2 extra vertices are needed for degenerate triangles between each strip 
		//unsigned uNumExtraVertices = ( GL_TRIANGLE_STRIP == _config.uRenderType && _uNumUsedVertices > 0 ? 2 : 0 ); 

		glBindVertexArray(batch->mesh.VAO);
		glBindBuffer(GL_ARRAY_BUFFER, batch->mesh.VBO);
		//if (uNumExtraVertices > 0)
		//{
		//	//need to add 2 vertex copies to create degenerate triangles between this strip 
		//	//and the last strip that was stored in the batch 
		//	glBufferSubData(GL_ARRAY_BUFFER, _uNumUsedVertices * sizeof(GuiVertex), sizeof(GuiVertex), &_lastVertex);
		//	glBufferSubData(GL_ARRAY_BUFFER, (_uNumUsedVertices + 1) * sizeof(GuiVertex), sizeof(GuiVertex), &vVertices[0]);
		//}

		// Use glMapBuffer instead, if moving large chunks of data > 1MB 
		//int _uNumUsedVertices = 400;
		//int uNumExtraVertices = 2;
		//glBufferSubData(GL_ARRAY_BUFFER, (_uNumUsedVertices + uNumExtraVertices) * sizeof(Vertex), newTileVertices.size() * sizeof(Vertex), &newTileVertices[0]);
		
		// IMPORTANT(ck):
		// STUDY(ck): The second param (offset) in this was set to verticesLength * sizeof(Vertex). This was causing the vertices the show up as stretched 
		// and elongated triangles
		
		//glBufferSubData(GL_ARRAY_BUFFER, 0, tileVertices.size() * sizeof(Vertex), &tileVertices[0]);
		// TODO(ck): Should be GL_ELEMENT_ARRAY_BUFFER?
		glBufferData(GL_ARRAY_BUFFER, tileVertices.size() * sizeof(Vertex), &tileVertices[0], GL_DYNAMIC_DRAW);
		/*
			// Upload Buffer
			gl.BindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
			gl.BufferData(GL_ARRAY_BUFFER, m_vertex_size * count, vertices, GL_DYNAMIC_DRAW);
		
		*/


		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//_uNumUsedVertices += vVertices.size() + uNumExtraVertices;
		//_lastVertex = vVertices[vVertices.size() - 1];
	}

	void DrawMap(CommonProgram* shader, RenderData2D* sprite, unsigned int textureID)
	{
		glUseProgram(shader->handle);
		
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(glGetUniformLocation(shader->handle, "imageArray"), 0);
		glUniform1i(glGetUniformLocation(shader->handle, "diffuse_layer"), 0);
		glBindTexture(GL_TEXTURE_2D_ARRAY, textureID);

		glBindVertexArray(sprite->VAO);
		glDrawElements(GL_TRIANGLES, 9600, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	void DrawObject(CommonProgram* shader, RenderData2D* data)
	{
		//glUseProgram(shader->id);

		// TODO(ck): We don't calculate the matrix here
		// we calc it in the game and send the matrix to the
		// renderer 
		mat4 model = mat4(1.0f);
		v3 tilePosition = {};
		tilePosition.x = data->pos.x;
		tilePosition.y = data->pos.y;
		model = glm::translate(model, tilePosition);

		//model = glm::translate(model, v3(0.5f * data->size.x, 0.0f * data->size.y, 0.0f));
		//model = glm::rotate(model, obj->rotation, v3(0.0f, 0.0f, 1.0f));
		//model = glm::translate(model, v3(-0.5f * data->size.x, -0.5f * data->size.y, 0.0f));

		//model = glm::scale(model, v3(data->size, 1.0f));

		glUniformMatrix4fv(glGetUniformLocation(shader->handle, "model"), 1, GL_FALSE, glm::value_ptr(model));
		
		//glUniform3f(glGetUniformLocation(shader->handle, "spriteColor"), data->color.r, data->color.g, data->color.b);

		//glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, data->texture.id);

		glBindVertexArray(data->VAO);

		//if (data->wireFrame)
		//	glDrawArrays(GL_LINES, 0, 6);
		//else 
		//	glDrawArrays(GL_TRIANGLES, 0, 6);


		//glDrawElements(GL_TRIANGLES, indices, GL_UNSIGNED_INT, (void*)(2 * 0));
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);


	}

	void DrawMap(CommonProgram* shader, unsigned int textureID, bool wireFrame)
	{
		// TODO(ck): If we want the ability to change the map at runtime we need to constantly
		// be filling and binding the batch (if things have changed)
		// Fill batch 
		// bind vertices


		mat4 model = mat4(1.0f);
		glUniformMatrix4fv(glGetUniformLocation(shader->handle, "model"), 1, GL_FALSE, glm::value_ptr(model));
		//v3 tilePosition = {};
		//tilePosition.x = data->pos.x;
		//tilePosition.y = data->pos.y;
		//model = glm::translate(model, tilePosition);


		//glActiveTexture(GL_TEXTURE0);
		// IMPORTANT(ck):
		// NOTE(ck): the reason why you set it to 0 is because thats the base texture slot
		// its not expecting the textureID thats only for binding
		glUniform1i(glGetUniformLocation(shader->handle, "image"), 0);
		glBindTexture(GL_TEXTURE_2D, textureID);

		glBindVertexArray(batch->mesh.VAO);
		// (void*)(index_size * pass.index_start)
		wireFrame ?
			glDrawElements(GL_LINES, usedIndices, GL_UNSIGNED_INT, (void*)(2 * 0))
			: glDrawElements(GL_TRIANGLES, usedIndices, GL_UNSIGNED_INT, (void*)(2 * 0));

		glBindVertexArray(0);

		//reset buffer
		//_uNumUsedVertices = 0;
		//_config.iPriority = 0;
		usedIndices = 0;
		tileVertices.clear();
	}

	void DrawMap(CommonProgram* shader, unsigned int textureID, int batchThing)
	{
		//
		// Fill batch 
		//
		
		
		//
		// bind vertices
		// 
		BindVertices();

		// 2 extra vertices are needed for degenerate triangles between each strip 
		//unsigned uNumExtraVertices = ( GL_TRIANGLE_STRIP == _config.uRenderType && _uNumUsedVertices > 0 ? 2 : 0 ); 

		glBindVertexArray(batch->mesh.VAO);
		glBindBuffer(GL_ARRAY_BUFFER, batch->mesh.VBO);
		//if (uNumExtraVertices > 0)
		//{
		//	//need to add 2 vertex copies to create degenerate triangles between this strip 
		//	//and the last strip that was stored in the batch 
		//	glBufferSubData(GL_ARRAY_BUFFER, _uNumUsedVertices * sizeof(GuiVertex), sizeof(GuiVertex), &_lastVertex);
		//	glBufferSubData(GL_ARRAY_BUFFER, (_uNumUsedVertices + 1) * sizeof(GuiVertex), sizeof(GuiVertex), &vVertices[0]);
		//}

		// Use glMapBuffer instead, if moving large chunks of data > 1MB 
		//int _uNumUsedVertices = 400;
		//int uNumExtraVertices = 2;
		//glBufferSubData(GL_ARRAY_BUFFER, (_uNumUsedVertices + uNumExtraVertices) * sizeof(Vertex), newTileVertices.size() * sizeof(Vertex), &newTileVertices[0]);

		// IMPORTANT(ck):
		// STUDY(ck): The second param (offset) in this was set to verticesLength * sizeof(Vertex). This was causing the vertices the show up as stretched 
		// and elongated triangles

		//glBufferSubData(GL_ARRAY_BUFFER, 0, tileVertices.size() * sizeof(Vertex), &tileVertices[0]);
		// TODO(ck): Should be GL_ELEMENT_ARRAY_BUFFER ??
		glBufferData(GL_ARRAY_BUFFER, tileVertices.size() * sizeof(Vertex), &tileVertices[0], GL_DYNAMIC_DRAW);
		/*
			// Upload Buffer
			gl.BindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
			gl.BufferData(GL_ARRAY_BUFFER, m_vertex_size * count, vertices, GL_DYNAMIC_DRAW);

		*/


		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//_uNumUsedVertices += vVertices.size() + uNumExtraVertices;
		//_lastVertex = vVertices[vVertices.size() - 1];


		//
		// Render batch
		//

		//glUseProgram(shader->id);

		//glActiveTexture(GL_TEXTURE0);
		// IMPORTANT(ck):
		// NOTE(ck): the reason why you set it to 0 is because thats the base texture slot
		// its not expecting the textureID thats only for binding
		glUniform1i(glGetUniformLocation(shader->handle, "image"), 0);
		glBindTexture(GL_TEXTURE_2D, textureID);

		glBindVertexArray(batch->mesh.VAO);
		// (void*)(index_size * pass.index_start)
		glDrawElements(GL_TRIANGLES, usedIndices, GL_UNSIGNED_INT, (void*)(2 * 0));
		glBindVertexArray(0);

		//reset buffer
		//_uNumUsedVertices = 0;
		//_config.iPriority = 0;

		// clear batch
	}

}