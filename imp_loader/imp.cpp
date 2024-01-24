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

IMPORTANT(ck): TODO(ck):
Imp loader for 2d is going to parse Aesprite data and build asset files for 2d. 
also create tilesheets for the engine to load as well

IMPORTANT(ck): README
The assimp-vc142-mtd.dll must be in D:\Programming\$$GameProjects\monster\imp_loader
for some reason... Visual studio must differ the paths it looks for dlls for the
sub projects? the main project requires the dll to be elsewhere.. could be some other issue


- load .obj .fbx and whatever is the easiest to work with file format for 3d models
- export out .imp files with Assimp for now
*/

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/version.h>

#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>

#include <stdio.h>
#include <fstream>
#include <vector>
#include <iostream>
#include <sstream>

#include <time.h>




struct Texture
{
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
		aiString str;
		mat->GetTexture(type, i, &str);
		Texture texture;
		texture.type = typeName;
		texture.path = str.C_Str();
		textures.push_back(texture);

	}
	return textures;
}

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
	glm::vec3 tangent;
	glm::vec3 bitangent;
};

struct Mesh
{
	std::vector<Vertex> vertices;
	std::vector<unsigned short int> indices;
	std::vector<Texture> textures;
};

struct Model
{
	std::vector<std::string> texturePaths;
	std::vector<Mesh> meshes;
	bool gammaCorrection;
};

