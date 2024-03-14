#include "mon_gl_render.h"

// TODO(ck): NO STL
#include <algorithm>

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
			texture->wrapS = GL_REPEAT;
			texture->wrapT = GL_REPEAT;
			//texture->wrapS = GL_CLAMP_TO_EDGE;(clamp to edge not working in 2d. causes uvs to be completely wrong?)
			//texture->wrapT = GL_CLAMP_TO_EDGE;
			texture->filterMin = GL_NEAREST;
			texture->filterMax = GL_NEAREST;

			// Linear for pixel art smoothing
			if (linearFilter)
			{
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
			InitTexture(texture, image->width, image->height, image->nrChannels, image->data);
		}
		else
		{
			// TODO(ck): no strings
			std::string msg = "failed to load texture. the image data is not loaded...";
			Mon::Log::warn(msg.c_str());
		}
	}

	void UploadGLMesh(Mesh* mesh)
	{
		// NOTE(ck): 
		// Could check for compatibility before using DSA... have legacy methods
		// that init all resources and switch the draw call?


		// set up vertex data (and buffer(s)) and configure vertex attributes
// ------------------------------------------------------------------
		glCreateBuffers(1, &mesh->VBO); // NOTE(ck): NOT 100% !!! "this is glGenBuffers and glBindBuffer in one call"
		// from Jasper: glGenBuffers does not actually create the buffer, it just gives you the next free buffer ID
		//	calling glBindBuffer for the first time is what actually allocates and creates the buffer under the hood
		//  Thats the part of glBindBuffer that glCreateBuffers replaces. It allocates a buffer ID and then triggers the internal creation without
		//	binding it
		//glNamedBufferStorage(mesh->VBO, sizeof(Vertex) * (verticeCount), vertices, GL_DYNAMIC_STORAGE_BIT);
		glNamedBufferStorage(mesh->VBO, sizeof(Vertex3D) * (mesh->verticeCount), mesh->vertices, GL_DYNAMIC_STORAGE_BIT);

		glCreateBuffers(1, &mesh->IBO);
		glNamedBufferStorage(mesh->IBO, sizeof(GLushort) * mesh->indiceCount, mesh->indices, GL_DYNAMIC_STORAGE_BIT);
		//glNamedBufferStorage(mesh->IBO, sizeof(GLushort)*indiceCount, indices, GL_DYNAMIC_STORAGE_BIT);

		glCreateVertexArrays(1, &mesh->VAO);
		                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            
		glVertexArrayVertexBuffer(mesh->VAO, 0, mesh->VBO, 0, sizeof(Vertex3D));
		glVertexArrayElementBuffer(mesh->VAO, mesh->IBO);

		glEnableVertexArrayAttrib(mesh->VAO, 0);
		glVertexArrayAttribFormat(mesh->VAO, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex3D, position));
		glVertexArrayAttribBinding(mesh->VAO, 0, 0);

		glEnableVertexArrayAttrib(mesh->VAO, 1);
		glVertexArrayAttribFormat(mesh->VAO, 1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex3D, normal));
		glVertexArrayAttribBinding(mesh->VAO, 1, 0);

		glEnableVertexArrayAttrib(mesh->VAO, 2);
		glVertexArrayAttribFormat(mesh->VAO, 2, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex3D, texCoords));
		glVertexArrayAttribBinding(mesh->VAO, 2, 0);

		// ADD REST OF ATTRIBUTES


		glBindVertexArray(mesh->VAO);

		std::string vaoName = mesh->name + " VAO";
		std::string vboName = mesh->name + " VBO";
		std::string iboName = mesh->name + " IBO";
		glObjectLabel(GL_VERTEX_ARRAY, mesh->VAO, -1, vaoName.c_str());
		glObjectLabel(GL_BUFFER, mesh->VBO, -1, vboName.c_str());
		glObjectLabel(GL_BUFFER, mesh->IBO, -1, iboName.c_str());

		glBindVertexArray(0);

		// NOTE(ck): Keep verts in memory for now
		/*delete[] mesh->vertices;
		delete[] mesh->indices;
		mesh->vertices = nullptr;
		mesh->indices = nullptr;*/
	}

	void UploadBatchMesh(Batch* batch, int maxVertices, int indicesLength)
	{
		// TODO(ck): TEMP remove
		batch->sheetTextureIndex = 7;

		batch->indices = new uint32_t[indicesLength];
		int offset = 0;
		// preload indices. this is for quads so we know the indices ahead of time
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

		glCreateBuffers(1, &batch->VBO);
		glNamedBufferStorage(batch->VBO, sizeof(BatchVertex3D) * maxVertices, nullptr, GL_DYNAMIC_STORAGE_BIT);

		glCreateBuffers(1, &batch->IBO);
		glNamedBufferStorage(batch->IBO, sizeof(uint32_t) * indicesLength, batch->indices, GL_DYNAMIC_STORAGE_BIT);

		glCreateVertexArrays(1, &batch->VAO);

		glVertexArrayVertexBuffer(batch->VAO, 0, batch->VBO, 0, sizeof(BatchVertex3D));
		glVertexArrayElementBuffer(batch->VAO, batch->IBO);

		glEnableVertexArrayAttrib(batch->VAO, 0);
		glVertexArrayAttribFormat(batch->VAO, 0, 3, GL_FLOAT, GL_FALSE, offsetof(BatchVertex3D, position));
		glVertexArrayAttribBinding(batch->VAO, 0, 0);

		glEnableVertexArrayAttrib(batch->VAO, 1);
		glVertexArrayAttribFormat(batch->VAO, 1, 3, GL_FLOAT, GL_FALSE, offsetof(BatchVertex3D, normal));
		glVertexArrayAttribBinding(batch->VAO, 1, 0);

		glEnableVertexArrayAttrib(batch->VAO, 2);
		glVertexArrayAttribFormat(batch->VAO, 2, 2, GL_FLOAT, GL_FALSE, offsetof(BatchVertex3D, texCoords));
		glVertexArrayAttribBinding(batch->VAO, 2, 0);

		glEnableVertexArrayAttrib(batch->VAO, 3);
		glVertexArrayAttribFormat(batch->VAO, 3, 3, GL_FLOAT, GL_FALSE, offsetof(BatchVertex3D, worldPosition));
		glVertexArrayAttribBinding(batch->VAO, 3, 0);

		
		//glBindVertexArray(data->VAO);


		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

	}

	void SetBlockSize(UniformObject* ubo, int blockSize, int blockCount)
	{
		ubo->blockSize = blockSize;
		ubo->totalSize = blockSize * blockCount;
	}

	void InitUniformObject(UniformObject* ubo, int blockSize, int blockCount)
	{
		// TODO(ck):
		// Make this buffer larger and just keep track of the used size for sending to OpenGL.
		SetBlockSize(ubo, blockSize, blockCount);
		// TODO(ck): Memory management -- allocate with renderer arena
		ubo->buffer = (GLubyte*)malloc(ubo->totalSize);
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
		texture->name = "Cubemap";
		glGenTextures(1, &texture->id);
		glBindTexture(GL_TEXTURE_CUBE_MAP, texture->id);

		for (unsigned int i = 0; i < 6; ++i)
		{
			// TODO(ck): NEED TO LOOKUP CUBEMAP IMAGES
			// HAVE A CUBEMAP ASSET THAT IS SET IN THE FILE??? THAT WAY THE GUI CAN SET THE 
			// INDEX TO THE TEXTURE AND SAVE IT SO ITS NOT A BIG DEAL... JUST TAG IMAGES 
			Image* img = GetImage(g_Assets, 23 + i);

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

	void InitRenderer(OpenGL* gl, int entityCount)
	{
		Mon::Log::print("Init renderer...\n===========================================");

		/*
		TODO(ck): Load from a configuration file
		should only load meshses in current chunk or level
		*/
		gl->program = {};
		gl->debugProgram = {};
		gl->waterProgram = {};
		gl->cubemapProgram = {};
		
		// TODO(ck): Remove 
		MonGL::LoadShader(&gl->program, "res/shaders/vert_colors.glsl", "res/shaders/frag_colors.glsl", NULL);
		MonGL::LoadShader(&gl->frameBufferProgram, "res/shaders/vert_screen.glsl", "res/shaders/frag_screen.glsl", NULL);
		MonGL::LoadShader(&gl->debugProgram, "res/shaders/vert_debug.glsl", "res/shaders/frag_debug.glsl", NULL);
		MonGL::LoadShader(&gl->quadProgram, "res/shaders/vert_quad_batch.glsl", "res/shaders/frag_quad_batch.glsl", NULL);
		MonGL::LoadShader(&gl->waterProgram, "res/shaders/vert_water.glsl", "res/shaders/frag_water.glsl", NULL);
		MonGL::LoadShader(&gl->cubemapProgram, "res/shaders/vert_cubemap.glsl", "res/shaders/frag_cubemap.glsl", NULL);
		

		/*
		IMPORTANT(ck): I think I needed this so I could actually set the ModelBlock
		render doc can't see it without doing this?
		

		You need to bind the block indexes so that glBindBufferRange gets the correct index
		Camera works without binding but it is at index 0 for some reason...
		*/
		unsigned int cameraMatricesBlockIndex = glGetUniformBlockIndex(gl->program.handle, "CameraBlock");
		glUniformBlockBinding(gl->program.handle, cameraMatricesBlockIndex, 0);
		unsigned int modelMatricesBlockIndex = glGetUniformBlockIndex(gl->program.handle, "ModelBlock");
		glUniformBlockBinding(gl->program.handle, modelMatricesBlockIndex, 1);

		gl->ubo = {};
		int blockSize;
		glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &blockSize);
		// TODO(ck): Max entity size? not entity count we don't want this tied to our entity count inside because we will have to reallocate 
		// the ubo each time
		int actualDrawnModelCount = 5;
		int totalBlocks = actualDrawnModelCount + 2; // + 2 for the camera block and the extra model block at the front
		//int totalBlocks = entityCount + 1;
		InitUniformObject(&gl->ubo, blockSize, totalBlocks);
		if (gl->ubo.buffer == nullptr)
		{
			printf("Failed to allocate for uniform buffer");
		}

		glCreateBuffers(1, &gl->ubo.gl_handle);
		// need to bind to something for glObjectLabel to work
		//glBindBuffer(GL_UNIFORM_BUFFER, state.ubo.gl_handle);
		std::string uboName = "uniform buffer";
		glObjectLabel(GL_BUFFER, gl->ubo.gl_handle, -1, uboName.c_str());

		glNamedBufferStorage(gl->ubo.gl_handle, gl->ubo.totalSize, nullptr, GL_DYNAMIC_STORAGE_BIT);
		glNamedBufferSubData(gl->ubo.gl_handle, 0, gl->ubo.totalSize, nullptr);


		// line buffer
		int vertSize = 2048;
		int indiceSize = 2048 * 4;
		InitLineBuffer(&gl->lineBuffer, vertSize, indiceSize);
		InitGLBuffer(&gl->lineBuffer);

		// TODO(ck): MOVE TEXTURE IDS TO data... program can have its own textues too?? 
		// a program can be like a material that can be applied to a mesh so it needs to have its own textures
		gl->waterProgram.textureIndexNormal1 = 6;
		gl->waterProgram.textureIndexNormal2 = 15;
		
		Mon::Log::print("Loading textures\n=============================================");
		unsigned int assetCount = g_Assets->textureAssetCount;
		for (unsigned int i = 0; i < assetCount; ++i)
		{
			TextureAsset* asset = Mon::GetTextureAsset(g_Assets, i);
			AddTexture(gl);
			MonGL::Texture* t = GetTexture(gl, i);
			//std::string name = std::string(asset->name);
			
			// TODO(ck): remove std::string
			Mon::Log::print(asset->name.c_str());
			LoadTexture(asset->name, t, asset->type, asset->isPixelArt, GetImage(g_Assets, asset->imageIndex));
		}

		// TODO(ck): IMPORTANT(ck): FIX THIS 
		// NOTE(ck): using image indexes [24 to 29] need a better way to handle this
		int newTextIndex = AddTexture(gl);
		MonGL::Texture* t = GetTexture(gl, newTextIndex);
		gl->cubemapProgram.skyboxTextureIndex = newTextIndex;
		LoadCubeMapTexture(t);
		gl->cubemap = {};
		LoadCubemap(&gl->cubemap);


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
		int batchIndex = AddBatch(gl);
		InitBatch(gl, batchIndex);

		// Init Animations
		{
			// TODO(ck): Remove this start it at 0 no need for empty space
			// null at index 0
			GLSpriteAnimator* glAnimator = &gl->spriteAnimators[0];
			glAnimator = {};
			gl->spriteAnimatorCount++;

			// Add animator method
			GLSpriteAnimator* glAnimator1 = &gl->spriteAnimators[1];
			glAnimator1->animationIndex = 1;
			gl->spriteAnimatorCount++;

			const int animationCount = 1;
			int frameCount = 4;

			const int subTextureCount = 4;
			float sheetSize = 256.0f;
			int tileSize = 32;
			int tileOffsetX = 5;
			int tileOffsetY = 7;

			float uvPadding = 0.0f;

			for (int i = 0; i < animationCount; ++i)
			{
				// Add animation method
				GLSpriteAnimation* ani = &gl->spriteAnimators[1].animations[i];
				ani->textureAtlasIndex = 0;
				glAnimator1->animationCount++;

				ani->frameIndex = 0;
				ani->frameCounter = 0;

				for (int frameIndex = 0; frameIndex < frameCount; ++frameIndex)
				{
					// Add frame method
					GLFrame* frame = &ani->frames[frameIndex];
					ani->frameCount++;

					if (frameIndex == 1)
					{
						tileOffsetX = 5;
						tileOffsetY = 7;
					}
					else if (frameIndex == 2)
					{
						tileOffsetX = 7;
					}
					else if (frameIndex == 3)
					{
						tileOffsetX = 0;
					}

					frame->subTexture = {};
					frame->subTexture.width = 32;
					frame->subTexture.height = 32;
					frame->subTexture.tileSize = 32;
					//frame->subTexture.texCoords[0] = v2(((tileOffsetX * tileSize) / sheetSize), ((tileOffsetY * tileSize) / sheetSize)); // bottom left
					//frame->subTexture.texCoords[1] = v2((((tileOffsetX + 1) * tileSize) / sheetSize), ((tileOffsetY * tileSize) / sheetSize)); // bottom right
					//frame->subTexture.texCoords[2] = v2((((tileOffsetX + 1) * tileSize) / sheetSize), (((tileOffsetY + 1) * tileSize) / sheetSize)); // top right
					//frame->subTexture.texCoords[3] = v2(((tileOffsetX * tileSize) / sheetSize), (((tileOffsetY + 1) * tileSize) / sheetSize)); // top left 

					frame->subTexture.texCoords[0] = v2(((tileOffsetX * tileSize) / sheetSize), ((tileOffsetY * tileSize) / sheetSize)); // bottom left
					frame->subTexture.texCoords[1] = v2((((tileOffsetX + 1) * tileSize) / sheetSize), ((tileOffsetY * tileSize) / sheetSize)); // bottom right
					frame->subTexture.texCoords[2] = v2((((tileOffsetX + 1) * tileSize) / sheetSize), (((tileOffsetY + 1) * tileSize) / sheetSize)); // top right
					frame->subTexture.texCoords[3] = v2(((tileOffsetX * tileSize) / sheetSize), (((tileOffsetY + 1) * tileSize) / sheetSize)); // top left 

					frame->duration = 5.0f;
				}
			}

			// Walk animation

			// Add animator method
			GLSpriteAnimator* glAnimatorWalk = &gl->spriteAnimators[1];
			glAnimatorWalk->animationIndex = 1;
			gl->spriteAnimatorCount++;

			frameCount = 10;

			//sheet size 
			for (int i = 0; i < animationCount; ++i)
			{
				// Add animation method
				GLSpriteAnimation* ani = &gl->spriteAnimators[2].animations[i];
				ani->textureAtlasIndex = 0;
				glAnimatorWalk->animationCount++;

				ani->frameIndex = 0;
				ani->frameCounter = 0;
				tileOffsetY = 5;
				tileOffsetX = 0;

				for (int frameIndex = 0; frameIndex < frameCount; ++frameIndex)
				{
					// Add frame method
					GLFrame* frame = &ani->frames[frameIndex];
					ani->frameCount++;

					frame->subTexture = {};
					frame->subTexture.height = 32;
					frame->subTexture.tileSize = 32;

					frame->subTexture.texCoords[0] = v2((tileOffsetX * tileSize) / sheetSize, (tileOffsetY * tileSize) / sheetSize); // bottom left
					frame->subTexture.texCoords[1] = v2(((tileOffsetX + 1) * tileSize) / sheetSize, (tileOffsetY * tileSize) / sheetSize); // bottom right
					frame->subTexture.texCoords[2] = v2(((tileOffsetX + 1) * tileSize) / sheetSize, ((tileOffsetY + 1) * tileSize) / sheetSize); // top right
					frame->subTexture.texCoords[3] = v2((tileOffsetX * tileSize) / sheetSize, ((tileOffsetY + 1) * tileSize) / sheetSize); // top left 
					frame->duration = 0.2f;

					tileOffsetX++;
				}
			}

		} // Animations 

		// Uniform buffer
		// configure a uniform buffer object
		// ---------------------------------
		// first. We get the relevant block indices
		//unsigned int uniformBlockIndexQuad = glGetUniformBlockIndex(gl->quadProgram.common.handle, "WorldMatrices");
		//// then we link each shader's uniform block to this uniform binding point
		//glUniformBlockBinding(gl->quadProgram.common.handle, uniformBlockIndexQuad, 0);

		//// Now actually create the buffer
		////glGenBuffers(1, &gl->ubo);
		////glBindBuffer(GL_UNIFORM_BUFFER, gl->ubo);
		////glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), NULL, GL_DYNAMIC_DRAW);
		////// define the range of the buffer that links to a uniform binding point
		//////glBindBufferRange(GL_UNIFORM_BUFFER, 0, gl->ubo, 0, 2 * sizeof(glm::mat4));
		////glBindBufferBase(GL_UNIFORM_BUFFER, 0, gl->ubo);
		

		// Frame buffer
		int screenWidth = 960;
		int screenHeight = 540;
		glGenFramebuffers(1, &gl->buffer.handle);
		glBindFramebuffer(GL_FRAMEBUFFER, gl->buffer.handle);

		glGenTextures(1, &gl->textureColourbuffer.handle);
		glBindTexture(GL_TEXTURE_2D, gl->textureColourbuffer.handle);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gl->textureColourbuffer.handle, 0);

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
	/// [BEGIN] begin render 
	/// 

	void BeginRender(OpenGL* gl, Config* config, mat4 projection, mat4 view, int shaderID)
	{
		// only call glViewPort when the size changes
		//MonGL::ViewPort(&config->viewPort);
		globalDrawCalls = 0;
	}

	///
	/// [END] BEGIN RENDER
	/// 

	///
	/// [BEGIN] Debug Drawing
	///



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

		Light* light = GetLight(gl, 0);
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
	// Main Render
	//

	bool BackToFrontByViewSpace(const RenderItem& left, const RenderItem& right)
	{
		return left.viewZ < right.viewZ;
	}

	bool FrontToBackByViewSpace(const RenderItem& left, const RenderItem& right)
	{
		return left.viewZ > right.viewZ;
	}

	void SortTransparentItems(std::vector<RenderItem>& items)
	{
		std::sort(items.begin(), items.end(), BackToFrontByViewSpace);

		return;
	}

	void SortOpaqueItems(std::vector<RenderItem>& items)
	{
		std::sort(items.begin(), items.end(), FrontToBackByViewSpace);

		return;
	}

	void Render(OpenGL* gl, Camera* camera, int gridTexture, int gridMesh, mat4 projection, mat4 view, bool drawDebug)
	{
		int camBlockIndex = 0; // these are the indexes into the ubo such as CamBlock{}; ModelBlock{};
		int modelBlockIndex = 1;

		// BeginRender 
		glNamedBufferSubData(gl->ubo.gl_handle, 0, gl->ubo.totalSize, gl->ubo.buffer);
		
		glEnable(GL_DEPTH_TEST);
		//glEnable(GL_CULL_FACE);
		//glClearColor(state.clearColour.r, state.clearColour.g, state.clearColour.b, 1.0f);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		unsigned int vec3AlignSize = 16;
		unsigned int camBlockSize = (sizeof(glm::mat4) * 2) + (vec3AlignSize * 2);
		glBindBufferRange(GL_UNIFORM_BUFFER, camBlockIndex, gl->ubo.gl_handle, 0, camBlockSize);


		// Draw terrain
		// Refactor the terrain after shaders next ground task?
		
		// Bind the "default" model matrix
		int defaultModelBlockOffset = 1;
		glBindBufferRange(GL_UNIFORM_BUFFER, modelBlockIndex, gl->ubo.gl_handle, gl->ubo.blockSize * defaultModelBlockOffset, sizeof(glm::mat4) + sizeof(glm::vec4));
		MonGL::DrawTerrain(gl, gridTexture, gridMesh, camera);

		// Draw models
		for (int i = 0; i < gl->opaqueItems.size(); ++i)
		{
			Mesh mesh = g_Assets->meshes[gl->opaqueItems[i].meshIndex];
			Texture* texture = GetTexture(gl, gl->opaqueItems[i].textureIndex);
			glBindVertexArray(mesh.VAO);
			//glUniform1i(glGetUniformLocation(gl->program.handle, "texture_diffuse1"), 0);
			//glBindTexture(GL_TEXTURE_2D, texture->id);
			// Indexing into textures with a shuffled array of indexes going 0 - 10
			
			glBindTextureUnit(0, texture->id);

			glBindBufferRange(GL_UNIFORM_BUFFER, modelBlockIndex, gl->ubo.gl_handle, gl->ubo.blockSize * gl->opaqueItems[i].uniformBufferOffset, sizeof(glm::mat4) + sizeof(glm::vec4));
			glDrawElements(GL_TRIANGLES, mesh.indiceCount, GL_UNSIGNED_SHORT, nullptr);

			globalDrawCalls++;
		}
		glBindVertexArray(0);


		// sort opaque items
		// sort transparent items
		// sort batch items???? <--- how do i combine with transparent items

		// bindFrameBuffer(fbo)

		// Draw the opaque items
		// gl->opaqueItems 
		
		// Draw the transparent items
		//gl->transparentItems

		// Draw the batch items
		// gl->batchItems

		// bindFramebuffer(0) // back to default buffer

		// RenderDebug()
		// draw the lineBuffer for debug items

		// Reset the buffers and other things like debug drawing

		// must always disable depth and cull tests before drawing the batch
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);

		if (drawDebug)
		{
			// maybe i need to disable depth
			//glEnable(GL_BLEND);
			// debugProgram.handle;
			glUseProgram(gl->debugProgram.handle);

			//glClearColor(state.clearColour.r, state.clearColour.g, state.clearColour.b, 1.0f);
			//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			//glEnable(GL_MULTISAMPLE);
			//glm::mat4 comboMat = projection * view;
			//glm::mat4 toWorldFromClip = glm::inverse(comboMat);
			glUniformMatrix4fv(glGetUniformLocation(gl->debugProgram.handle, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
			glUniformMatrix4fv(glGetUniformLocation(gl->debugProgram.handle, "view"), 1, GL_FALSE, glm::value_ptr(view));

			// update the aabb batch
			glBindVertexArray(gl->lineBuffer.VAO);

			// upload the batch and reset its offsets and lastvertice and lastindice indexes
			glNamedBufferSubData(gl->lineBuffer.VBO, 0, sizeof(DebugVertex) * (gl->lineBuffer.lastVerticeIndex), gl->lineBuffer.vertices);
			glNamedBufferSubData(gl->lineBuffer.IBO, 0, sizeof(unsigned int short) * (gl->lineBuffer.lastIndiceIndex), gl->lineBuffer.indices);

			glDrawElements(GL_LINES, gl->lineBuffer.usedIndices, GL_UNSIGNED_SHORT, nullptr);

			glBindVertexArray(0);
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

	void DrawTerrain(OpenGL* gl, int gridTexture, int gridMesh, Camera* camera)
	{
		Mesh* mesh = GetMesh(g_Assets, gridMesh);
		Texture* texture = GetTexture(gl, gridTexture);
		unsigned int shaderID = gl->program.handle;
		
		Light* light = GetLight(gl, 0);

		glUniform1f(glGetUniformLocation(shaderID, "texCoordScale"), 1.0f);

		glUniform3fv(glGetUniformLocation(shaderID, "light.pos"), 1, &light->pos[0]);
		glUniform3fv(glGetUniformLocation(shaderID, "light.ambient"), 1, &light->ambient[0]);
		glUniform3fv(glGetUniformLocation(shaderID, "light.diffuse"), 1, &light->diffuse[0]);
		glUniform3fv(glGetUniformLocation(shaderID, "light.specular"), 1, &light->specular[0]);

		v3 ambient = v3(0.1f);
		v3 diffuse = v3(1.0f);
		v3 specular = v3(0.2f);
		glUniform3fv(glGetUniformLocation(shaderID, "material.ambient"), 1, &ambient[0]);
		glUniform3fv(glGetUniformLocation(shaderID, "material.diffuse"), 1, &diffuse[0]);
		glUniform3fv(glGetUniformLocation(shaderID, "material.specular"), 1, &specular[0]);
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

		//mat4 matModel = mat4(1.0f);
		//glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, glm::value_ptr(matModel));
		glBindVertexArray(mesh->VAO);

		//int polygonMode = data->wireFrame ? GL_LINE : GL_FILL;
		int polygonMode = GL_FILL;
		glPolygonMode(GL_FRONT_AND_BACK, polygonMode);

		glDrawElements(GL_TRIANGLES, mesh->indiceCount, GL_UNSIGNED_SHORT, 0);

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
		int quadCount = 2000;
		int maxVertices = quadCount * 4;
		int indicesLength = quadCount * 6;

		// init batch mesh
		Batch* batch = GetBatch(gl, batchIndex);
		UploadBatchMesh(batch, maxVertices, indicesLength);
	}

	bool SortBatchItems(BatchItem& left, BatchItem& right)
	{
		float distToCameraA = left.worldPos.z - left.cameraZ;
		float distToCameraB = right.worldPos.z - right.cameraZ;
		return (distToCameraA < distToCameraB);
	}

	void SortBatch(std::vector<BatchItem>& batchItems)
	{
		assert(batchItems.size() > 0);
		// TODO(ck): Hack for now put the camera position inside of the batch item
		std::sort(batchItems.begin(), batchItems.end(), SortBatchItems);

		// std::qsort(batchItems, batchItems.size(), sizeof(BatchItem), sortBatchItems);
		return;
	}

	void FillBatch(Batch* batch, float posX, float posY, float posZ, int texOffsetX, int texOffsetY, int tileSize)
	{
		float textureSheetSize = 256.0f;
		posX = posX - 0.5f;
		posY = posY - 0.5f;
		posZ = posZ + 0.5f;
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

	void FillBatch(Batch* batch, float posX, float posY, float posZ, float camX, float camY, float camZ, GLSubTexture* subTexture, int tileSize)
	{

		float textureSheetSize = 256.0f;
		float vertSize = 1.0f;

		posX = posX - 0.5f;
		posY = posY - 0.5f;
		posZ = posZ + 0.5f;

#if 1
		//Vertex3D vec0 = {
		//	v3(posX, posY, posZ),
		//	v3(1.0f, 0.0f, 0.0f),
		//	subTexture->texCoords[0]
		//};

		//Vertex3D vec1 = {
		//	v3((posX + vertSize), posY, posZ),
		//	v3(1.0f, 1.0f, 1.0f),
		//	subTexture->texCoords[1]
		//};

		//Vertex3D vec2 = {
		//	v3((posX + vertSize), (posY + vertSize), (posZ)),
		//	v3(1.0f, 1.0f, 1.0f),
		//	subTexture->texCoords[2]
		//};

		//Vertex3D vec3 = {
		//	v3(posX, (posY + vertSize), posZ),
		//	v3(1.0f, 1.0f, 1.0f),
		//	subTexture->texCoords[3]
		//};

		/*
		* rotating the vertices before sending to gpu so we do not have to have a unique batch shader?
			https://stackoverflow.com/questions/63205847/c-opengl-im-trying-to-rotate-a-group-of-vertices-which-itll-simulate-a
		*/

		BatchVertex3D batchvec0 = {
			v3(posX, posY, posZ),
			v3(1.0f, 0.0f, 0.0f),
			subTexture->texCoords[0],
			v3(posX, posY, posZ)
		};

		BatchVertex3D batchvec1 = {
			//v3((posX), posY, posZ + vertSize),
			v3((posX + vertSize), posY, posZ),
			v3(1.0f, 1.0f, 1.0f),
			subTexture->texCoords[1],
			v3(posX, posY, posZ)
		};

		BatchVertex3D batchvec2 = {
			//v3((posX + vertSize), (posY), (posZ + vertSize)),
			v3((posX + vertSize), (posY + vertSize), (posZ)),
			v3(1.0f, 1.0f, 1.0f),
			subTexture->texCoords[2],
			v3(posX, posY, posZ)
		};

		BatchVertex3D batchvec3 = {
			//v3(posX, (posY), posZ + vertSize),
			v3(posX, (posY + vertSize), posZ),
			v3(1.0f, 1.0f, 1.0f),
			subTexture->texCoords[3],
			v3(posX, posY, posZ)
		};

#endif

#if 0
		batch->vertices_.push_back(vec0);
		batch->vertices_.push_back(vec1);
		batch->vertices_.push_back(vec2);
		batch->vertices_.push_back(vec3);
#endif
		batch->vertices__.push_back(batchvec0);
		batch->vertices__.push_back(batchvec1);
		batch->vertices__.push_back(batchvec2);
		batch->vertices__.push_back(batchvec3);
		
		batch->usedIndices += 6;


		// TODO(ck): Alternate path for filling up uniform buffer and holding the matrices in there
		
		// set rotation matrix for ubo
		//mat4 model = mat4(1.0f);
		//v3 spritePos = v3(posX, posY, posZ);
		//model = glm::translate(model, spritePos);

		// TODO(ck): Rotation causes Z axis to grow??? how to apply rotations to a sprite batcher?
		// .. world coordinates are baked into vertices can it be done in shader?
		//model = glm::rotate(model, glm::radians(-45.0f), v3{ 1.0f, 0.0f, 0.0f });
		//model = glm::translate(model, -spritePos);

		// because we are updating the ubo for each sprite we don't need to containerize it
		// need to bind it here..? 
		//uniformBuffer.push_back(model); // kind of want to keep the data though since its small right now?
	}

	void BindBatchVertices(Batch* batch)
	{
		glBindVertexArray(batch->VAO);
		glBindBuffer(GL_ARRAY_BUFFER, batch->VBO);

		// Use glMapBuffer instead, if moving large chunks of data > 1MB 
		//glBufferSubData(GL_ARRAY_BUFFER, 0, batch->vertices_.size() * sizeof(Vertex3D), &batch->vertices_[0]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, batch->vertices__.size() * sizeof(BatchVertex3D), &batch->vertices__[0]);


		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	
	void DrawBatch(OpenGL* gl, Batch* batch, v3 cameraRight, v3 cameraUp, bool rotateBatch)
	{
		// TODO(ck): If we want the ability to change the map at runtime we need to constantly
// be filling and binding the batch (if things have changed)
// Fill batch 
// bind vertices
		//v3 pos = {};
		//v3 basePos = v3(0.0f, 0.0f, 0.0f);
		//pos.x = basePos.x;
		//pos.y = basePos.y;


		//glUniform3fv(program->lightSpecular, 1, &light.specular[0]);
		//glUniformMatrix4fv(glGetUniformLocation(batchShaderHandle, "model"), 1, GL_FALSE, glm::value_ptr(model));
		
		//glUniform3fv(gl->quadProgram.billboardPos, 1, 0); // &light.specular[0]);
		//glUniform3fv(gl->quadProgram.billboardSize, 1, 0); // &light.specular[0]);
		//glUniformMatrix4fv(gl->quadProgram.VP, 1, GL_FALSE, glm::value_ptr(model));

		// set the view and projection matrix in the uniform block - we only have to do this once per loop iteration.


		//mat4 model = mat4(1.0f);
		/// Rotation
		//v3 playerPos = gl->batchItems_[0].worldPos;
		//pos = playerPos;
		//model = glm::translate(model, pos);
		//
		//// TODO(ck): Rotation causes Z axis to grow??? how to apply rotations to a sprite batcher?
		//// .. world coordinates are baked into vertices can it be done in shader?
		//model = glm::rotate(model, glm::radians(-45.0f), v3{ 1.0f, 0.0f, 0.0f });
		//model = glm::translate(model, -pos);


		// STUDY(ck): TODO(ck): Why does this work?
		// My thought process is that you add the translation in (which is already there because the vertices are in world space
		//  then afterwards the rotation is applied to make sure its rotating around the sprite's current position in world space
		//  we need to subtract the world position out because we are done rotating and the world position is already encoded in our
		//  vertices. Confirm this.....

		//v2 worldScale = v2(64.0f);
		//model = glm::scale(model, v3(worldScale, 1.0f));


		glUniform3fv(gl->quadProgram.cameraRight_worldspace, 1, &cameraRight[0]);
		glUniform3fv(gl->quadProgram.cameraUp_worldspace, 1, &cameraUp[0]);

		glUniform1i(gl->quadProgram.rotateBatch, rotateBatch);

		//glActiveTexture(GL_TEXTURE0);
		// IMPORTANT(ck):
		// NOTE(ck): the reason why you set it to 0 is because thats the base texture slot
		// its not expecting the textureID thats only for binding
		// bind textures on corresponding texture units
		Texture* textureSheet = GetTexture(gl, batch->sheetTextureIndex);
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(gl->quadProgram.common.textureDiffuse1, 0); // texture sheet location
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
		//batch->vertices_.clear();
		batch->vertices__.clear();
	}


	///
	///  [END] Batch Rending 
	///

	///
	/// [BEGIN] Sprite Animation
	///

	void UpdateSpriteAnimation(GLSpriteAnimation* animation, int animationIndex, float dt)
	{

		GLFrame* frame = &animation->frames[animation->frameIndex];
		
		animation->frameCounter += dt;

		while (animation->frameCounter >= frame->duration)
		{
			// reset frame counter
			animation->frameCounter -= frame->duration;

			// increment frame, move back if we're at the end
			animation->frameIndex++;
			if (animation->frameIndex >= animation->frameCount)
				animation->frameIndex = 0;
		}

		
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

		// I think play is advancing the animation and saying play it while this is true
		// and update is just updating it so if its not being called nothing will change?
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

	}

	///
	/// [END] Sprite Animation
	///

	///
	///	[BEGIN] End Rendering
	///


	///
	/// DEBUG RENDERING MOVE
	/// 
	void InitLineBuffer(LineBuffer* buffer, int totalVertSize, int totalIndiceSize)
	{
		// IMPORTANT(ck): Make sure to always malloc an array with sizeof(DebugVertex) if its a struct was not allocating arrays of structs properly before
		buffer->vertices = (DebugVertex*)malloc(totalVertSize * sizeof(DebugVertex));
		buffer->indices = (unsigned int short*)malloc(totalIndiceSize * sizeof(unsigned int short*));
		buffer->totalVerticeSize = totalVertSize;
		buffer->totalIndiceSize = totalIndiceSize;

	}

	void InitGLBuffer(LineBuffer* buffer)
	{
		//int aabbCount = 4096; // 4096
		//int maxVertices = aabbCount * 8;
		//int indicesLength = aabbCount * 16; // or 15??

		glCreateBuffers(1, &buffer->VBO);
		glNamedBufferStorage(buffer->VBO, sizeof(DebugVertex) * buffer->totalVerticeSize, nullptr, GL_DYNAMIC_STORAGE_BIT);

		glCreateBuffers(1, &buffer->IBO);
		glNamedBufferStorage(buffer->IBO, sizeof(GLushort) * buffer->totalIndiceSize, nullptr, GL_DYNAMIC_STORAGE_BIT);

		glCreateVertexArrays(1, &buffer->VAO);
		glVertexArrayVertexBuffer(buffer->VAO, 0, buffer->VBO, 0, sizeof(DebugVertex));
		glVertexArrayElementBuffer(buffer->VAO, buffer->IBO);

		glEnableVertexArrayAttrib(buffer->VAO, 0);
		glVertexArrayAttribFormat(buffer->VAO, 0, 4, GL_FLOAT, GL_FALSE, offsetof(DebugVertex, pos));
		glVertexArrayAttribBinding(buffer->VAO, 0, 0);

		glEnableVertexArrayAttrib(buffer->VAO, 1);
		glVertexArrayAttribFormat(buffer->VAO, 1, 4, GL_FLOAT, GL_FALSE, offsetof(DebugVertex, colour));
		glVertexArrayAttribBinding(buffer->VAO, 1, 0);
	}

	void DrawLine2D(LineBuffer* buffer, v4 a, v4 b, v4 colour)
	{
		// Move to pixel space before rendering
#if 0
		a.x *= 16.0f;
		a.y *= 16.0f;
		b.x *= 16.0f;
		b.y *= 16.0f;
#endif

		buffer->vertices[buffer->lastVerticeIndex++] = DebugVertex{ a, colour };
		buffer->vertices[buffer->lastVerticeIndex++] = DebugVertex{ b, colour };

		buffer->indices[buffer->lastIndiceIndex++] = 0 + (buffer->offset);
		buffer->indices[buffer->lastIndiceIndex++] = 1 + (buffer->offset);

		buffer->offset += 2;
		buffer->usedIndices += 2;
	}


	void DrawLine(LineBuffer* buffer, glm::vec4 a, glm::vec4 b, glm::vec4 colour)
	{

		buffer->vertices[buffer->lastVerticeIndex++] = DebugVertex{ a, colour };
		buffer->vertices[buffer->lastVerticeIndex++] = DebugVertex{ b, colour };

		buffer->indices[buffer->lastIndiceIndex++] = 0 + (buffer->offset);
		buffer->indices[buffer->lastIndiceIndex++] = 1 + (buffer->offset);

		buffer->offset += 2;
		buffer->usedIndices += 2;
	}

	void DrawFrustum(LineBuffer* buffer, glm::vec4 vertices[])
	{
		glm::vec4 colour = glm::vec4(0.10f, 0.10f, 0.65f, 1.0f);
		DrawLine(buffer, vertices[0], vertices[1], colour); // near top
		DrawLine(buffer, vertices[1], vertices[3], colour); // near right
		DrawLine(buffer, vertices[3], vertices[2], colour); // near bottom
		DrawLine(buffer, vertices[2], vertices[0], colour); // near left

		DrawLine(buffer, vertices[4], vertices[5], colour); // far top
		DrawLine(buffer, vertices[5], vertices[7], colour); // far right
		DrawLine(buffer, vertices[7], vertices[6], colour); // far bottom
		DrawLine(buffer, vertices[6], vertices[4], colour); // far left

		DrawLine(buffer, vertices[0], vertices[4], colour); // left top
		DrawLine(buffer, vertices[2], vertices[6], colour); // left bottom

		DrawLine(buffer, vertices[1], vertices[5], colour); // right top
		DrawLine(buffer, vertices[3], vertices[7], colour); // right bottom
	}

	void DrawBox(LineBuffer* buffer, glm::vec3 min, glm::vec3 max, glm::vec4 colour)
	{
		//int entOffset = 0; // TODO(ck): DEBUGGING LINE INDICE UPLOAD TO BOUNDING BOX

		buffer->vertices[buffer->lastVerticeIndex++] = DebugVertex{ glm::vec4(min.x, min.y, min.z, 1.0f), colour };
		buffer->vertices[buffer->lastVerticeIndex++] = DebugVertex{ glm::vec4(max.x, min.y, min.z, 1.0f), colour };
		buffer->vertices[buffer->lastVerticeIndex++] = DebugVertex{ glm::vec4(min.x, max.y, min.z, 1.0f), colour };
		buffer->vertices[buffer->lastVerticeIndex++] = DebugVertex{ glm::vec4(max.x, max.y, min.z, 1.0f), colour };

		buffer->vertices[buffer->lastVerticeIndex++] = DebugVertex{ glm::vec4(min.x, min.y, max.z, 1.0f), colour };
		buffer->vertices[buffer->lastVerticeIndex++] = DebugVertex{ glm::vec4(max.x, min.y, max.z, 1.0f), colour };
		buffer->vertices[buffer->lastVerticeIndex++] = DebugVertex{ glm::vec4(min.x, max.y, max.z, 1.0f), colour };
		buffer->vertices[buffer->lastVerticeIndex++] = DebugVertex{ glm::vec4(max.x, max.y, max.z, 1.0f), colour };

		// first rect of lines
		buffer->indices[buffer->lastIndiceIndex++] = (0 + buffer->offset);
		buffer->indices[buffer->lastIndiceIndex++] = (1 + buffer->offset);
		buffer->indices[buffer->lastIndiceIndex++] = (1 + buffer->offset);
		buffer->indices[buffer->lastIndiceIndex++] = (3 + buffer->offset);
		buffer->indices[buffer->lastIndiceIndex++] = (3 + buffer->offset);
		buffer->indices[buffer->lastIndiceIndex++] = (2 + buffer->offset);
		buffer->indices[buffer->lastIndiceIndex++] = (2 + buffer->offset);
		buffer->indices[buffer->lastIndiceIndex++] = (0 + buffer->offset);


		// second rect of lines
		buffer->indices[buffer->lastIndiceIndex++] = (4 + buffer->offset);
		buffer->indices[buffer->lastIndiceIndex++] = (5 + buffer->offset);
		buffer->indices[buffer->lastIndiceIndex++] = (5 + buffer->offset);
		buffer->indices[buffer->lastIndiceIndex++] = (7 + buffer->offset);
		buffer->indices[buffer->lastIndiceIndex++] = (7 + buffer->offset);
		buffer->indices[buffer->lastIndiceIndex++] = (6 + buffer->offset);
		buffer->indices[buffer->lastIndiceIndex++] = (6 + buffer->offset);
		buffer->indices[buffer->lastIndiceIndex++] = (4 + buffer->offset);

		// connecting lines top and bottom
		buffer->indices[buffer->lastIndiceIndex++] = (0 + buffer->offset);
		buffer->indices[buffer->lastIndiceIndex++] = (4 + buffer->offset);
		buffer->indices[buffer->lastIndiceIndex++] = (1 + buffer->offset);
		buffer->indices[buffer->lastIndiceIndex++] = (5 + buffer->offset);
		buffer->indices[buffer->lastIndiceIndex++] = (2 + buffer->offset);
		buffer->indices[buffer->lastIndiceIndex++] = (6 + buffer->offset);
		buffer->indices[buffer->lastIndiceIndex++] = (3 + buffer->offset);
		buffer->indices[buffer->lastIndiceIndex++] = (7 + buffer->offset);


		buffer->offset += 8;
		buffer->usedIndices += 24;
	}

	void DrawBox2D(LineBuffer* buffer, glm::vec3 min, glm::vec3 max, glm::vec4 colour)
	{
		//int entOffset = 0; // TODO(ck): DEBUGGING LINE INDICE UPLOAD TO BOUNDING BOX
		float pixelSize = 1.0f;
		float thickness = 1.0f;
		buffer->vertices[buffer->lastVerticeIndex++] = DebugVertex{ glm::vec4(min.x * pixelSize, min.y * pixelSize, min.z * pixelSize, 1.0f), colour };
		buffer->vertices[buffer->lastVerticeIndex++] = DebugVertex{ glm::vec4(max.x * pixelSize, min.y * pixelSize, min.z * pixelSize, 1.0f), colour };
		buffer->vertices[buffer->lastVerticeIndex++] = DebugVertex{ glm::vec4(min.x * pixelSize, max.y * pixelSize, min.z * pixelSize, 1.0f), colour };
		buffer->vertices[buffer->lastVerticeIndex++] = DebugVertex{ glm::vec4(max.x * pixelSize, max.y * pixelSize, min.z * pixelSize, 1.0f), colour };

		// first rect of lines
		buffer->indices[buffer->lastIndiceIndex++] = (0 + buffer->offset);
		buffer->indices[buffer->lastIndiceIndex++] = (1 + buffer->offset);
		buffer->indices[buffer->lastIndiceIndex++] = (1 + buffer->offset);
		buffer->indices[buffer->lastIndiceIndex++] = (3 + buffer->offset);
		buffer->indices[buffer->lastIndiceIndex++] = (3 + buffer->offset);
		buffer->indices[buffer->lastIndiceIndex++] = (2 + buffer->offset);
		buffer->indices[buffer->lastIndiceIndex++] = (2 + buffer->offset);
		buffer->indices[buffer->lastIndiceIndex++] = (0 + buffer->offset);


		buffer->offset += 4;
		buffer->usedIndices += 8;
	}

	void ResetBuffer(LineBuffer* buffer)
	{
		buffer->usedIndices = 0;
		buffer->offset = 0;
		buffer->lastVerticeIndex = 0;
		buffer->lastIndiceIndex = 0;
	}

	void EndRender(OpenGL* gl)
	{
		assert(globalDrawCalls > 0);
		
		// We don't even need these we can use a raw array and clear it out each frame?
		// just set everything to 0 
		ResetBuffer(&gl->lineBuffer);
		gl->transparentItems.clear();
		gl->opaqueItems.clear();
		

		gl->batchItems_.clear();
		


		gl->batchItems2D.clear();

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
		MonGL::LoadShader(&gl->frameBufferProgram, "res/shaders/vert_screen.glsl", "res/shaders/frag_screen.glsl", NULL);
		MonGL::LoadShader(&gl->debugProgram, "res/shaders/vert_debug.glsl", "res/shaders/frag_debug.glsl", NULL);
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

		LoadTexture((char*)"temp1", t1, MonGL::TextureType::Diffuse, true, GetImage(g_Assets, 0));
		LoadTexture((char*)"temp2", t2, MonGL::TextureType::Diffuse, true, GetImage(g_Assets, 1));
		LoadTexture((char*)"temp3", t3, MonGL::TextureType::Diffuse, true, GetImage(g_Assets, 2));
		LoadTexture((char*)"temp4", t4, MonGL::TextureType::Diffuse, true, GetImage(g_Assets, 3));
		LoadTexture((char*)"temp5", t5, MonGL::TextureType::Diffuse, true, GetImage(g_Assets, 4));
		LoadTexture((char*)"temp6", t6, MonGL::TextureType::Diffuse, true, GetImage(g_Assets, 5));
		LoadTexture((char*)"temp7", t7, MonGL::TextureType::Diffuse, true, GetImage(g_Assets, 6));
		LoadTexture((char*)"temp8", t8, MonGL::TextureType::Diffuse, true, GetImage(g_Assets, 7));
		LoadTexture((char*)"temp9", t9, MonGL::TextureType::Diffuse, true, GetImage(g_Assets, 8));
		LoadTexture((char*)"temp10", t10, MonGL::TextureType::Diffuse, true, GetImage(g_Assets, 9));
		LoadTexture((char*)"temp11", t11, MonGL::TextureType::Diffuse, true, GetImage(g_Assets, 10));
		LoadTexture((char*)"temp12", t12, MonGL::TextureType::Diffuse, true, GetImage(g_Assets, 11));
		LoadTexture((char*)"temp13", t13, MonGL::TextureType::Diffuse, true, GetImage(g_Assets, 12));
		LoadTexture((char*)"temp14", t14, MonGL::TextureType::Diffuse, true, GetImage(g_Assets, 13));
		LoadTexture((char*)"temp15", t15, MonGL::TextureType::Diffuse, true, GetImage(g_Assets, 14));
		LoadTexture((char*)"temp16", t16, MonGL::TextureType::Diffuse, true, GetImage(g_Assets, 17));
		

		/*
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
		MonGL::Texture* t = GetTexture(gl, assetCount+1);
		
		
		*/


		// Texture Atlases
		AddTexture(gl);
		MonGL::Texture* t17 = GetTexture(gl, 17);
		AddTexture(gl);
		MonGL::Texture* t18 = GetTexture(gl, 18);
		AddTexture(gl);
		MonGL::Texture* t19 = GetTexture(gl, 19);
		LoadTextureFile((char*)"sprite_atlas", t17, GetImage(g_Assets, 30), TextureType::Diffuse, true, true);
		LoadTextureFile((char*)"debug_icons", t18, GetImage(g_Assets, 31), TextureType::Diffuse, true, true);
		LoadTextureFile((char*)"tilemap", t19, GetImage(g_Assets, 15), TextureType::Diffuse, true, true);

		// 2D Texture Array 
		// need this to use different textures in the same batch
		//AddTexture(gl);
		//MonGL::Texture* t20 = GetTexture(gl, 20);
		//Load2DTextureArrayFile(t20, "");


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

		// Init Animations
		{
			// null at index 0
			GLSpriteAnimator* glAnimator = &gl->spriteAnimators[0];
			glAnimator = {};
			gl->spriteAnimatorCount++;

			// Add animator method
			GLSpriteAnimator* glAnimator1 = &gl->spriteAnimators[1];
			glAnimator1->animationIndex = 1;
			gl->spriteAnimatorCount++;

			const int animationCount = 1;
			int frameCount = 4;

			const int subTextureCount = 4;
			float sheetSize = 256.0f;
			float tileSize = 128.0f;

			// 256sheetsize / 32 tilesize=8 -1 = 7 for offsets y=7 is top of sheet x=0is left of sheet 
			// what if we our tile is a different size...
			
			// can we somehow add to 32 so say tileSize=32 objectSize=96 96/32=3 [objectSize/tileSize]=[offsets] 
			float tileOffsetX = 0.0f;
			float tileOffsetY = 0.0f;

			for (int i = 0; i < animationCount; ++i)
			{
				// Add animation method
				GLSpriteAnimation* ani = &gl->spriteAnimators[1].animations[i];
				ani->textureAtlasIndex = 0;
				glAnimator1->animationCount++;

				ani->frameIndex = 0;
				ani->frameCount = 0;

				for (int frameIndex = 0; frameIndex < frameCount; ++frameIndex)
				{
					// Add frame method
					GLFrame* frame = &ani->frames[frameIndex];
					ani->frameCount++;

					if (frameIndex == 1)
					{
						//tileSize = 32;
						tileOffsetX = 6;
					}
					else if (frameIndex == 2)
					{
						tileOffsetX = 1;
					}
					else if (frameIndex == 3)
					{
						tileOffsetX = 1;
					}

					frame->subTexture = {};
					frame->subTexture.width = 32;
					frame->subTexture.height = 32;
					frame->subTexture.tileSize = 128;
					frame->subTexture.sheetSize = 256.0f;
					float cellSize = 128.0f;
					float worldSize = 1.0f; // we don't need this always going to be 1 for us?
					float spacing = 0.0f;
					// if (spacing > 0) then add 1 to the tileOffsets and add 1 to the cell size so that we can get the math right for our padding
					
					v2 min = { ((tileOffsetX * cellSize) + spacing) / sheetSize, ((tileOffsetY * cellSize) + spacing) / sheetSize };
					v2 max = { ((tileOffsetX + worldSize) * cellSize) / sheetSize, (((tileOffsetY + worldSize) * cellSize)) / sheetSize };
					
					frame->subTexture.texCoords[0] = v2{min.x, min.y}; // bottom left
					frame->subTexture.texCoords[1] = v2{max.x, min.y}; // bottom right
					frame->subTexture.texCoords[2] = v2{max.x, max.y}; // top right
					frame->subTexture.texCoords[3] = v2{min.x, max.y}; // top left 

					frame->duration = 0.2f;

				}
			}

			// Walk animation

			// Add animator method
			GLSpriteAnimator* glAnimatorWalk = &gl->spriteAnimators[1];
			glAnimatorWalk->animationIndex = 1;
			gl->spriteAnimatorCount++;

			frameCount = 10;
			tileSize = 32;
			//sheet size 
			for (int i = 0; i < animationCount; ++i)
			{
				// Add animation method
				GLSpriteAnimation* ani = &gl->spriteAnimators[2].animations[i];
				ani->textureAtlasIndex = 0;
				glAnimatorWalk->animationCount++;

				ani->frameIndex = 0;
				ani->frameCounter = 0;
				tileOffsetY = 5;
				tileOffsetX = 0;

				for (int frameIndex = 0; frameIndex < frameCount; ++frameIndex)
				{
					// Add frame method
					GLFrame* frame = &ani->frames[frameIndex];
					ani->frameCount++;

					frame->subTexture = {};
					frame->subTexture.height = 32;
					frame->subTexture.height = 32;
					frame->subTexture.tileSize = 32;
					frame->subTexture.sheetSize = 256.0f;

					float cellSize = 32.0f;
					float spacing = 0.0f;
					float worldSize = 1.0f; // bad name for this?
					v2 min = { ((tileOffsetX * cellSize) + spacing) / sheetSize, ((tileOffsetY * cellSize) + spacing) / sheetSize };
					v2 max = { ((tileOffsetX + worldSize) * cellSize) / sheetSize, (((tileOffsetY + worldSize) * cellSize)) / sheetSize };
					frame->subTexture.texCoords[0] = v2{min.x, min.y}; // bottom left
					frame->subTexture.texCoords[1] = v2{max.x, min.y}; // bottom right
					frame->subTexture.texCoords[2] = v2{max.x, max.y}; // top right
					frame->subTexture.texCoords[3] = v2{min.x, max.y}; // top left 

					frame->duration = 0.2f;
					tileOffsetX++;
				}
			}
		} // Animations 


		// Frame buffer
		glCreateFramebuffers(1, &gl->fbo);
		glCreateTextures(GL_TEXTURE_2D, 1, &gl->textureColorbuffer);
		//glGenerateMipmap(GL_TEXTURE_2D); // DO I NEED THIS?

		glTextureParameteri(gl->textureColorbuffer, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(gl->textureColorbuffer, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTextureParameteri(gl->textureColorbuffer, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(gl->textureColorbuffer, GL_TEXTURE_WRAP_T, GL_REPEAT);

		float viewPortW = 960.0f;
		float viewPortH = 540.0f;
		glTextureStorage2D( gl->textureColorbuffer, 1, GL_RGB16, (int)viewPortW, (int)viewPortH);
		glTextureSubImage2D(gl->textureColorbuffer, 0, 0, 0, (int)viewPortW, (int)viewPortH, GL_RGB16, GL_UNSIGNED_BYTE, NULL);
		glObjectLabel(GL_TEXTURE, gl->textureColorbuffer, -1, "fbo_screen_texture");

		glNamedFramebufferTexture(gl->fbo, GL_COLOR_ATTACHMENT0, gl->textureColorbuffer, 0);

		// depth texture
		// Instead of using a render buffer (which is legacy) we use a depth texture which allows us to use 
		// it in separate passes.
		glCreateTextures(GL_TEXTURE_2D, 1, &gl->depthTexture);

		glTextureParameteri(gl->depthTexture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(gl->depthTexture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTextureParameteri(gl->depthTexture, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
		glTextureParameteri(gl->depthTexture, GL_TEXTURE_COMPARE_MODE, GL_NONE);

		glTextureStorage2D(gl->depthTexture, 1, GL_DEPTH_COMPONENT16, (int)viewPortW, (int)viewPortH);
		glTextureSubImage2D(gl->depthTexture, 0, 0, 0, (int)viewPortW, (int)viewPortH, GL_DEPTH_COMPONENT16, GL_FLOAT, NULL);

		glNamedFramebufferTexture(gl->fbo, GL_DEPTH_ATTACHMENT, gl->depthTexture, 0);


		//if (glCheckNamedFramebufferStatus(fbo, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			//return false; // ERROR FRAMEBUFFER framebuffer is not complete 
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//
		// Frame buffer VAO
		//
		float quadVertices[] = {

			// pos			tex coords
			-1.0f, 1.0f,	0.0f, 1.0f,
			-1.0f, -1.0f,	0.0f, 0.0f,
			1.0f, -1.0f,	1.0f, 0.0f,

			-1.0f, 1.0f,	0.0f, 1.0f,
			1.0f, -1.0f,	1.0f, 0.0f,
			1.0f, 1.0f,		1.0f, 1.0f,
		};

		
		glCreateBuffers(1, &gl->screenVBO);
		glNamedBufferStorage(gl->screenVBO, sizeof(quadVertices), &quadVertices, GL_DYNAMIC_STORAGE_BIT);

		glCreateVertexArrays(1, &gl->screenVAO);
		glVertexArrayVertexBuffer(gl->screenVAO, 0, gl->screenVBO, 0, sizeof(float) * 4);

		glEnableVertexArrayAttrib(gl->screenVAO, 0);
		//glVertexArrayAttribFormat(screenVAO, 0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glVertexArrayAttribFormat(gl->screenVAO, 0, 2, GL_FLOAT, GL_FALSE, 0);
		glVertexArrayAttribBinding(gl->screenVAO, 0, 0);

		glEnableVertexArrayAttrib(gl->screenVAO, 1);
		//glVertexArrayAttribFormat(screenVAO, 1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		glVertexArrayAttribFormat(gl->screenVAO, 1, 2, GL_FLOAT, GL_FALSE, (2 * sizeof(float)));
		glVertexArrayAttribBinding(gl->screenVAO, 1, 0);

		glUseProgram(gl->frameBufferProgram.handle);
		glUniform1i(glGetUniformLocation(gl->frameBufferProgram.handle, "screenTexture"), 0);


		// Tilemap and sprite batch
		int batchIndex = AddBatch(gl);
		InitBatch(gl, batchIndex, 1000);
		batchIndex = AddBatch(gl);
		InitBatch(gl, batchIndex, 1000);		


		// line buffer
		int vertSize = 2048;
		int indiceSize = 2048 * 4;
		InitLineBuffer(&gl->lineBuffer, vertSize, indiceSize);
		InitGLBuffer(&gl->lineBuffer);

		// Set Blend Function
		//glDepthFunc(GL_LESS);
		glEnable(GL_BLEND);
		// https://www.adriancourreges.com/blog/2017/05/09/beware-of-transparent-pixels/ proper blend func for pre-multiplied alpha
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		//glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
		
		glDisable(GL_MULTISAMPLE);
		// TODO(ck): Remove temp
		glViewport(0, 0, 960, 540);
		//glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
		//glBlendFuncSeparate(GL_ONE, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	}

	void InitOpenGLBatchMesh(BatchData* data)
	{
		const int quadCount = 2000;
		const int maxVertices = quadCount * 4;
		int indicesLength = quadCount * 6;

		glCreateBuffers(1, &data->VBO);
		glNamedBufferStorage(data->VBO, sizeof(Vertex) * maxVertices, nullptr, GL_DYNAMIC_STORAGE_BIT);

		glCreateBuffers(1, &data->IBO);
		glNamedBufferStorage(data->IBO, sizeof(uint32_t) * indicesLength, data->indices, GL_DYNAMIC_STORAGE_BIT);

		glCreateVertexArrays(1, &data->VAO);

		glVertexArrayVertexBuffer(data->VAO, 0, data->VBO, 0, sizeof(Vertex));
		glVertexArrayElementBuffer(data->VAO, data->IBO);

		glEnableVertexArrayAttrib(data->VAO, 0);
		glVertexArrayAttribFormat(data->VAO, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
		glVertexArrayAttribBinding(data->VAO, 0, 0);

		glEnableVertexArrayAttrib(data->VAO, 1);
		glVertexArrayAttribFormat(data->VAO, 1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, color));
		glVertexArrayAttribBinding(data->VAO, 1, 0);

		glEnableVertexArrayAttrib(data->VAO, 2);
		glVertexArrayAttribFormat(data->VAO, 2, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, texCoords));
		glVertexArrayAttribBinding(data->VAO, 2, 0);

		glBindVertexArray(data->VAO);

		std::string vaoName = "Batch VAO";
		std::string vboName = "Batch VBO";
		std::string iboName = "Batch IBO";
		glObjectLabel(GL_VERTEX_ARRAY, data->VAO, -1, vaoName.c_str());
		glObjectLabel(GL_BUFFER, data->VBO, -1, vboName.c_str());
		glObjectLabel(GL_BUFFER, data->IBO, -1, iboName.c_str());

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
		// do not prebuild indicies?

		// Stop prebuilding

		batch->indices = new uint32_t[batch->indicesLength];
		int offset = 0;
		for (int i = 0; i < batch->indicesLength; i += 6) // 6 indices for each quad
		{
			batch->indices[i + 0] = 0 + offset;
			batch->indices[i + 1] = 1 + offset;
			batch->indices[i + 2] = 2 + offset;

			batch->indices[i + 3] = 2 + offset;
			batch->indices[i + 4] = 3 + offset;
			batch->indices[i + 5] = 0 + offset;

			offset += 4; // indices for 4 vertices
		}

		InitOpenGLBatchMesh(batch);
	}

	void InitGLBatch(BatchMesh* batch)
	{
		// Design the same way as the LineBuffer object now that you ACTUALLY understand how this rendering "stuff" works
	}

	void InitBatch(OpenGL* gl, int batchIndex, int tileAmount)
	{
		BatchData* batch = GetBatch2D(gl, batchIndex);
		InitBatch(batch, tileAmount);
	}

	void FillBatch(BatchData* batch, float sheetSize, int tileSize, float worldX, float worldY, v2 textureOffset, v2 cameraPos)
	{
		// TODO(ck): Pass Point or integer Rect no floating points
		int tileOffsetX = (int)textureOffset.x; // There is not an extra small cell at the start of the x offset.
		int tileOffsetY = (int)textureOffset.y - 1; // There is an extra small cell at the start of the y offset so subtract it

		// Do not update the animation inside of the batch 
		// update the animation in the update and then get the data here.
		float worldSize = 1.0f;
		float spacing = 1.0f;
		float cellSize = 17.0f; // pixels

		v2 min = { ((tileOffsetX * cellSize)+spacing) / sheetSize, ((tileOffsetY * cellSize)+spacing) / sheetSize };
		v2 max = { (((tileOffsetX + worldSize)* cellSize)) / sheetSize, (((tileOffsetY + worldSize) * cellSize)) / sheetSize };

		float size = 1.0f;
		float x = worldX;
		float y = worldY;

		Vertex vec0 = {
			v3(x, y, 0.0f),
			v3(1.0f, 0.0f, 0.0f),
			v2{min.x, min.y} // bl
		};
		Vertex vec1 = {
			v3((x + size), y, 0.0f),
			v3(0.0f, 1.0f, 0.0f),
			v2{max.x, min.y} // br
		};
		Vertex vec2 = {
			v3((x + size), (y + size), 0.0f),
			v3(0.0f, 0.0f, 1.0f),
			v2{max.x, max.y} // tr
		};
		Vertex vec3 = {
			v3((x), (y + size), 0.0f),
			v3(1.0f, 1.0f, 0.0f),
			v2{min.x, max.y} // tl
		};

		batch->usedIndices += 6;
		batch->vertices.push_back(vec0);
		batch->vertices.push_back(vec1);
		batch->vertices.push_back(vec2);
		batch->vertices.push_back(vec3);
	}


	void FillBatch(BatchData* batch, float sheetSize, int tileSize, float worldX, float worldY, GLSubTexture subTexture)
	{
		// 
		//float size = spriteSize * (float)subTexture.tileSize;
		
		//int size = subTexture.tileSize;
		float size = 1.0f;
		if (subTexture.tileSize == 128)
			size = 3.0f;

		// tile coords to world coords
		//worldX *= (float)tileSize;
		//worldY *= (float)tileSize;
		float pixelsPerMeter = 1.0f;
		// worldSize is 2.0f; local(model) - 32pixels;
		// TODO(ck): Ask Jasper about this??? is world size your 1m and then what is pixel size we dont call it world anymore?
		// Move the sprite into the middle of the bounding box... worldX - (worldSize/2.0f) * pixelsPerMeter

		// x and y is pixel space?

		// I don't really have a local space in the pixel art?
		// Its almost like everything starts in world then goes to pixels
		// I guess local space would be "tile space" and then we go to world space then pixel space?

		//float x = (worldX-1.0f) * pixelsPerMeter;
		//float y = (worldY-1.0f) * pixelsPerMeter;
		float x = (worldX-0.5f);
		float y = (worldY-0.5f);

		Vertex vec0 = {
			v3(x, y, 0.0f),
			v3(1.0f, 0.0f, 0.0f),
			subTexture.texCoords[0]
		};
		Vertex vec1 = {
			v3((x + size), y, 0.0f),
			v3(0.0f, 1.0f, 0.0f),
			subTexture.texCoords[1]
		};
		Vertex vec2 = {
			v3((x + size), (y + size), 0.0f),
			v3(0.0f, 0.0f, 1.0f),
			subTexture.texCoords[2]
		};
		Vertex vec3 = {
			v3(x, (y + size), 0.0f),
			v3(1.0f, 1.0f, 0.0f),
			subTexture.texCoords[3]
		};

		batch->usedIndices += 6;
		batch->vertices.push_back(vec0);
		batch->vertices.push_back(vec1);
		batch->vertices.push_back(vec2);
		batch->vertices.push_back(vec3);
	}

	void FillBatch(BatchData* batch, BatchItem2D item, GLSubTexture subTexture)
	{
		// 
//float size = spriteSize * (float)subTexture.tileSize;
		int size = subTexture.tileSize;


		float pixelsPerMeter = 16.0f;
		float x = (item.worldPos.x - 1.0f) * pixelsPerMeter;
		float y = (item.worldPos.y - 1.0f) * pixelsPerMeter;

		Vertex vec0 = {
			v3(x, y, -1.0f),
			v3(1.0f, 0.0f, 0.0f),
			subTexture.texCoords[0]
		};
		Vertex vec1 = {
			v3((x + size), y, -1.0f),
			v3(0.0f, 1.0f, 0.0f),
			subTexture.texCoords[1]
		};
		Vertex vec2 = {
			v3((x + size), (y + size), -1.0f),
			v3(0.0f, 0.0f, 1.0f),
			subTexture.texCoords[2]
		};
		Vertex vec3 = {
			v3(x, (y + size), -1.0f),
			v3(1.0f, 1.0f, 0.0f),
			subTexture.texCoords[3]
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

	bool SortBatchItems2D(BatchItem2D& left, BatchItem2D& right)
	{
		return (left.worldPos.y > right.worldPos.y);
	}

	void SortBatch2D(std::vector<BatchItem2D>& batchItems)
	{
		assert(batchItems.size() > 0);
		std::sort(batchItems.begin(), batchItems.end(), SortBatchItems2D);
		// std::qsort(batchItems, batchItems.size(), sizeof(BatchItem), sortBatchItems);
		return;
	}

	void DrawBatch(BatchData* batch, CommonProgram* shader, unsigned int textureID, bool wireFrame)
	{
		glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 1, -1, "Batch Items");

		BindVertices(batch);

		v3 pos = {};
		mat4 model = mat4(1.0f);
		model = glm::translate(model, pos);

		glUniformMatrix4fv(glGetUniformLocation(shader->handle, "model"), 1, GL_FALSE, glm::value_ptr(model));

		glActiveTexture(GL_TEXTURE0);
		glUniform1i(glGetUniformLocation(shader->handle, "image"), 0);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glBindVertexArray(batch->VAO);

		//int polygonMode = textureID == 19 ? GL_LINE : GL_FILL;
		/*int polygonMode = GL_LINE;
		glPolygonMode(GL_FRONT_AND_BACK, polygonMode);*/
		glDrawElements(GL_TRIANGLES, batch->usedIndices, GL_UNSIGNED_INT, (void*)(0));
		glBindVertexArray(0);

		// TODO(ck): convert this to use our own Array instead of an std::vector not sure what the perf
		// is on .clear() especially if we have a lot of vertices in the batch?
		// reset buffer
		batch->usedIndices = 0;
		batch->vertices.clear();

		glPopDebugGroup();
		globalDrawCalls++;
	}
}