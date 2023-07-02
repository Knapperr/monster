#include "mon_gl_render.h"

#include <glad/glad.h>

namespace MonGL
{
	// TODO(ck): TODO(ck): Somehow put into game so we can call from gui
	// can have a getter method that retrieves the globalDrawCalls from here
	int globalDrawCalls = 0;

	// TODO(ck): Remove std::string
	void LoadTexture(std::string name, Texture* texture, TextureType type, bool pixelArtTexture, Image* image)
	{
		LoadTextureFile(name, texture, image, type, true, pixelArtTexture);
	}

	// TODO(ck): Remove std::string
	void LoadTextureFile(std::string name, Texture* texture, Image* image, TextureType type, bool linearFilter, bool pixelArtTexture)
	{
		texture->name = name;
		texture->type = type;
		// TODO(CK): Clean up
		// set wrap and filter here for now same with internal formats
		if (pixelArtTexture)
		{
			texture->wrapS = GL_CLAMP_TO_EDGE;
			texture->wrapT = GL_CLAMP_TO_EDGE;
			texture->filterMin = GL_NEAREST;
			texture->filterMax = GL_NEAREST;

			// Linear for 3D
			if (linearFilter)
			{
				texture->wrapS = GL_REPEAT; // TODO(ck): Not sure if necessary
				texture->wrapT = GL_REPEAT; // TODO(ck): Not sure if necessary
				texture->filterMin = GL_LINEAR;
				texture->filterMax = GL_LINEAR;
			}
		}
		else
		{
			texture->wrapS = GL_REPEAT;
			texture->wrapT = GL_REPEAT;
			texture->filterMin = GL_LINEAR_MIPMAP_LINEAR;
			texture->filterMax = GL_LINEAR;
		}

		if (image->data)
		{
			Generate2DTexture(texture, image->width, image->height, image->nrChannels, image->data);
		}
		else
		{
			// TODO(ck): no strings
			std::string msg = "failed to load texture";
			Mon::Log::warn(msg.c_str());
		}
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
		// unbind
		glBindVertexArray(0);
	}

	void UploadOpenGLMesh2D(Mesh2D* mesh)
	{
		glGenVertexArrays(1, &mesh->VAO);
		glBindVertexArray(mesh->VAO);

		glGenBuffers(1, &mesh->VBO);
		glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
		glBufferData(GL_ARRAY_BUFFER, mesh->verticeCount * sizeof(Vertex), mesh->vertices, GL_STATIC_DRAW);

		glGenBuffers(1, &mesh->IBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indiceCount * sizeof(unsigned int), mesh->indices, GL_STATIC_DRAW);

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

	///
	/// [begin] CUBE MAP
	///

	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,

		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,

		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,

		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,

		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};

	void LoadCubeMapTexture(Texture* texture)
	{
		texture->type = TextureType::CubeMap;

		glGenTextures(1, &texture->id);
		glBindTexture(GL_TEXTURE_CUBE_MAP, texture->id);

		for (unsigned int i = 0; i < 6; ++i)
		{
			// TODO(ck): NEED TO LOOKUP CUBEMAP IMAGES
			// HAVE A CUBEMAP ASSET THAT IS SET IN THE FILE??? THAT WAY THE GUI CAN SET THE 
			// INDEX TO THE TEXTURE AND SAVE IT SO ITS NOT A BIG DEAL... JUST TAG IMAGES 
			Image* img = GetImage(g_Assets, 24 + i);

			if (img->data)
			{
				texture->width = img->width;
				texture->height = img->height;
				texture->internalFormat = GL_RGB;
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
							 0, GL_RGB, img->width, img->height, 0, GL_RGB, GL_UNSIGNED_BYTE, img->data);
			}
			else
			{
				// TODO(ck): no strings
				std::string msg = "failed to load cubemap textures";
				Mon::Log::warn(msg.c_str());
			}
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}

