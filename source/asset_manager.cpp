#include <asset_manager.hpp>

Mesh AssetManager::getMeshFromDir(const char* mesh_dir)
{
	std::string mesh_name = mesh_dir;
	if (AssetManager::mesh_list.contains(mesh_name))
	{
		return AssetManager::mesh_list[mesh_name];
	}
	std::cout << "ERROR: NO MESH OF SUCH NAME " << mesh_name << std::endl;
	return Mesh();
}

Shader AssetManager::getShaderFromName(const char* shader_name)
{
	std::string shader_name_string = shader_name;
	if (AssetManager::shader_list.contains(shader_name_string))
	{
		return AssetManager::shader_list[shader_name_string];
		std::cout << "Found shader from the list! Returning now." << std::endl;
	}
	std::cout << "ERROR: NO SHADER OF SUCH NAME " << shader_name << std::endl;
	return Shader();
}

Material AssetManager::getMaterialFromName(const char* material_name)
{
	std::string material_name_string = material_name;
	if (AssetManager::material_list.contains(material_name_string))
	{
		return AssetManager::material_list[material_name_string];
	}
	std::cout << "ERROR: NO MATERIAL OF SUCH NAME " << material_name << std::endl;
	return Material();
}


void AssetManager::load_all_meshes()
{
	if (std::filesystem::exists(MODEL_PATH) && std::filesystem::is_directory(MODEL_PATH))
	{
		for (const auto& entry : std::filesystem::directory_iterator(MODEL_PATH))
		{
			if (std::filesystem::is_regular_file(entry.status()) && entry.path().extension() == ".glb")
			{
				Mesh mesh = AssetManager::loadModel(entry.path());
				//renderer.setupMesh(mesh);
				std::string mesh_dir = std::filesystem::relative(entry.path(), MODEL_PATH).generic_string();
				std::cout << "adding " << mesh_dir << " to mesh list." << std::endl;
				AssetManager::mesh_list[mesh_dir] = mesh;
			}
		}
	}
}

void AssetManager::load_all_shaders()
{
	Shader unlitShader = Shader("assets/shaders/unlit.vert", "assets/shaders/unlit.frag", nullptr, shader::UNLIT);
	unlitShader.compile();

	Shader defaultShader = Shader("assets/shaders/default.vert", "assets/shaders/default.frag", nullptr);
	defaultShader.compile();

	unlitShader.use();

	AssetManager::shader_list["default"] = defaultShader;
	AssetManager::shader_list["unlit"] = unlitShader;
}

void AssetManager::load_all_materials()
{
	if (std::filesystem::exists(MATERIAL_PATH) && std::filesystem::is_directory(MATERIAL_PATH))
	{
		for (const auto& entry : std::filesystem::directory_iterator(MATERIAL_PATH))
		{
			if (std::filesystem::is_regular_file(entry.status()) && entry.path().extension() == ".json")
			{
				std::ifstream file(entry.path());
				if (!file.is_open())
				{
					std::cout << "Couldn't open file: " << entry.path() << std::endl;
					continue;
				}
				json material_json;
				file >> material_json;
				Material material = Material();

				try
				{
					material.diffuse_path = material_json.at("diffuse_path").get<std::string>();

					std::string color = material_json.at("color").get<std::string>();
					std::stringstream ss(color);
					glm::vec3 diffuse;
					char comma1, comma2;
					if (ss >> diffuse.x >> comma1 >> diffuse.y >> comma2 >> diffuse.z)
					{
						material.diffuse = diffuse;
					}

					material.setShader(material_json.at("shader_path").get<std::string>().c_str());
				}
				catch(const json::exception& e)
				{
					std::cout << "(approx. L72 of asset_manager.cpp) ERROR: " << e.what() << std::endl;
				}
				
				std::string material_dir = std::filesystem::relative(entry.path(), MATERIAL_PATH).generic_string();
				AssetManager::material_list[material_dir] = material;
			}
		}
	}
}