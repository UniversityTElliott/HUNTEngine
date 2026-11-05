#pragma once
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <unordered_map>



namespace HUNT
{
	//IDk if I like this impl stuff ASK GAZ WHY/SHOUYLD KEEP?
	class Renderer;
	class Texture;
	class Mesh;
	class Material;
	class Model;
	class MeshData;
	class Material;


	class AssetManager
	{
	public:
		 AssetManager(Renderer* renderer);
		~AssetManager();

		//Loads single mesh models.
		 void LoadTextureToAssetManager(const char* path, std::string name);
		 void LoadModelToAssetManager(const char* path, std::string name);
		 void LoadMeshToAssetManager(MeshData meshDataIn, Material materialIn, std::string name);
		 //All of these are pointers as they are owned HERE or by RENDERER and everything else just peeks at them
		 Texture* GetTexture(const std::string& name);
		 Model* GetModel(const std::string& name);
		 Mesh* GetMesh(const std::string& name);

	private:

		struct Impl;
		std::unique_ptr<Impl> impl_;

	};
}