	void LoadCubemap(Cubemap* cubemap)
	{
		glGenVertexArrays(1, &cubemap->VAO);
		glGenBuffers(1, &cubemap->VBO);
		glBindVertexArray(cubemap->VAO);
		glBindBuffer(GL_ARRAY_BUFFER, cubemap->VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	}

	///
	/// [END] CUBE MAP
	/// 


	///
	/// [BEGIN] Renderer
	///	

	void InitRenderer(OpenGL* gl)
	{
		/*
		TODO(ck): Load from a configuration file
		should only load meshses in current chunk or level
		*/
		gl->program = {};
		gl->waterProgram = {};
		gl->cubemapProgram = {};

		// TODO(ck): Remove 
		MonGL::LoadShader(&gl->program, "res/shaders/vert_colors.glsl", "res/shaders/frag_colors.glsl", NULL);
		MonGL::LoadShader(&gl->quadProgram, "res/shaders/vert_quad_batch.glsl", "res/shaders/frag_quad_batch.glsl", NULL);
		MonGL::LoadShader(&gl->waterProgram, "res/shaders/vert_water.glsl", "res/shaders/frag_water.glsl", NULL);
		MonGL::LoadShader(&gl->cubemapProgram, "res/shaders/vert_cubemap.glsl", "res/shaders/frag_cubemap.glsl", NULL);
		
		// TODO(ck): MOVE TEXTURE IDS TO data... program can have its own textues too?? 
		// a program can be like a material that can be applied to a mesh so it needs to have its own textures
		gl->waterProgram.textureIndexNormal1 = 6;
		gl->waterProgram.textureIndexNormal2 = 15;
		
		// NOTE(ck): first index loaded
		AddTexture(gl);	
		unsigned int assetCount = g_Assets->textureAssetCount;
		for (unsigned int i = 1; i < assetCount; ++i)
		{
			TextureAsset* asset = Mon::GetTextureAsset(g_Assets, i);
			AddTexture(gl);
			MonGL::Texture* t = GetTexture(gl, i);
			//std::string name = std::string(asset->name);
			
			// TODO(ck): remove std::string
			LoadTexture(asset->name, t, asset->type, asset->isPixelArt, GetImage(g_Assets, asset->imageIndex));
		}

		// TODO(ck): IMPORTANT(ck): FIX THIS 
		// NOTE(ck): using image indexes [24 to 29] need a better way to handle this
		AddTexture(gl);
		MonGL::Texture* t23 = GetTexture(gl, assetCount+1);
		LoadCubeMapTexture(t23);
		gl->cubemap = {};
		LoadCubemap(&gl->cubemap);


		// #0 for the 
		AddLight(gl);
		int index = AddLight(gl);
		gl->lights[index].id = "001";
		gl->lights[index].pos = v3(24.0f, 64.0f, 26.0f);
		gl->lights[index].ambient = v3(0.3f, 0.1f, 0.0f);
		gl->lights[index].diffuse = v3(0.8f);
		gl->lights[index].specular = v3(0.3f);
		gl->lights[index].attachedToEntity = false;
		index = AddLight(gl);
		gl->lights[index].id = "002";
		gl->lights[index].pos = v3(24.0f, 64.0f, 20.0f);
		gl->lights[index].ambient = v3(1.0f);
		gl->lights[index].diffuse = v3(1.0f);
		gl->lights[index].specular = v3(1.0f);
		gl->lights[index].attachedToEntity = false;

		// Init Batches
		// 0 is null batch
		AddBatch(gl);
		int batchIndex = AddBatch(gl);
		InitBatch(gl, batchIndex);

		// Init Sub Textures for sprite animation
		//for (int i = 0; i < 10; ++i)
		//{

		//	unsigned int index = gl->subTextureCount++;
		//	GLSubTexture *subText = &gl->subTextures[index];
		//	subText = {};


		//}
		// Init Animations
		{
			// null at index 0
			GLSpriteAnimation* glAnimation = &gl->spriteAnimations[0];
			glAnimation = {};
			gl->spriteAnimationCount++;

			GLSpriteAnimation* glAni1 = &gl->spriteAnimations[1];
			glAni1->textureAtlasIndex = 1;
			gl->spriteAnimationCount++;

			const int subTextureCount = 4;
			float sheetSize = 256.0f;
			int tileSize = 32;
			int tileOffsetX = 3;			
			int tileOffsetY = 7;

			for (int i = 0; i < subTextureCount; ++i)
			{
				if (i == 1)
				{
					tileOffsetX = 2;
				}
				else if (i == 2)
				{
					tileOffsetX = 1;
				}
				else if (i == 3)
				{
					tileOffsetX = 0;
				}

				GLSubTexture* subT = &gl->spriteAnimations[1].subTextures[i];
				
				subT->height = 32;
				subT->width = 32;
				subT->textureAtlasIndex = 1;
				subT->tileSize = 32;
				subT->texCoords[0] = v2((tileOffsetX * tileSize) / sheetSize, (tileOffsetY * tileSize) / sheetSize); // bottom left
				subT->texCoords[1] = v2(((tileOffsetX + 1) * tileSize) / sheetSize, (tileOffsetY * tileSize) / sheetSize); // bottom right
				subT->texCoords[2] = v2(((tileOffsetX + 1) * tileSize) / sheetSize, ((tileOffsetY + 1) * tileSize) / sheetSize); // top right
				subT->texCoords[3] = v2((tileOffsetX * tileSize) / sheetSize, ((tileOffsetY + 1) * tileSize) / sheetSize); // top left 

			}
		}

		// Frame buffer
		int screenWidth = 960;
		int screenHeight = 540;
		glGenFramebuffers(1, &gl->buffer.handle);
		glBindFramebuffer(GL_FRAMEBUFFER, gl->buffer.handle);

		glGenTextures(1, &gl->textureColorbuffer.handle);
		glBindTexture(GL_TEXTURE_2D, gl->textureColorbuffer.handle);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gl->textureColorbuffer.handle, 0);

		/*
		https://www.khronos.org/opengl/wiki/Renderbuffer_Object#:~:text=Renderbuffer%20Objects%20are%20OpenGL%20Objects,shader)%20from%20the%20produced%20image.
			Renderbuffer Objects are OpenGL Objects that contain images. They are created and used specifically with 
			Framebuffer Objects. They are optimized for use as render targets, while Textures may not be, and are the
			logical choice when you do not need to sample (i.e. in a post-pass shader) from the produced image.
			If you need to resample (such as when reading depth back in a second shader pass), use Textures instead.
			Renderbuffer objects also natively accommodate Multisampling (MSAA).
		*/
		// (not sampling it so just doing this locally in the method)
		unsigned int rbo;
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);


		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			Log::warn("Framebuffer is not complete!");
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	///
	/// [END] 
	///	

	///
	/// [BEGIN] Shader Programs
	/// 

	void UseProgram(CommonProgram* program)
	{
		glUseProgram(program->handle);
	}

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

	void UseProgram(QuadBatchProgram* program, RenderSetup setup)
	{
		UseProgram(&program->common, setup);


		// BEGIN RENDER UNIFORMS
		glUniformMatrix4fv(program->common.projection, 1, GL_FALSE, glm::value_ptr(setup.projection));
		glUniformMatrix4fv(program->common.view, 1, GL_FALSE, glm::value_ptr(setup.viewMatrix));
	}

	void UseProgram(WaterProgram* program, RenderSetup setup)
	{
		UseProgram(&program->common, setup);

	}

	///
	/// [BEGIN] Init RenderData
	///

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

	void SetBoundingBox(RenderData* data)
	{
		data->lineWidth = 2;
		data->color = v3(0.7f, 0.15f, 0.4f);
		data->meshIndex = 5;
		data->visible = true;
	}

	void SetCube(RenderData* data)
	{
		data->visible = true;
		data->scale = v3(1.0f);
		data->meshIndex = 2;
	}

	// TODO(ck): can remove this?
	// NOTE(ck): Assume the vertices and indices have been loaded
	void SetModel(RenderData* data)
	{
		data->scale = v3(1.0f);
		data->visible = true;
	}

	/// 
	/// [END] INIT 
	/// 

	/// 
	/// [BEGIN] begin render 
	/// 

