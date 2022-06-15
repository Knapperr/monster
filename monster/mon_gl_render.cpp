#include "mon_gl_render.h"

#include <glad/glad.h>

namespace MonGL
{
	// TODO(ck): TODO(ck): Somehow put into game so we can call from gui
	// can have a getter method that retrieves the globalDrawCalls from here
	int globalDrawCalls = 0;

	void LoadTexture(Texture* texture, TextureType type, bool pixelArtTexture, int shaderID, Image* image)
	{
		LoadTextureFile(texture, image, type, true, pixelArtTexture);
	}

	void LoadTextureFile(Texture* texture, Image* image, TextureType type, bool linearFilter, bool pixelArtTexture)
	{
		texture->type = type;
		// TODO(CK): Clean up
		// set wrap and filter here for now same with internal formats
		if (pixelArtTexture == false)
		{
			texture->wrapS = GL_REPEAT;
			texture->wrapT = GL_REPEAT;
			texture->filterMin = GL_LINEAR_MIPMAP_LINEAR;
			texture->filterMax = GL_LINEAR;
		}
		else
		{
			texture->wrapS = GL_CLAMP_TO_EDGE;
			texture->wrapT = GL_CLAMP_TO_EDGE;
			texture->filterMin = GL_NEAREST;
			texture->filterMax = GL_NEAREST;
			// Linear for 3D and Texture Atlas
			if (linearFilter)
			{
				//texture->filterMin = GL_LINEAR_MIPMAP_LINEAR;
				texture->filterMin = GL_LINEAR;
				texture->filterMax = GL_LINEAR;
			}
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
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, normal));
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
		MonGL::LoadShader(&gl->waterProgram, "res/shaders/vert_water.glsl", "res/shaders/frag_water.glsl", NULL);

		// TODO(ck): MOVE TEXTURE IDS TO data... program can have its own textues too?? 
		// a program can be like a material that can be applied to a mesh so it needs to have its own textures
		gl->waterProgram.textureIndexNormal1 = 6;
		gl->waterProgram.textureIndexNormal2 = 15;
		
		// TODO(ck): Need a texture atlas rather than loading all of these
		// textures for the entities 
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

		AddTexture(gl);
		MonGL::Texture* t17 = GetTexture(gl, 17);

		int shaderID = gl->program.handle;
		LoadTexture(t1, MonGL::TextureType::Diffuse, true, shaderID, GetImage(g_Assets, 1));
		LoadTexture(t2, MonGL::TextureType::Diffuse, true, shaderID, GetImage(g_Assets, 2));
		LoadTexture(t3, MonGL::TextureType::Diffuse, true, shaderID, GetImage(g_Assets, 3));
		LoadTexture(t4, MonGL::TextureType::Diffuse, true, shaderID, GetImage(g_Assets, 4));
		LoadTexture(t5, MonGL::TextureType::Diffuse, false, shaderID, GetImage(g_Assets, 5));
		LoadTexture(t6, MonGL::TextureType::Normal,  false, shaderID, GetImage(g_Assets, 6));
		LoadTexture(t7, MonGL::TextureType::Diffuse, true, shaderID, GetImage(g_Assets, 7));
		LoadTexture(t8, MonGL::TextureType::Diffuse, true, shaderID, GetImage(g_Assets, 8));
		LoadTexture(t9, MonGL::TextureType::Diffuse, true, shaderID, GetImage(g_Assets, 9));
		LoadTexture(t10, MonGL::TextureType::Diffuse, true, shaderID, GetImage(g_Assets, 10));
		LoadTexture(t11, MonGL::TextureType::Diffuse, true, shaderID, GetImage(g_Assets, 11));
		LoadTexture(t12, MonGL::TextureType::Diffuse, true, shaderID, GetImage(g_Assets, 12));
		LoadTexture(t13, MonGL::TextureType::Diffuse, true, shaderID, GetImage(g_Assets, 13));
		LoadTexture(t14, MonGL::TextureType::Diffuse, true, shaderID, GetImage(g_Assets, 14));
		LoadTexture(t15, MonGL::TextureType::Normal,  false, shaderID, GetImage(g_Assets, 15));
		LoadTexture(t16, MonGL::TextureType::Diffuse, true, shaderID, GetImage(g_Assets, 16));
		LoadTexture(t17, MonGL::TextureType::Diffuse, true, shaderID, GetImage(g_Assets, 17));