Mesh ProcessMesh(Model* model, aiMesh* mesh, const aiScene* scene)
{
	// data to fill 
	std::vector<Vertex> vertices;
	std::vector<unsigned short int> indices;
	std::vector<Texture> textures;
	
	for (int i = 0; i < mesh->mNumVertices; ++i)
	{
		Vertex vertex;

		// transfer assimp v3 to ours
		// 
		glm::vec3 vec;
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
			glm::vec2 vec = {};
			// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
			// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.texCoords = vec;
		}
		else
		{
			vertex.texCoords = glm::vec2{ 0.0f, 0.0f };
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
		model->meshes.push_back(ProcessMesh(model, mesh, scene));
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

void WriteModel(Model* model, std::string name, std::string impPath)
{

}

// old garbage format to get Monster engine loading models quickly
void ExportImpFile(Model* model, std::string name, std::string impPath)
{
	std::ofstream file;
	file.open(impPath);

	file << std::fixed;

	for (int i = 0; i < model->meshes.size(); ++i)
	{
		file << name << "\n";

		Mesh* mesh = &model->meshes[i];
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
}

// Write std::vector data directly to load directly into a Vertex* stream.
void WriteBinaryModel(Model* model, const char* filename)
{
	std::ofstream file(filename, std::ios::binary);
	// write all the data
	int verticeCount = model->meshes[0].vertices.size();

	file.write(reinterpret_cast<const char*>(&verticeCount), sizeof(int));
	file.write(reinterpret_cast<const char*>(model->meshes[0].vertices.data()), sizeof(Vertex) * verticeCount);

	int indiceCount = model->meshes[0].indices.size();
	file.write(reinterpret_cast<const char*>(&indiceCount), sizeof(int));
	file.write(reinterpret_cast<const char*>(model->meshes[0].indices.data()), sizeof(unsigned short int) * indiceCount);

	file.close();
}

// Testing methods
// how to ensure test is correct does it need set params to test properly??
// how do i automate it? what am i looking for to ensure a model is loaded correctly
void LoadBinaryModel(const char* filename)
{
	// ensure file can be loaded properly for the engine
	// this keeps the test code out of the engine
	std::ifstream file(filename, std::ios::binary);
	int verticeCount;
	file.read(reinterpret_cast<char*>(&verticeCount), sizeof(int));

	Vertex* vertices = new Vertex[verticeCount];
	file.read(reinterpret_cast<char*>(vertices), sizeof(Vertex) * verticeCount);

	int indiceCount;
	file.read(reinterpret_cast<char*>(&indiceCount), sizeof(int));
	
	unsigned short int* indices = new unsigned short int[indiceCount];
	file.read(reinterpret_cast<char*>(indices), sizeof(unsigned short int) * indiceCount);

	file.close();

	delete[] vertices;
	delete[] indices;

}

// WRITING TO A BINARY FILE
/*
#include <iostream>
#include <fstream>

int main() {
	int myArray[] = {1, 2, 3, 4, 5};
	int arraySize = sizeof(myArray) / sizeof(myArray[0]);

	std::ofstream outFile("myArray.bin", std::ios::binary);
	if (!outFile) {
		std::cerr << "Failed to open the file for writing." << std::endl;
		return 1;
	}

	// Write the array size as binary data
	outFile.write(reinterpret_cast<const char*>(&arraySize), sizeof(int));

	// Write the array as binary data
	outFile.write(reinterpret_cast<const char*>(myArray), sizeof(int) * arraySize);

	outFile.close();

	return 0;
}
*/

// READING FROM A BINARY FILE
/*
#include <iostream>
#include <fstream>

int main() {
	std::ifstream inFile("myArray.bin", std::ios::binary);
	if (!inFile) {
		std::cerr << "Failed to open the file for reading." << std::endl;
		return 1;
	}

	int arraySize;
	inFile.read(reinterpret_cast<char*>(&arraySize), sizeof(int));

	if (arraySize <= 0) {
		std::cerr << "Invalid array size in the file." << std::endl;
		return 1;
	}

	int* loadedArray = new int[arraySize];
	inFile.read(reinterpret_cast<char*>(loadedArray), sizeof(int) * arraySize);

	inFile.close();

	// Now, 'loadedArray' contains the data from the binary file.

	// Don't forget to release the dynamically allocated memory when you're done.
	delete[] loadedArray;

	return 0;
}
*/

struct Date
{
	int year;
	int month;
	int day;
	int hour;
	int min;

	std::string toString()
	{
		std::stringstream result;

		result << (year) << "-" << (month) << "-" << day << " " << hour << ":" << min;
		return result.str();
	}

	std::string fileName()
	{
		std::stringstream result;

		result << (year) << "-" << (month) << "-" << day << ".log";
		return result.str();
	}
};

enum class Severity
{
	ERROR,
	WARN,
	TRACE
};

struct Log
{
	std::ofstream file;
	std::string name;
};

void InitLogger(Log* log, const char* fileName)
{
	log->file = std::ofstream(fileName);
	log->name = fileName;
}

void CleanLogger(Log* log)
{
	printf("[INFO]: Cleaning the logger...");
	log->file << "[INFO]: Cleaning the logger... Closing file: " << log->name << "\n";

	log->file.close();
	delete log;
}

void Error(Log* log, const char* msg)
{
	printf("[ERROR]: %s\n", msg);
	log->file << "[ERROR]: " << msg << "\n";
}

void Warn(Log* log, const char* msg)
{
	printf("[WARN]: %s\n", msg);
	log->file << "[WARN]: " << msg << "\n";
}

void Trace(Log* log, const char* msg)
{
	printf("[TRACE]: %s\n", msg);
	log->file << "[TRACE]: " << msg << "\n";
}

void PrintLog(Log* log, Severity severity, const char* msg)
{
	switch (severity)
	{
		case Severity::ERROR:
			Error(log, msg);
			break;
		case Severity::WARN:
			Warn(log, msg);
			break;
		case Severity::TRACE:
			Trace(log, msg);
			break;
		default:
			Error(log, msg);
			break;
	}
}

void PrintLog(Log* log, const char* msg, bool printLabel = true)
{
	if (printLabel)
	{
		printf("[INFO]: %s\n", msg);
		log->file << "[INFO]: " << msg << "\n";
	}
	else
	{
		printf("%s\n", msg);
		log->file << msg << "\n";
	}
}

int main(int argc, char** argv)
{

	/*
	Make this multi threaded?
	yeah do this. its doable you just need to make sure
	Assimp::Importer is used for each thread.
	this would look GREAT on portfolio
	
	*/

	// TODO(ck): LOG THIS keep a data file of all the asset changes and stuff?
	time_t t = time(NULL);
	tm dateTime;
	localtime_s(&dateTime, &t);
	Date date = {};
	date.year = dateTime.tm_year+1900;
	date.month = dateTime.tm_mon+1;
	date.day = dateTime.tm_mday;
	date.hour = dateTime.tm_hour;
	date.min = dateTime.tm_min;

	Log* log = new Log();
	InitLogger(log, date.fileName().c_str());
	
	PrintLog(log, "============================\n\timp loader\n============================\n\tCole Knapp\n----------------------------", false);

	std::string dateLabel = "Current date: " + date.toString();
	PrintLog(log, dateLabel.c_str());

	std::string majorVersion = std::to_string(aiGetVersionMajor());
	std::string minorVersion = std::to_string(aiGetVersionMinor()); 
	std::string revVersion = std::to_string(aiGetVersionRevision()); // "point release", "subminor"
	//std::string assimpLegal = aiGetLegalString();
	std::string assimpVersion = "Current Assimp Version: " + majorVersion + "." + minorVersion + "." + revVersion;
	PrintLog(log, assimpVersion.c_str());

	/*
	cmd commands 
	f - "directory" export folder of files
	s - "path" export single model
	h - help show commands
	*/
	Model model = {};

	// Batch 2-4 models on each thread?
	/*
	Refer to ck_notes for checklist
	1. Read the folder location.
	2. Choose how many models you want per thread... 
	   perform some calculation to split up the work?
	   obj files / 2 for now to split the work up in half
	3. launch LoadModel tasks and output a model...
	   not sure how to do this thread safe? is it okay to just output &model or
	   do you
	
	
	*/

	// loop over models folder and export them out
	LoadModel(&model, "models/cube/blender_cube.obj", false);
	WriteBinaryModel(&model, "cube.imp");

	model = {};
	LoadModel(&model, "models/sphere/sphere.obj", false);
	WriteBinaryModel(&model, "sphere.imp");

	model = {};
	LoadModel(&model, "models/cone/cone.obj", false);
	WriteBinaryModel(&model, "cone.imp");
	
	model = {};
	LoadModel(&model, "models/waddledee_new/waddledee.obj", false);
	WriteBinaryModel(&model, "waddle.imp");

	model = {};
	LoadModel(&model, "models/wolly/wolly.obj", false);
	WriteBinaryModel(&model, "wolly.imp");
	
	
	LoadBinaryModel("sphere.imp");
	LoadBinaryModel("cone.imp");
	LoadBinaryModel("cube.imp");
	LoadBinaryModel("waddle.imp");
	LoadBinaryModel("wolly.imp");


	//model = {};
	//LoadModel(&model, "models/cube/blender_cube.obj", false);

	// Export config file to a binary file that the engine can read easily
	// the config file can be human readable and easy to edit
	// this way people can edit the file themselves (think modding support)

	// LOG EVERY MODEL BEING PRINTED
	/*
	PrintLog(log, Severity::TRACE, "Loading: models/grass_star/Grass.obj");

	model = {};
	LoadModel(&model, "models/grass_star/Grass.obj", false);
	ExportImpFile(&model, "grass", "test_grass.imp");

	model = {};
	LoadModel(&model, "models/tree/my_tree.obj", false);
	ExportImpFile(&model, "tree", "tree.imp");

	model = {};
	LoadModel(&model, "models/plane_64/plane_64.obj", false);
	ExportImpFile(&model, "plane_64", "plane_64.imp");

	model = {};
	LoadModel(&model, "models/gem/gem_1x1.obj", false);
	ExportImpFile(&model, "gem", "gem_1x1.imp");

	model = {};
	LoadModel(&model, "models/light_sphere/light_sphere.obj", false);
	ExportImpFile(&model, "light_sphere", "light_sphere.imp");

	model = {};
	LoadModel(&model, "models/village_house/village_house_combined.obj", false);
	ExportImpFile(&model, "village_house_combined", "village_house.imp");

	model = {};
	LoadModel(&model, "models/pumpkin/new_pumpkin.obj", false);
	ExportImpFile(&model, "pumpkin", "pumpkin.imp");


	model = {};
	LoadModel(&model, "models/tall_monster/tall_monster.obj", false);
	ExportImpFile(&model, "tall_monster", "tall_monster.imp");

	*/
	// Verify Exported Imp Files before check for (nan) and exponents in the data
	// Export out to asset folder location as well and the debug folder for RenderDoc

	// Asset file
	/* NOTE(ck): Stores path to .imp (model file)? 
				 Stores image assets
				 Stores shader information 
				 Stores textures with index to image asset texture type, is pixel texture, shader index, image asset Index


		Need a way to map indexes to friendly names to make gui easy to read and creating assets easy as well
		LoadTexture(t1, MonGL::TextureType::Diffuse, true, shaderID, GetImage(g_Assets, 1));

	*/

	// TODO WEEKEND PROJECT 
	// WRITE A LITTLE CONSOLE COMMAND WINDOW THAT CAN BE USED TO 
	// OR EVEN BETTER WRITE AN API THAT ALLOWS YOU TO CALL FUNCTIONS FROM OUR GUI TO USE IT!
	// LAUNCH THE EXE AND SEND COMMANDS TO IT


	/*
	Allocate tilemap and put buffers into binary file for direct loading?
	
	*/

	CleanLogger(log);

	return 0;
}