	void BeginRender(OpenGL* gl, Config* config, mat4 projection, mat4 view, int shaderID)
	{
		//glBindFramebuffer(GL_FRAMEBUFFER, gl->buffer.handle);
		

		// glDisable(GL_CULL_FACE); --- might need to turn off for cube map

		/////glEnable(GL_CULL_FACE);
		/////glDepthFunc(GL_ALWAYS);

		// TODO(ck): Platform->Renderer->clearColor 
		glClearColor(0.126f, 0.113f, 0.165f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUniformMatrix4fv(glGetUniformLocation(shaderID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(shaderID, "view"), 1, GL_FALSE, glm::value_ptr(view));

		MonGL::ViewPort(&config->viewPort);
		globalDrawCalls = 0;


		// set up lights 
		// make a uniform buffer of the lights and send it to the shader


	}

	///
	/// [END] BEGIN RENDER
	/// 

	///
	/// [BEGIN] Debug Drawing
	///

	void InitLine(Line* line)
	{
		// TODO(ck): Memory Allocation
		line->data.meshIndex = 6;
		line->data.color = v3(0.1f, 0.5f, 1.0f);
		line->data.visible = true;
	}

	void DrawLine(OpenGL* gl, Line* line)
	{
		Mesh* mesh = GetMesh(g_Assets, line->data.meshIndex);
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
		if (!data->visible)
			return;

		Mesh* mesh = GetMesh(g_Assets, data->meshIndex);
		unsigned int shaderID = gl->program.handle;
		glUseProgram(shaderID);

		glUniform3fv(glGetUniformLocation(shaderID, "viewPos"), 1, &camera->pos[0]);

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

	// TODO(ck): STOP PASSING TEXTUREID IMPORTANT(ck):
	void ActivateUniforms(CommonProgram* program, ProgramData programData, RenderSetup setup, Light light, v3 viewPos, int textureID)
	{
		// TODO(ck): Performance - Only call this if the program actually changes or find a way to call at the beginning 
		// sort entities so that water is always at the end so that gluseprogram gets called before the water is drawn
		
		// IMPORTANT(ck):
		// CANT CALL THIS EVERYTIME I DRAW SOMETHING NEED TO 
		// have a BeginRender() that calls glUseProgram 
		// water might have to be separated out?
		//glUseProgram(program->handle);

		// yeah this design is flawed i can just switch the water shader at the end and 
		// call any mesh on it... 
		// you do want to activate the uniforms but you dont want to call glUseProgram
		
		// TODO(ck): Need begin render attributes in here
		//MonGL::ViewPort(&config->viewPort);


		bool useTexture = true;
		glUniform1i(program->useTexture, useTexture);
		glUniform1i(program->pixelTexture, useTexture);
		// bind textures on corresponding texture units
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(program->textureDiffuse1, 0);
		glBindTexture(GL_TEXTURE_2D, textureID);
		// bind texturenormal1 and texturenormal2

		//glUniform3fv(glGetUniformLocation(shaderID, "colliderColor"), 1, &data->color[0]);
		glUniform1i(program->collider, false);

		glUniform1f(program->texCoordScale, programData.texCoordScale);

		glUniform3fv(program->lightPos, 1, &light.pos[0]);
		glUniform3fv(program->lightAmbient, 1, &light.ambient[0]);
		glUniform3fv(program->lightDiffuse, 1, &light.diffuse[0]);
		glUniform3fv(program->lightSpecular, 1, &light.specular[0]);
		glUniform3fv(program->viewPos, 1, &viewPos[0]);


		glUniform1f(glGetUniformLocation(program->handle, "material.shininess"), setup.materialShininess);

	}

	// TODO(ck): STOP PASSING TEXTUREID IMPORTANT(ck):
	void ActivateUniforms(WaterProgram* program, ProgramData programData, RenderSetup setup, v3 viewPos, Light light, int textureID, int normal1TextureID, int normal2TextureID)
	{
		// need to pass struct or something to shader because they are
		// going to be different...
		// STUDY(ck):  can you have shader includes in glsl??????
		glUseProgram(program->common.handle);

		ActivateUniforms(&program->common, programData, setup, light, viewPos, textureID);

		// BEGIN RENDER UNIFORMS
		glUniformMatrix4fv(program->common.projection, 1, GL_FALSE, glm::value_ptr(setup.projection));
		glUniformMatrix4fv(program->common.view, 1, GL_FALSE, glm::value_ptr(setup.viewMatrix));
		
		// GET THIS INFO
		glActiveTexture(GL_TEXTURE1);
		glUniform1i(program->textureNormal1, 1);
		glBindTexture(GL_TEXTURE_2D, normal1TextureID);

		glActiveTexture(GL_TEXTURE2);
		glUniform1i(program->textureNormal2, 2);
		glBindTexture(GL_TEXTURE_2D, normal2TextureID);

		glUniform1f(program->time, setup.time);
		glUniform1f(program->uJump, programData.uJump);
		glUniform1f(program->vJump, programData.vJump);
		glUniform1f(program->tiling, programData.tiling);
		glUniform1f(program->speed, programData.speed);
		glUniform1f(program->flowStrength, programData.flowStrength);
		glUniform1f(program->flowOffset, programData.flowOffset);
		glUniform1f(program->heightScale, programData.heightScale);
		glUniform1f(program->heightScaleModulated, programData.heightScaleModulated);
		glUniform1f(program->waveLength, programData.waveLength);
	}

	int ActivateUniforms(OpenGL* gl, ProgramData programData, ProgramType type, RenderSetup setup, int baseTextureID, v3 viewPos)
	{
		// TODO(ck): Just check the type here if a new program type comes in then we have to activate uniforms and stuff otherwise just continue 
		// like normally that way we don't need a switch everytime a new shader type comes in we just simply call glUseProgram and whatever other
		// setup we need to do but if not we don't call it we just activate uniforms as normal

		Light* light = GetLight(gl, 1);
		switch (type)
		{
		case ProgramType::Common:
			ActivateUniforms(&gl->program, programData, setup, *light, viewPos, baseTextureID);
			return gl->program.handle;
		case ProgramType::Water:
			Texture* normal1 = GetTexture(gl, gl->waterProgram.textureIndexNormal1);
			Texture* normal2 =  GetTexture(gl, gl->waterProgram.textureIndexNormal2);
			ActivateUniforms(&gl->waterProgram, programData, setup, viewPos, *light, baseTextureID, normal1->id, normal2->id);
			return gl->waterProgram.common.handle;
		}

		return -1;
	}

	//
	// Public Draw Calls
	//

	// TODO(ck): Maybe pass OpenGL to this then we have all the data?
	void Draw(OpenGL* gl, Config* config, RenderSetup setup, float spriteAngleDegrees, RenderData* data, v3 pos, Camera* camera)
	{
		if (!data->visible)
			return;

		Mesh* mesh = GetMesh(g_Assets, data->meshIndex);
		Texture* texture = GetTexture(gl, data->textureIndex);

		// if programType == water && first time hitting this? view pos as well
		//		glUseProgram(water);
		// TODO(ck): To get this working just call glUseProgram on the water because its the only entity that has it right now
		 
		int shaderHandle = ActivateUniforms(gl, data->programData, data->programType, setup, texture->id, camera->pos);

		data->worldMatrix = mat4(1.0f);
		data->worldMatrix = glm::translate(data->worldMatrix, pos);
		if (mesh->indiceCount > 0)
		{
			data->worldMatrix = glm::rotate(data->worldMatrix, glm::radians(spriteAngleDegrees), v3{ 1.0f, 0.0f, 0.0f });
		}
		data->worldMatrix = glm::scale(data->worldMatrix, data->scale);
		
		// NOTE(ck): Need to get this with glGetUniformLocation because we are doing a shader switch in this
		// 
		//glUniformMatrix4fv(gl->program.model, 1, GL_FALSE, glm::value_ptr(data->worldMatrix));
		glUniformMatrix4fv(glGetUniformLocation(shaderHandle, "model"), 1, GL_FALSE, glm::value_ptr(data->worldMatrix));

		int polygonMode = data->wireFrame ? GL_LINE : GL_FILL;
		glPolygonMode(GL_FRONT_AND_BACK, polygonMode);

		glBindVertexArray(mesh->VAO);
		if (mesh->indiceCount > 0)
		{
			glDrawElements(GL_TRIANGLES, mesh->indiceCount, GL_UNSIGNED_INT, 0);
		}
		else
		{
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		// TODO(ck): This is bugging us out
		// Always good practice to set everything back to defaults once configured
		// NOTE(CK): bind texture must be AFTER glActiveTexture or it will not unbind properly
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, 0);
		globalDrawCalls++;
	}

	void DrawLights(OpenGL* gl)
	{
		Mesh* mesh = GetMesh(g_Assets, 9);
		Texture* texture = GetTexture(gl, 5);
		mat4 worldMatrix = mat4(1.0f);

		for (int i = 1; i < gl->lightCount; ++i)
		{
			worldMatrix = mat4(1.0f);
			worldMatrix = glm::translate(worldMatrix, gl->lights[i].pos);
			glUniformMatrix4fv(glGetUniformLocation(gl->program.handle , "model"), 1, GL_FALSE, glm::value_ptr(worldMatrix));

			int polygonMode = GL_LINE; // GL_FILL
			glPolygonMode(GL_FRONT_AND_BACK, polygonMode);
			
			glBindVertexArray(mesh->VAO);
			if (mesh->indiceCount > 0)
			{
				glDrawElements(GL_TRIANGLES, mesh->indiceCount, GL_UNSIGNED_INT, 0);
			}
			else
			{
				glDrawArrays(GL_TRIANGLES, 0, 36);
			}

			glBindVertexArray(0);
			globalDrawCalls++;
		}
	}

	void DrawCubeMap(OpenGL* gl, RenderSetup setup)
	{
		//glStencilMask(0x00);
		glDepthFunc(GL_LEQUAL);
		// TODO(ck): Might need cull face for cube map
		//glEnable(GL_CULL_FACE); 

		glUseProgram(gl->cubemapProgram.common.handle);

		// remove translation from the view matrix
		mat4 view = mat4(mat3(setup.viewMatrix));
		glUniformMatrix4fv(gl->cubemapProgram.common.view, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(gl->cubemapProgram.common.projection, 1, GL_FALSE, glm::value_ptr(setup.projection));
	
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		
		glBindVertexArray(gl->cubemap.VAO);
		
		Texture* texture = GetTexture(gl, gl->cubemapProgram.skyboxTextureIndex);
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(gl->cubemapProgram.skybox, 0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, texture->id);

		glDrawArrays(GL_TRIANGLES, 0, 36);
		
		glBindVertexArray(0);
		// back to default
		glDepthFunc(GL_LESS);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		globalDrawCalls++;
	}

	void DrawTerrain(OpenGL* gl, RenderData* data, Camera* camera)
	{
		Mesh* mesh = GetMesh(g_Assets, data->meshIndex);
		Texture* texture = GetTexture(gl, data->textureIndex);
		unsigned int shaderID = gl->program.handle;
		
		Light* light = GetLight(gl, 1);

		glUniform1f(glGetUniformLocation(shaderID, "texCoordScale"), data->programData.texCoordScale);

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

		glActiveTexture(GL_TEXTURE0);
		glUniform1i(glGetUniformLocation(shaderID, "texture_diffuse1"), 0);
		glBindTexture(GL_TEXTURE_2D, texture->id);

		mat4 matModel = mat4(1.0f);
		glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, glm::value_ptr(matModel));
		glBindVertexArray(mesh->VAO);

		int polygonMode = data->wireFrame ? GL_LINE : GL_FILL;
		glPolygonMode(GL_FRONT_AND_BACK, polygonMode);

		glDrawElements(GL_TRIANGLES, mesh->indiceCount, GL_UNSIGNED_INT, 0);

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
	///	[BEGIN] Batch Rendering
	///

	
	void InitBatch(OpenGL* gl, int batchIndex)
	{
		// batch 
		int quadCount = 4096;
		int maxVertices = quadCount * 4;
		int indicesLength = quadCount * 6;


		// init batch mesh
		Batch* batch = GetBatch(gl, batchIndex);


		// TODO(ck): TEMP remove
		batch->sheetTextureIndex = 8;

		batch->indices = new uint32_t[indicesLength];
		int offset = 0;
		for (int i = 0; i < indicesLength; i += 6)
		{
			batch->indices[i + 0] = 0 + offset;
			batch->indices[i + 1] = 1 + offset;
			batch->indices[i + 2] = 2 + offset;
				 
			batch->indices[i + 3] = 2 + offset;
			batch->indices[i + 4] = 3 + offset;
			batch->indices[i + 5] = 0 + offset;

			offset += 4;
		}

		glGenVertexArrays(1, &batch->VAO);
		glBindVertexArray(batch->VAO);

		glGenBuffers(1, &batch->VBO);
		glBindBuffer(GL_ARRAY_BUFFER, batch->VBO);
		glBufferData(GL_ARRAY_BUFFER, maxVertices * sizeof(Vertex3D), nullptr, GL_DYNAMIC_DRAW);

		glGenBuffers(1, &batch->IBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, batch->IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesLength * sizeof(batch->IBO), batch->indices, GL_DYNAMIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, position));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, normal));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, texCoords));

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}


	void FillBatch(Batch* batch, float posX, float posY, float posZ, int texOffsetX, int texOffsetY, int tileSize)
	{
		float textureSheetSize = 256.0f;
		posX = posX - 0.5f;
		posY = posY - 0.5f;
		float vertSize = 1.0f;

		// Texture coords

		// use SubTexture for texture coords

		v2 tr = v2(((texOffsetX + 1) * tileSize) / textureSheetSize, ((texOffsetY + 1) * tileSize) / textureSheetSize);
		v2 tl = v2((texOffsetX * tileSize) / textureSheetSize, ((texOffsetY + 1) * tileSize) / textureSheetSize);

		v2 br = v2(((texOffsetX + 1) * tileSize) / textureSheetSize, (texOffsetY * tileSize) / textureSheetSize);
		v2 bl = v2((texOffsetX * tileSize) / textureSheetSize, (texOffsetY * tileSize) / textureSheetSize);

		Vertex3D vec0 = {
			v3(posX, posY, posZ),
			v3(1.0f, 0.0f, 0.0f),
			bl
		};

		Vertex3D vec1 = {
			v3((posX + vertSize), posY, posZ),
			v3(1.0f, 1.0f, 1.0f),
			br
		};

		Vertex3D vec2 = {
			v3((posX + vertSize), (posY + vertSize), (posZ)),
			v3(1.0f, 1.0f, 1.0f),
			tr
		};

		Vertex3D vec3 = {
			v3(posX, (posY + vertSize), posZ),
			v3(1.0f, 1.0f, 1.0f),
			tl
		};

		batch->usedIndices += 6;
		batch->vertices_.push_back(vec0);
		batch->vertices_.push_back(vec1);
		batch->vertices_.push_back(vec2);
		batch->vertices_.push_back(vec3);
	}

	void FillBatch(Batch* batch, float posX, float posY, float posZ, GLSubTexture* subTexture, int tileSize)
	{
		float textureSheetSize = 256.0f;
		posX = posX - 0.5f;
		posY = posY - 0.5f;
		float vertSize = 1.0f;

		Vertex3D vec0 = {
			v3(posX, posY, posZ),
			v3(1.0f, 0.0f, 0.0f),
			subTexture->texCoords[0]
		};

		Vertex3D vec1 = {
			v3((posX + vertSize), posY, posZ),
			v3(1.0f, 1.0f, 1.0f),
			subTexture->texCoords[1]
		};

		Vertex3D vec2 = {
			v3((posX + vertSize), (posY + vertSize), (posZ)),
			v3(1.0f, 1.0f, 1.0f),
			subTexture->texCoords[2]
		};

		Vertex3D vec3 = {
			v3(posX, (posY + vertSize), posZ),
			v3(1.0f, 1.0f, 1.0f),
			subTexture->texCoords[3]
		};

		batch->usedIndices += 6;
		batch->vertices_.push_back(vec0);
		batch->vertices_.push_back(vec1);
		batch->vertices_.push_back(vec2);
		batch->vertices_.push_back(vec3);
	}

	void BindBatchVertices(Batch* batch)
	{
		glBindVertexArray(batch->VAO);
		glBindBuffer(GL_ARRAY_BUFFER, batch->VBO);

		// Use glMapBuffer instead, if moving large chunks of data > 1MB 
		glBufferSubData(GL_ARRAY_BUFFER, 0, batch->vertices_.size() * sizeof(Vertex3D), &batch->vertices_[0]);

		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	
	void DrawBatch(OpenGL* gl, Batch* batch)
	{
		// TODO(ck): If we want the ability to change the map at runtime we need to constantly
// be filling and binding the batch (if things have changed)
// Fill batch 
// bind vertices
		v3 pos = {};
		v3 basePos = v3(0.0f, 0.0f, 0.0f);
		pos.x = basePos.x;
		pos.y = basePos.y;



		mat4 model = mat4(1.0f);
		model = glm::translate(model, pos);
		// TODO(ck): Rotation causes Z axis to grow??? how to apply rotations to a sprite batcher?
		// .. world coordinates are baked into vertices can it be done in shader?
		//model = glm::rotate(model, glm::radians(-45.0f), v3{ 0.0f, 0.0f, 1.0f });

		//v2 worldScale = v2(64.0f);
		//model = glm::scale(model, v3(worldScale, 1.0f));

		int batchShaderHandle = gl->quadProgram.common.handle;
		glUniformMatrix4fv(glGetUniformLocation(batchShaderHandle, "model"), 1, GL_FALSE, glm::value_ptr(model));

		//glActiveTexture(GL_TEXTURE0);
		// IMPORTANT(ck):
		// NOTE(ck): the reason why you set it to 0 is because thats the base texture slot
		// its not expecting the textureID thats only for binding
		// bind textures on corresponding texture units
		int textureSheetLocation = gl->quadProgram.common.textureDiffuse1;
		Texture* textureSheet = GetTexture(gl, batch->sheetTextureIndex);
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(textureSheetLocation, 0);
		glBindTexture(GL_TEXTURE_2D, textureSheet->id); // texture index


		//glUniform1i(glGetUniformLocation(batchShaderHandle, "texture_diffuse1"), 0);
		glBindVertexArray(batch->VAO);
		// (void*)(index_size * pass.index_start)

		bool wireFrame = false;
		int polygonMode = wireFrame ? GL_LINE : GL_FILL;
		glPolygonMode(GL_FRONT_AND_BACK, polygonMode);
		glDrawElements(GL_TRIANGLES, batch->usedIndices, GL_UNSIGNED_INT, (void*)(0));
		glBindVertexArray(0);

		//reset buffer
		//_uNumUsedVertices = 0;
		//_config.iPriority = 0;

		batch->usedIndices = 0;
		batch->vertices_.clear();
	}


	///
	///  [END] Batch Rending 
	///

	///
	/// [BEGIN] Sprite Animation
	///

	void UpdateAnimation(GLSpriteAnimation* animation)
	{
		/*
		 PLAY ()
		{
		BLAH_ASSERT(m_sprite, "No Sprite Assigned!");

		for (int i = 0; i < m_sprite->animations.size(); i++)
		{
			if (m_sprite->animations[i].name == animation)
			{
				if (m_animation_index != i || restart)
				{
					m_animation_index = i;
					m_frame_index = 0;
					m_frame_counter = 0;
				}

				break;
			}
		*/


		// I think play is advancing the animation and saying play it while this is true
		// and update is just updating it so if its not being called nothing will change?

		/*
		 // UPDATE ()
		// only update if we're in a valid state
		bool Animator::in_valid_state() const
		{
			return
				m_sprite &&
				m_animation_index >= 0 &&
				m_animation_index < m_sprite->animations.size() &&
				m_frame_index >= 0 &&
				m_frame_index < m_sprite->animations[m_animation_index].frames.size();
		}


		if (in_valid_state())
		{
			// quick references
			auto& anim = m_sprite->animations[m_animation_index];
			auto& frame = anim.frames[m_frame_index];

			// increment frame counter
			m_frame_counter += Time::delta;

			// move to next frame after duration
			while (m_frame_counter >= frame.duration)
			{
				// reset frame counter
				m_frame_counter -= frame.duration;

				// increement frame, move back if we're at the end
				m_frame_index++;
				if (m_frame_index >= anim.frames.size())
					m_frame_index = 0;
			}
		}
		*/
	}

	///
	/// [END] Sprite Animation
	///

	///
	///	[BEGIN] End Rendering
	///

	void EndRender(OpenGL* gl)
	{
		assert(globalDrawCalls > 0);
		
		gl->batchItems_.clear();
		gl->renderItems_.clear();

		// now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.

		//glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		//glClear(GL_COLOR_BUFFER_BIT);

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
 
	/// =================================================================================
	
	///
	///	2D Renderer Methods
	///

	/// =================================================================================

	void InitRenderer2D(OpenGL* gl)
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
		MonGL::LoadShader(&gl->program, "res/shaders/vert_sprite.glsl", "res/shaders/frag_sprite.glsl", NULL);
		MonGL::LoadShader(&gl->waterProgram.common, "res/shaders/vert_water.glsl", "res/shaders/frag_water.glsl", NULL);
		// TODO(ck): Do not need this now?
		//state->mainShaderID = state->shader.handle;

		// TODO(ck): Need a texture atlas rather than loading all of these
		// textures for the entities
		// index 0 
		AddTexture(gl);

		AddTexture(gl);
		MonGL::Texture* t1 = GetTexture(gl, 1);
		AddTexture(gl);
		MonGL::Texture* t2 = GetTexture(gl, 2);
		AddTexture(gl);
		MonGL::Texture* t3 = GetTexture(gl, 3);
		AddTexture(gl);
		MonGL::Texture* t4 = GetTexture(gl, 4);
		AddTexture(gl);
		MonGL::Texture* t5 = GetTexture(gl, 5);
		AddTexture(gl);
		MonGL::Texture* t6 = GetTexture(gl, 6);
		AddTexture(gl);
		MonGL::Texture* t7 = GetTexture(gl, 7);
		AddTexture(gl);
		MonGL::Texture* t8 = GetTexture(gl, 8);
		AddTexture(gl);
		MonGL::Texture* t9 = GetTexture(gl, 9);
		AddTexture(gl);
		MonGL::Texture* t10 = GetTexture(gl, 10);
		AddTexture(gl);
		MonGL::Texture* t11 = GetTexture(gl, 11);
		AddTexture(gl);
		MonGL::Texture* t12 = GetTexture(gl, 12);
		AddTexture(gl);
		MonGL::Texture* t13 = GetTexture(gl, 13);
		AddTexture(gl);
		MonGL::Texture* t14 = GetTexture(gl, 14);
		AddTexture(gl);
		MonGL::Texture* t15 = GetTexture(gl, 15);
		AddTexture(gl);
		MonGL::Texture* t16 = GetTexture(gl, 16);

		int shaderID = gl->program.handle;
		LoadTexture((char*)"temp1", t1, MonGL::TextureType::Diffuse, true, GetImage(g_Assets, 1));
		LoadTexture((char*)"temp2", t2, MonGL::TextureType::Diffuse, true, GetImage(g_Assets, 2));
		LoadTexture((char*)"temp3", t3, MonGL::TextureType::Diffuse, true, GetImage(g_Assets, 3));
		LoadTexture((char*)"temp4", t4, MonGL::TextureType::Diffuse, true, GetImage(g_Assets, 4));
		LoadTexture((char*)"temp5", t5, MonGL::TextureType::Diffuse, true, GetImage(g_Assets, 5));
		LoadTexture((char*)"temp6", t6, MonGL::TextureType::Diffuse, true, GetImage(g_Assets, 6));
		LoadTexture((char*)"temp7", t7, MonGL::TextureType::Diffuse, true, GetImage(g_Assets, 7));
		LoadTexture((char*)"temp8", t8, MonGL::TextureType::Diffuse, true, GetImage(g_Assets, 8));
		LoadTexture((char*)"temp9", t9, MonGL::TextureType::Diffuse, true, GetImage(g_Assets, 9));
		LoadTexture((char*)"temp10", t10, MonGL::TextureType::Diffuse, true, GetImage(g_Assets, 10));
		LoadTexture((char*)"temp11", t11, MonGL::TextureType::Diffuse, true, GetImage(g_Assets, 11));
		LoadTexture((char*)"temp12", t12, MonGL::TextureType::Diffuse, true, GetImage(g_Assets, 12));
		LoadTexture((char*)"temp13", t13, MonGL::TextureType::Diffuse, true, GetImage(g_Assets, 13));
		LoadTexture((char*)"temp14", t14, MonGL::TextureType::Diffuse, true, GetImage(g_Assets, 14));
		LoadTexture((char*)"temp15", t15, MonGL::TextureType::Diffuse, true, GetImage(g_Assets, 15));
		LoadTexture((char*)"temp16", t16, MonGL::TextureType::Diffuse, true, GetImage(g_Assets, 18));
		
		// Texture Atlases
		AddTexture(gl);
		MonGL::Texture* t17 = GetTexture(gl, 17);
		AddTexture(gl);
		MonGL::Texture* t18 = GetTexture(gl, 18);
		AddTexture(gl);
		MonGL::Texture* t19 = GetTexture(gl, 19);
		LoadTextureFile((char*)"sprite_atlas", t17, GetImage(g_Assets, 31), TextureType::Diffuse, true, true);
		LoadTextureFile((char*)"debug_icons", t18, GetImage(g_Assets, 32), TextureType::Diffuse, true, true);
		LoadTextureFile((char*)"tilemap", t19, GetImage(g_Assets, 16), TextureType::Diffuse, true, true);

		// Get textures loading from file like 3D
// NOTE(ck): first index loaded
//AddTexture(gl);
//unsigned int assetCount = g_Assets->textureAssetCount;
//for (unsigned int i = 1; i < assetCount; ++i)
//{
//	TextureAsset* asset = Mon::GetTextureAsset(g_Assets, i);
//	AddTexture(gl);
//	MonGL::Texture* t = GetTexture(gl, i);
//	LoadTexture(t, asset->type, asset->isPixelArt, GetImage(g_Assets, asset->imageIndex));
//}

		// null at 0 index
		AddBatch(gl);
		// Tilemap and sprite batch
		int batchIndex = AddBatch(gl);
		InitBatch(gl, batchIndex, 1000);
		batchIndex = AddBatch(gl);
		InitBatch(gl, batchIndex, 1000);		



		// Set Blend Function
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	void InitOpenGLBatchMesh(BatchData* data)
	{
		int usedVertices = 0;
		const int quadCount = 2000;
		const int maxVertices = quadCount * 4;
		const int indicesLength = quadCount * 6;

		glGenVertexArrays(1, &data->VAO);
		glBindVertexArray(data->VAO);

		glGenBuffers(1, &data->VBO);
		glBindBuffer(GL_ARRAY_BUFFER, data->VBO);
		glBufferData(GL_ARRAY_BUFFER, maxVertices * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

		glGenBuffers(1, &data->IBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data->IBO);
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

	void InitRenderData2D(RenderData2D* sprite, int size)
	{
		sprite->meshIndex = 1;
		sprite->textureIndex = 1;
		sprite->color = v3(1.0f);
		sprite->pos = {};
	}

	void InitQuad2D(Mesh2D* mesh, v2 pos)
	{
		mesh->id = "2D_QUAD";

		int verticeCount = 4;
		mesh->vertices = new MonGL::Vertex[verticeCount];

		// 2 = 32pixels
		
		float size = 1.0f;
		float tileSize = 32.0f;
		//mesh->vertices[0].position = v3(0.0f, 0.0f, 0.0f);
		mesh->vertices[0].position = v3(pos.x * tileSize, pos.y * tileSize, 0.0f);
		mesh->vertices[0].color = v3(1.0f, 0.0f, 0.0f);
		mesh->vertices[0].texCoords = v2(1.0f, 1.0f);

		//mesh->vertices[1].position = v3((0.0f + 1.0f), (0.0f), 0.0f);
		mesh->vertices[1].position = v3((pos.x + size) * tileSize, pos.y * tileSize, 0.0f);
		mesh->vertices[1].color = v3(1.0f, 0.0, 0.0f);
		mesh->vertices[1].texCoords = v2(1.0f, 0.0f);

		//mesh->vertices[2].position = v3((0.0f + 1.0f), (0.0f + 1.0f), 0.0f);
		mesh->vertices[2].position = v3((pos.x + size) * tileSize, (pos.y + size) * tileSize, 0.0f);
		mesh->vertices[2].color = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[2].texCoords = v2(0.0f, 0.0f);

		//mesh->vertices[3].position = v3((0.0f), (0.0f + 1.0f), 0.0f);
		mesh->vertices[3].position = v3(pos.x * tileSize, (pos.y + size) * tileSize, 0.0f);
		mesh->vertices[3].color = v3(1.0f, 1.0f, 1.0f);
		mesh->vertices[3].texCoords = v2(0.0f, 1.0f);

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
		UploadOpenGLMesh2D(mesh);
		
		



		//glGenVertexArrays(1, &mesh->VAO);
		//glGenBuffers(1, &mesh->VBO);
		//glGenBuffers(1, &EBO);

		//glBindVertexArray(mesh->VAO);
		//glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);

		//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		//glEnableVertexAttribArray(0);
		//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);

		//glEnableVertexAttribArray(1);
		//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));

		//glEnableVertexAttribArray(2);
		//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

		//glBindBuffer(GL_ARRAY_BUFFER, 0);
		//glBindVertexArray(0);

	}

	void InitBatch(BatchData* batch, int tileAmount)
	{
		// TODO(ck): MEMORY - TEST 
		// Initialize outside of method?
		//batch = new BatchData();
		
		// These will be figured out after looping our tilemap and pushing quads
		// TODO(ck): Need to be able to choose amount of vertices and indices
		batch->quadCount = 4096;
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

		InitOpenGLBatchMesh(batch);
	}

	void InitBatch(OpenGL* gl, int batchIndex, int tileAmount)
	{
		BatchData* batch = GetBatch2D(gl, batchIndex);
		InitBatch(batch, tileAmount);
	}

	void FillBatch(BatchData* batch, float sheetSize, int tileSize, float spriteSize, float worldX, float worldY, v2 textureOffset, v2 cameraPos)
	{
		// TODO(ck): Pass Point or integer Rect no floating points
		int tileOffsetX = (int)textureOffset.x;
		int tileOffsetY = (int)textureOffset.y;

		// Update GLSubTexture textCoords here

		v2 topRight = v2(((tileOffsetX + 1) * tileSize) / sheetSize, ((tileOffsetY + 1) * tileSize) / sheetSize);
		v2 topLeft = v2((tileOffsetX * tileSize) / sheetSize, ((tileOffsetY + 1) * tileSize) / sheetSize);
		v2 bottomRight = v2(((tileOffsetX + 1) * tileSize) / sheetSize, (tileOffsetY * tileSize) / sheetSize);
		v2 bottomLeft = v2((tileOffsetX * tileSize) / sheetSize, (tileOffsetY * tileSize) / sheetSize);

#if 0
		// top left
		float u = tileOffsetX / sheetSize;
		float v = tileOffsetY / sheetSize;
		// bottom right
		float u2 = (tileOffsetX + tileSize) / sheetSize;
		float v2_ = (tileOffsetY + tileSize) / sheetSize;

		v4 uvRect = v4(u, v, u2 - u, v2_ - v);
		float left = uvRect.x;
		float right = uvRect.x + uvRect.z;
		float top = uvRect.y;
		float bottom = uvRect.y + uvRect.w;

		v2 tr = v2(right, top);
		v2 tl = v2(left, top);
		v2 br = v2(right, bottom);
		v2 bl = v2(left, bottom);
#endif

		float size = spriteSize * 16.0f; // spriteSize * pixelsPerMeter
		float pixelsPerMeter = 16.0f;

		worldX *= pixelsPerMeter;
		worldY *= pixelsPerMeter;
		float x = worldX;
		float y = worldY;

		// tile coords to world coords
		/*
		worldX *= 16.0f;
		worldY *= 16.0f;
		float x = worldX;
		float y = worldY;
		*/
		// world to screen position
		//v2 cameraCoords = v2(worldX, worldY) - cameraPos;
		//float screenW = 480.0f;
		//float screenH = 270.0f;
		//float x = cameraCoords.x + screenW / 2.0f;
		//float y = cameraCoords.y + screenH / 2.0f;


		Vertex vec0 = {
				v3(x,
				   y,
				   0.0f),
				v3(1.0f, 0.0f, 0.0f),
				bottomLeft
		};
		Vertex vec1 = {
			v3((x + size),
				y,
				0.0f),
			v3(0.0f, 1.0f, 0.0f),
			bottomRight
		};
		Vertex vec2 = {
			v3((x + size),
				(y + size),
				0.0f),
			v3(0.0f, 0.0f, 1.0f),
			topRight
		};
		Vertex vec3 = {
			v3((x),
				(y + size),
				0.0f),
			v3(1.0f, 1.0f, 0.0f),
			topLeft
		};

		batch->usedIndices += 6;
		batch->vertices.push_back(vec0);
		batch->vertices.push_back(vec1);
		batch->vertices.push_back(vec2);
		batch->vertices.push_back(vec3);
	}

	void BindVertices(BatchData* batch)
	{
		glBindVertexArray(batch->VAO);
		glBindBuffer(GL_ARRAY_BUFFER, batch->VBO);
		// Use glMapBuffer instead, if moving large chunks of data > 1MB 
		glBufferSubData(GL_ARRAY_BUFFER, 0, batch->vertices.size() * sizeof(Vertex), &batch->vertices[0]);

		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void DrawBatch(BatchData* batch, CommonProgram* shader, unsigned int textureID, bool wireFrame)
	{
		BindVertices(batch);

		v3 pos = {};
		mat4 model = mat4(1.0f);
		model = glm::translate(model, pos);

		glUniformMatrix4fv(glGetUniformLocation(shader->handle, "model"), 1, GL_FALSE, glm::value_ptr(model));

		glActiveTexture(GL_TEXTURE0);
		glUniform1i(glGetUniformLocation(shader->handle, "image"), 0);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glBindVertexArray(batch->VAO);

		glDrawElements(GL_TRIANGLES, batch->usedIndices, GL_UNSIGNED_INT, (void*)(0));
		glBindVertexArray(0);

		// reset buffer
		batch->usedIndices = 0;
		batch->vertices.clear();
	}

	void DrawObject(CommonProgram* shader, RenderData2D* data, v2 cameraPos)
	{
		mat4 model = mat4(1.0f);
		// tile position is in opengl object position -0.5 to 0.5 vertices
		v3 tilePosition = v3(0.0f, 0.0f, 0.0f);
		

		// IMPORTANT(ck): Move position to camera space
		//tilePosition.x = (data->pos.x - cameraPos.x);
		//tilePosition.y = (data->pos.y - cameraPos.y);
		tilePosition.x = (data->pos.x);
		tilePosition.y = (data->pos.y);

		model = glm::translate(model, tilePosition);
		model = glm::scale(model, v3(1.0f));
		//model = glm::translate(model, v3(0.5f * data->size.x, 0.0f * data->size.y, 0.0f));
		//model = glm::rotate(model, obj->rotation, v3(0.0f, 0.0f, 1.0f));
		//model = glm::translate(model, v3(-0.5f * data->size.x, -0.5f * data->size.y, 0.0f));

		glUniformMatrix4fv(glGetUniformLocation(shader->handle, "model"), 1, GL_FALSE, glm::value_ptr(model));
		
		//glUniform3f(glGetUniformLocation(shader->handle, "spriteColor"), data->color.r, data->color.g, data->color.b);
		Mesh2D* mesh = &g_Assets->quad2D;
		Texture* texture = {};
		//Texture* texture = GetTexture(gl, data->textureIndex);

		glActiveTexture(GL_TEXTURE0);
		glUniform1i(glGetUniformLocation(shader->handle, "image"), 0);
		//glUniform1i(program->textureDiffuse1, 0);
		glBindTexture(GL_TEXTURE_2D, data->textureIndex);

		glBindVertexArray(mesh->VAO);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawElements(GL_TRIANGLES, mesh->indiceCount, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
}