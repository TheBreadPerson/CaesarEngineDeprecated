#pragma once
#include <unordered_map>
#include <stdio.h>
#include <filesystem>
#include <mesh.hpp>
#include <shader.hpp>
#include <json.hpp>
#include <globals.hpp>
#include <fstream>
#include <iostream>
#include <sstream>

#include <fastgltf/glm_element_traits.hpp>
#include <fastgltf/core.hpp>
#include <fastgltf/types.hpp>

using json = nlohmann::json;


namespace AssetManager
{
	inline const char* MODEL_PATH = "assets/models/";
	inline const char* SHADER_PATH = "assets/shaders/";
	inline const char* MATERIAL_PATH = "assets/materials/";

	inline std::unordered_map<std::string, Mesh> mesh_list;
	inline std::unordered_map<std::string, Shader> shader_list;
	inline std::unordered_map<std::string, Material> material_list;
	unsigned int LoadTexture(const char* filepath);
	Mesh loadModel(std::filesystem::path path);

	void load_all_meshes();
	void load_all_shaders();
	void load_all_materials();

	Mesh getMeshFromDir(const char* mesh_dir);
	Shader getShaderFromName(const char* shader_name);
	Material getMaterialFromName(const char* material_name);

};