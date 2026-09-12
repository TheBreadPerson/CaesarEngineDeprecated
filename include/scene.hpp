#pragma once

#include <memory>
#include <globals.hpp>
#include <vector>
#include <light.hpp>
#include <json.hpp>

using json = nlohmann::json;

struct SceneData
{
	std::vector<Entity*> entityList;
	std::vector<Light*> lights;
	Entity* skybox_ent;
	SceneLight sceneLighting;
};

class Scene
{
public:
	virtual void OnDisable() = 0;
	virtual void Start() = 0;
	virtual void Update() {};
	virtual void FixedUpdate(double deltaTime) {};
	SceneData sceneData;
	std::string name = "scene";
};

class SceneManager
{
public:
	SceneManager();
	SceneManager(std::shared_ptr<Scene> scene);
	void openScene(std::shared_ptr<Scene> scene);
	void closeScene();
	void Update();
	void FixedUpdate(double deltaTime);
	std::shared_ptr<Scene> currentScene;

	void saveScene(std::shared_ptr<Scene> _scene);
	SceneData loadScene(const char* scene_path);
};

extern SceneManager sceneManager;