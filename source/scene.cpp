#include <scene.hpp>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <globals.hpp>

SceneManager::SceneManager() : currentScene(nullptr)
{

}

SceneManager::SceneManager(std::shared_ptr<Scene> scene)
{
	openScene(scene);
}

void SceneManager::openScene(std::shared_ptr<Scene> scene)
{
	if (currentScene)
	{
		currentScene->OnDisable();

		currentScene = scene;

		currentScene->Start();
	}
	else
	{
		currentScene = scene;
		if (currentScene) // Ensure currentScene is not nullptr
		{
			currentScene->Start();  // Safe to call Start now
		}
		else
		{
			std::cerr << "Error: currentScene is nullptr!" << std::endl;
		}
	}
}

void SceneManager::closeScene()
{
	if (currentScene)
	{
		currentScene->OnDisable();
		currentScene = nullptr;
	}
}

void SceneManager::Update()
{
	if (currentScene)
	{
		currentScene->Update();
	}
}

void SceneManager::FixedUpdate(double deltaTime)
{
	if (currentScene)
	{
		currentScene->FixedUpdate(deltaTime);
	}
}

void SceneManager::saveScene(std::shared_ptr<Scene> _scene)
{
	std::ofstream file("assets/scenes/" + _scene->name + ".json");
	SceneData sceneData = _scene->sceneData;
	std::cout << sceneData.entityList.size() << std::endl;
	json scene_json;
	json entities_json;

	for (auto& const entity : sceneData.entityList)
	{
		json entity_json;
		entity_json["ID"] = std::format("{}", entity->GetID());

		json transform_json;
		transform_json["position"] = std::format("{},{},{}", entity->transform.position.x, entity->transform.position.y, entity->transform.position.z);
		transform_json["scale"] = std::format("{},{},{}", entity->transform.scale.x, entity->transform.scale.y, entity->transform.scale.z);
		transform_json["rotation"] = std::format("{},{},{}", entity->transform.rotation.x, entity->transform.rotation.y, entity->transform.rotation.z);
		entity_json["transform"] = transform_json;

		if (entity->hasComponents())
		{
			json components_json;
			if (entity->HasComponent<MeshRenderer>())
			{
				json component_json;
				// Handle each component individually... yeah ik chud shit but whatever
				MeshRenderer* mrenderer = entity->GetComponent<MeshRenderer>();
				component_json["mesh_path"] = mrenderer->mesh_path;
				component_json["material"] = mrenderer->material_path;
				components_json["MeshRenderer"] = component_json;
			}

			entity_json["components"] = components_json;
		}
		entities_json[entity->name] = entity_json;
	}
	scene_json["entities"] = entities_json;

	
	file << std::setw(4) << scene_json << std::endl;
	file.close();
}

SceneData SceneManager::loadScene(const char* scene_path)
{
	return SceneData();
	/*if (std::filesystem::exists(scene_path) && std::filesystem::is_regular_file(scene_path))
	{
		std::ifstream file(scene_path);
		if (!file.is_open())
		{
			std::cout << "Couldn't open scene file: " << scene_path << std::endl;
			return SceneData();
		}
		json scene_json;
		file >> scene_json;
		SceneData sceneData = SceneData();

		try
		{
			sceneData.diffuse_path = material_json.at("diffuse_path").get<std::string>();

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
		catch (const json::exception& e)
		{
			std::cout << "(approx. L72 of asset_manager.cpp) ERROR: " << e.what() << std::endl;
		}

		std::string material_dir = std::filesystem::relative(entry.path(), MATERIAL_PATH).generic_string();
		AssetManager::material_list[material_dir] = material;
	}*/
}