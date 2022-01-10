/*
	 ________  _________    _     _____  ___ ______ ___________
	|_   _|  \/  || ___ \  | |   |  _  |/ _ \|  _  \  ___| ___ \
	  | | | .  . || |_/ /  | |   | | | / /_\ \ | | | |__ | |_/ /
	  | | | |\/| ||  __/   | |   | | | |  _  | | | |  __||    /
	 _| |_| |  | || |      | |___\ \_/ / | | | |/ /| |___| |\ \
	 \___/\_|  |_/\_|      \_____/\___/\_| |_/___/ \____/\_| \_|


DESC:	Imp loader current uses Assimp to load popular model formats. 
		The Assimp loaded models are then exported to .imp files to be consumed by the Monster engine
		
		This loader will be a crucial part of the Monster engine for asset loading and exporting
		Audio will be dealt with in the imp loader as well once that bridge appears.

		Jan 01/2022 - Cole Knapp


IMPORTANT(ck): README
The assimp-vc142-mtd.dll must be in D:\Programming\$$GameProjects\monster\imp_loader
for some reason... Visual studio must differ the paths it looks for dlls for the
sub projects? the main project requires the dll to be elsewhere.. could be some other issue


- load .obj .fbx and whatever is the easiest to work with file format for 3d models
- export out .imp files with Assimp for now

Assimp is heavy I do not want it in the main engine. Eventually I would like to write
the loader on my own and not rely on assimp. Remember that we can use this project in finnsie
the best thing to do is have this project loading the file and call it from monster instead of 
having an import/export logic in Monster
*/

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <stdio.h>

#include <vector>

struct Texture
{
	unsigned int id;
	std::string type;
	std::string path;
};

// checks all material textures of a given type and loads the textures if they're not loaded yet.
// the required info is returned as a Texture struct.
std::vector<Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
	std::vector<Texture> textures;
	for (int i = 0; i < mat->GetTextureCount(type); ++i)
	{
		
	}

	return textures;
}

struct v3
{
	float x;
	float y;
	float z;
};

struct v2
{
	float x;
	float y;
	float z;
};

struct Vertex
{
	v3 position;
	v3 normal;
	v2 texCoords;
	v3 tangent;
	v3 bitangent;
};

struct Mesh
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;
};

struct Model
{
	std::vector<Texture> textures;
	std::vector<Mesh> meshes;
	bool gammaCorrection;
};

Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
	// data to fill 
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	for (int i = 0; i < mesh->mNumVertices; ++i)
	{
		Vertex vertex;

		// transfer assimp v3 to ours
		v3 vec;
		vec.x = mesh->mVertices[i].x;
		vec.y = mesh->mVertices[i].y;
		vec.z = mesh->mVertices[i].z;
		vertex.position = vec;
		
		vec.x = mesh->mNormals[i].x;
		vec.y = mesh->mNormals[i].y;
		vec.z = mesh->mNormals[i].z;
		vertex.normal = vec;

		if (mesh->mTextureCoords[0])
		{
			v2 vec; 
			// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
			// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.texCoords = vec;
		}
		else
		{
			vertex.texCoords = v2{ 0.0f, 0.0f };
		}

		vec.x = mesh->mTangents[i].x;
		vec.y = mesh->mTangents[i].y;
		vec.z = mesh->mTangents[i].z;
		vertex.tangent = vec;

		vec.x = mesh->mBitangents[i].x;
		vec.z = mesh->mBitangents[i].y;
		vec.y = mesh->mBitangents[i].z;
		vertex.bitangent = vec;

		// TODO(ck): Can also pull out mColor[] information...
		vertices.push_back(vertex);
	}

	for (int i = 0; i < mesh->mNumFaces; ++i)
	{
		aiFace face = mesh->mFaces[i];
		for (int j = 0; j < face.mNumIndices; ++j)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

	// we assume a convention for sampler names in the shaders. Each diffuse texture should be named
	// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
	// Same applies to other texture as the following list summarizes:
	// diffuse: texture_diffuseN
	// specular: texture_specularN
	// normal: texture_normalN
	
	std::vector<Texture> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

	std::vector<Texture> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
	textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

	std::vector<Texture> normalMaps = LoadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
	textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

	std::vector<Texture> heightMaps = LoadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
	textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());


	return Mesh{ vertices, indices, textures };
}

void ProcessNode(Model* model, aiNode* node, const aiScene* scene)
{
	// process each mesh located at the current node
	for (int i = 0; i < node->mNumMeshes; ++i)
	{
		// node object only contains indices to index the actual objects in the scene
		// the scene contains all the data, node is just to keep stuff organized (like realations between nodes)
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		model->meshes.push_back(ProcessMesh(mesh, scene));
	}

	// after meshes have been processed, recursively process each of the child nodes
	for (int i = 0; i < node->mNumChildren; ++i)
	{
		ProcessNode(model, node->mChildren[i], scene);
	}
}

bool LoadModel(Model* model, std::string path, bool gammaCorrection)
{
	model->gammaCorrection = gammaCorrection;

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		// Error::Assimp:: importer.GetErrorString() << "\n";
		return false;
	}
	// process ASSIMP's root node recursively
	ProcessNode(model, scene->mRootNode, scene);

	return true;
}


void CreateImpFileFromModel(Model* model)
{
/*
if (ImGui::SmallButton("SAVE TO IMP FILE"))
{
	// Save the selected model to an '.imp' file
	// loads with the Monster Engine
	std::string path = "model.imp";
	std::ofstream file;
	file.open(path);


	GameObject* obj = g_Game->objects[selected];
	for (int i = 0; i < obj->model->meshes.size(); ++i)
	{
		file << obj->name << "\n";

		Mesh* mesh = &obj->model->meshes[i];
		file << mesh->vertices.size() << "\n";
		file << mesh->indices.size() << "\n";
		for (int vertIndex = 0; vertIndex < mesh->vertices.size(); ++vertIndex)
		{

			file << mesh->vertices[vertIndex].position.x << "\n" << mesh->vertices[vertIndex].position.y << "\n" << mesh->vertices[vertIndex].position.z << "\n"
					<< mesh->vertices[vertIndex].normal.x << "\n" << mesh->vertices[vertIndex].normal.y << "\n" << mesh->vertices[vertIndex].normal.z << "\n"
					<< mesh->vertices[vertIndex].texCoords.x << "\n" << mesh->vertices[vertIndex].texCoords.y << "\n"
					<< mesh->vertices[vertIndex].tangent.x << "\n" << mesh->vertices[vertIndex].tangent.y << "\n" << mesh->vertices[vertIndex].tangent.z << "\n"
					<< mesh->vertices[vertIndex].bitangent.x << "\n" << mesh->vertices[vertIndex].bitangent.y << "\n" << mesh->vertices[vertIndex].bitangent.z << "\n";
		}
		for (int indIndex = 0; indIndex < mesh->indices.size(); ++indIndex)
		{
			file << mesh->indices[indIndex] << "\n";
		}
	}

	file.close();
}
*/
}


int main(int argc, char** argv)
{
	printf("================\nimp loader\n================\nCole Knapp\n-----------------\n");

	/*
	cmd commands 
	f - "directory" export folder of files
	s - "path" export single model
	h - help show commands
	*/



	return 0;
}