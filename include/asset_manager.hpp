#pragma once
#include <unordered_map>
#include <stdio.h>
#include <mesh.hpp>
#include <shader.hpp>

namespace AssetManager
{
	inline std::unordered_map<std::string, Mesh> mesh_list;
	inline std::unordered_map<std::string, Shader> shader_list;
	unsigned int LoadTexture(const char* filepath);
	Mesh loadModel(std::filesystem::path path);

	inline Mesh getMeshFromDir(const char* mesh_dir)
	{
		std::string mesh_name = mesh_dir;
		if (AssetManager::mesh_list.contains(mesh_name))
		{
			return AssetManager::mesh_list[mesh_name];
		}

		return Mesh();
	}
	inline Shader getShaderFromName(const char* shader_name)
	{
		std::string shader_name_string = shader_name_string;
		if (AssetManager::shader_list.contains(shader_name_string))
		{
			return AssetManager::shader_list[shader_name_string];
			std::cout << "Found shader from the list! Returning now." << std::endl;
		}
		std::cout << "ERROR: NO SHADER OF SUCH NAME " << shader_name << std::endl;
		return Shader();
	}

};