		// #0 for the 
		AddLight(gl);

		int index = AddLight(gl);
		gl->lights[index].id = "001";
		gl->lights[index].ambient = v3(0.3f);
		gl->lights[index].diffuse = v3(0.8f);
		gl->lights[index].specular = v3(0.3f);
		gl->lights[index].pos = v3(0.0f, 20.0f, 0.0f);
		index = AddLight(gl);
		gl->lights[index].id = "002";
		gl->lights[index].ambient = v3(1.0f);
		gl->lights[index].diffuse = v3(1.0f);
		gl->lights[index].specular = v3(1.0f);
		gl->lights[index].pos = v3(1.0f);

		// Frame buffer
		int screenWidth = 1440;
		int screenHeight = 900;

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
		UseProgram(&program->common, setup);

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

	void InitBoundingBox(RenderData* data)
	{
		data->lineWidth = 2;
		data->color = v3(0.7f, 0.15f, 0.4f);
		data->meshIndex = 5;
		data->visible = true;
	}

	void InitCube(RenderData* data)
	{
		data->visible = true;
		data->scale = v3(1.0f);
		data->meshIndex = 2;
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

	/// 
	/// [END] INIT 
	/// 

	/// 
	/// [BEGIN] begin render 
	/// 

	void BeginRender(OpenGL* gl, Config* config, mat4 projection, mat4 view, int shaderID)
	{
		//glBindFramebuffer(GL_FRAMEBUFFER, gl->buffer.handle);
		glEnable(GL_DEPTH_TEST);

		// TODO(ck): Platform->Renderer->clearColor 
		glClearColor(0.126f, 0.113f, 0.165f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUniformMatrix4fv(glGetUniformLocation(shaderID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(shaderID, "view"), 1, GL_FALSE, glm::value_ptr(view));

		MonGL::ViewPort(&config->viewPort);
		globalDrawCalls = 0;
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
	void ActivateUniforms(CommonProgram* program, RenderSetup setup, Light light, v3 viewPos, int textureID)
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

		//glUniform3fv(glGetUniformLocation(shaderID, "colliderColor"), 1, &data->color[0]);
		glUniform1i(program->collider, false);


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

		ActivateUniforms(&program->common, setup, light, viewPos, textureID);

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
		Light* light = GetLight(gl, 1);
		switch (type)
		{
		case ProgramType::Common:
			ActivateUniforms(&gl->program, setup, *light, viewPos, baseTextureID);
			return gl->program.handle;
		case ProgramType::Water:
			Texture* normal1 = GetTexture(gl, gl->waterProgram.textureIndexNormal1);
			Texture* normal2 =  GetTexture(gl, gl->waterProgram.textureIndexNormal2);
			ActivateUniforms(&gl->waterProgram, programData, setup, viewPos, *light, baseTextureID, normal1->id, normal2->id);
			return gl->waterProgram.common.handle;
		}
	}

	//
	// Public Draw Calls
	
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

	void DrawTerrain(OpenGL* gl, RenderData* data, Camera* camera)
	{
		Mesh* mesh = GetMesh(g_Assets, data->meshIndex);
		Texture* texture = GetTexture(gl, data->textureIndex);
		unsigned int shaderID = gl->program.handle;
		
		Light* light = GetLight(gl, 1);

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
	///	[BEGIN] End Rendering
	///

	void EndRender()
	{
		assert(globalDrawCalls > 0);
		
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



	///
	/// 2D
	///

	BatchData* batch;
	std::vector<Vertex> tileVertices;
	int usedIndices = 0;


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
		//



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

		int shaderID = gl->program.handle;
		LoadTexture(t1, MonGL::TextureType::Diffuse, false, shaderID, GetImage(g_Assets, 1));
		LoadTexture(t2, MonGL::TextureType::Diffuse, false, shaderID, GetImage(g_Assets, 2));
		LoadTexture(t3, MonGL::TextureType::Diffuse, false, shaderID, GetImage(g_Assets, 3));
		LoadTexture(t4, MonGL::TextureType::Diffuse, false, shaderID, GetImage(g_Assets, 4));
		LoadTexture(t5, MonGL::TextureType::Diffuse, false, shaderID, GetImage(g_Assets, 5));
		LoadTexture(t6, MonGL::TextureType::Diffuse, false, shaderID, GetImage(g_Assets, 6));
		LoadTexture(t7, MonGL::TextureType::Diffuse, false, shaderID, GetImage(g_Assets, 7));
		LoadTexture(t8, MonGL::TextureType::Diffuse, false, shaderID, GetImage(g_Assets, 8));
		LoadTexture(t9, MonGL::TextureType::Diffuse, false, shaderID, GetImage(g_Assets, 9));
		LoadTexture(t10, MonGL::TextureType::Diffuse, false, shaderID, GetImage(g_Assets, 10));
		LoadTexture(t11, MonGL::TextureType::Diffuse, false, shaderID, GetImage(g_Assets, 11));
		LoadTexture(t12, MonGL::TextureType::Diffuse, false, shaderID, GetImage(g_Assets, 12));
		LoadTexture(t13, MonGL::TextureType::Diffuse, false, shaderID, GetImage(g_Assets, 13));
		LoadTexture(t14, MonGL::TextureType::Diffuse, false, shaderID, GetImage(g_Assets, 14));

	}

	void InitOpenGLBatchMesh(BatchData* data)
	{
		int usedVertices = 1;
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
		// NOTE(ck): TODO(ck):
		// size should be equal to 1 
		// 1 unit should equal 16 pixels which is the size of a normal quad in monster 2d
		const int MAP_SIZE = 40;
		float x = (float)((MAP_SIZE / 2));
		float y = (float)((MAP_SIZE / 2));
		x *= size;
		y *= size;
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

		sprite->meshIndex = 1;
		sprite->color = v3(1.0f);
		sprite->pos = {};
	}

	void InitQuad2D()
	{
		// This is for the regular meshes
		



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

	void FillBatch(int tileOffsetX, int tileOffsetY, float tileXPos, float tileYPos, int tileSize, v2 cameraPos)
	{		
		/*
		
		
		v2 PlayerLeftTop = {PlayerGroundPointX - 0.5f*MetersToPixels*LowEntity->Width,
                PlayerGroundPointY - 0.5f*MetersToPixels*LowEntity->Height};
        v2 EntityWidthHeight = {LowEntity->Width, LowEntity->Height};
		
		        {
        DrawRectangle(Buffer,
                        PlayerLeftTop,
                        PlayerLeftTop + MetersToPixels*EntityWidthHeight,
                        PlayerR, PlayerG, PlayerB);
			}
		*/


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
		// TODO(ck): The tileXpos and tileYPos need to be multiplied by the tilesize to get the proper size
		
		/*
		June 05 2022 
		I think I just unlocked a realization. these are vertices NOT positions in 
		the real game world. these are for drawing. you need to remember that these
		are rendering positions. this HAS to be the same as handmade at least it 
		seems he doesn't do the math like this until rendering time which makes sense
		maybe we can do this!!! 
		vertex positions != world positions remember that
		
		I think in the 3d the model matrix is taking care of this for us
		although i probably need to have some kind of map positions like 
		in handmade?
		*/

		// Create vertices from the middle of the world
		/*
		* 
		* IMPORTANT(ck): June 5th 2022
		I think this is wrong. I am meant to be building my map beforehand starting like this
		then when I create my vertices here I won't have to do this it will be set up
		Lets try it in the 3d first maybe?
		*/
		const int MAP_SIZE = 40;
		float x = (float)(tileXPos - (MAP_SIZE / 2));
		float y = (float)(tileYPos - (MAP_SIZE / 2));
		x = x - cameraPos.x;
		y = y - cameraPos.y;
		tileSize = 1;
		Vertex vec0 = {
			v3(x, y, 0.0f),
			v3(1.0f, 0.0f, 0.0f),
			v2(bottomLeftX, bottomLeftY)	
		};

		Vertex vec1 = {
			v3(x + tileSize, y, 0.0f),
			v3(0.0f, 1.0f, 0.0f),
			v2(bottomRightX, bottomRightY)
		};

		Vertex vec2 = {
			v3(x + tileSize, y + tileSize, 0.0f),
			v3(0.0f, 0.0f, 1.0f),
			v2(topRightX, topRightY)
		};

		Vertex vec3 = {
			v3(x, y + tileSize, 0.0f),
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

		glBindVertexArray(batch->VAO);
		glBindBuffer(GL_ARRAY_BUFFER, batch->VBO);
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
		
		mat4 model = mat4(1.0f);

		glUniformMatrix4fv(glGetUniformLocation(shader->handle, "model"), 1, GL_FALSE, glm::value_ptr(model));

		glActiveTexture(GL_TEXTURE0);
		glUniform1i(glGetUniformLocation(shader->handle, "imageArray"), 0);
		glUniform1i(glGetUniformLocation(shader->handle, "diffuse_layer"), 0);
		glBindTexture(GL_TEXTURE_2D_ARRAY, textureID);

		Mesh* mesh = GetMesh(g_Assets, sprite->meshIndex);
		glBindVertexArray(mesh->VAO);
		glDrawElements(GL_TRIANGLES, 9600, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	void DrawObject(CommonProgram* shader, RenderData2D* data)
	{
		//glUseProgram(shader->id);

		mat4 model = mat4(1.0f);
		v3 tilePosition = {};
		tilePosition.x = data->pos.x;
		tilePosition.y = data->pos.y;
		model = glm::translate(model, tilePosition);

		//model = glm::translate(model, v3(0.5f * data->size.x, 0.0f * data->size.y, 0.0f));
		//model = glm::rotate(model, obj->rotation, v3(0.0f, 0.0f, 1.0f));
		//model = glm::translate(model, v3(-0.5f * data->size.x, -0.5f * data->size.y, 0.0f));


		glUniformMatrix4fv(glGetUniformLocation(shader->handle, "model"), 1, GL_FALSE, glm::value_ptr(model));
		
		//glUniform3f(glGetUniformLocation(shader->handle, "spriteColor"), data->color.r, data->color.g, data->color.b);
		Mesh2D* mesh = &g_Assets->quad2D;
		Texture* texture = {};
		//Texture* texture = GetTexture(gl, data->textureIndex);

		glActiveTexture(GL_TEXTURE0);
		//glUniform1i(program->textureDiffuse1, 0);
		glBindTexture(GL_TEXTURE_2D, data->textureIndex);
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, texture->id);

		glBindVertexArray(mesh->VAO);

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
		int mapWidthHeight = 40;
		v3 basePos = {};
		basePos.x = 40 / 2;
		basePos.y = 40 / 2;
		mat4 model = mat4(1.0f);
		//model = glm::translate(model, basePos);

		glUniformMatrix4fv(glGetUniformLocation(shader->handle, "model"), 1, GL_FALSE, glm::value_ptr(model));



		//glActiveTexture(GL_TEXTURE0);
		// IMPORTANT(ck):
		// NOTE(ck): the reason why you set it to 0 is because thats the base texture slot
		// its not expecting the textureID thats only for binding
		glUniform1i(glGetUniformLocation(shader->handle, "image"), 0);
		glBindTexture(GL_TEXTURE_2D, textureID);

		glBindVertexArray(batch->VAO);
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

		glBindVertexArray(batch->VAO);
		glBindBuffer(GL_ARRAY_BUFFER, batch->VBO);
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

		glBindVertexArray(batch->VAO);
		// (void*)(index_size * pass.index_start)
		glDrawElements(GL_TRIANGLES, usedIndices, GL_UNSIGNED_INT, (void*)(2 * 0));
		glBindVertexArray(0);

		//reset buffer
		//_uNumUsedVertices = 0;
		//_config.iPriority = 0;

		// clear batch
	}

}