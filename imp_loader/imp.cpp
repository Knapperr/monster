/*

     ________  _________   _     _____  ___ ______ ___________
    |_   _|  \/  || ___ \ | |   |  _  |/ _ \|  _  \  ___| ___ \
      | | | .  . || |_/ / | |   | | | / /_\ \ | | | |__ | |_/ /
      | | | |\/| ||  __/  | |   | | | |  _  | | | |  __||    /
 _   _| |_| |  | || |     | |___\ \_/ / | | | |/ /| |___| |\ \
(_)  \___/\_|  |_/\_|     \_____/\___/\_| |_/___/ \____/\_| \_| 

*/

/*
- load .obj .fbx and whatever is the easiest to work with file format for 3d models
- export out .imp files with Assimp for now

Assimp is heavy I do not want it in the main engine. Eventually I would like to write
the loader on my own and not rely on assimp. Remember that we can use this project in finnsie
the best thing to do is have this project loading the file and call it from monster instead of 
having an import/export logic in Monster
*/
/*
GameObject* obj = g_Game->objects[selected];
for (int i = 0; i < obj->model->meshes.size(); ++i)
{
	file << obj->name << "\n";

	Mesh* mesh = &obj->model->meshes[i];
	for (int j = 0; j < mesh->vertices.size(); ++j)
	{

		file << mesh->vertices[j].position.x << "\n" << mesh->vertices[j].position.y << "\n" << mesh->vertices[j].position.z << "\n"
				<< mesh->vertices[j].normal.x << "\n" << mesh->vertices[j].normal.y << "\n" << mesh->vertices[j].normal.z << "\n"
				<< mesh->vertices[j].texCoords.x << "\n" << mesh->vertices[j].texCoords.y << "\n"
				<< mesh->vertices[j].tangent.x << "\n" << mesh->vertices[j].tangent.y << "\n" << mesh->vertices[j].tangent.z << "\n"
				<< mesh->vertices[j].bitangent.x << "\n" << mesh->vertices[j].bitangent.y << "\n" << mesh->vertices[j].bitangent.z << "\n";
	}
}
*/


