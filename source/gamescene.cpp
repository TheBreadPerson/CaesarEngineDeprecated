#include <gamescene.hpp>
#include <collision.hpp>
#include <transform.hpp>
#include <renderer.hpp>
#include <player.hpp>
#include <mesh.hpp>
#include <iostream>
#include <globals.hpp>
#include <physics.hpp>
#include <light.hpp>
#include <asset_manager.hpp>

Mesh skybox;

MeshRenderer planeRenderer;
MeshRenderer cubeRenderer;
MeshRenderer wallRenderer;

Material defaultMaterial;
Material wallpaperMaterial;
Material unlitMaterial;
Material lightMaterial;

Material grassMaterial;
Material metalFloorMaterial;

Entity player_ent;
Entity box1_ent;
Entity wall_ent;
Entity globe_ent;
Entity plane_ent;
Entity monkey_ent;
Entity light_ent;
Entity light2;

Entity* testEntity;

GameScene::GameScene()
{

}

void GameScene::Start()
{
	name = "game";

	//defaultMaterial.diffuse = vec4(1.0f);
	//defaultMaterial.diffuse_path = "assets/container.png";
	////defaultMaterial.specularMap = AssetManager::LoadTexture("assets/specular2.png");
	//defaultMaterial.setShader("default");
	//defaultMaterial.save("assets/materials/default_material.json");

	wallpaperMaterial.diffuse = vec4(1.0f);
	wallpaperMaterial.diffuse_path = "assets/wallpapertest.png";
	wallpaperMaterial.shader_path = "default";
	wallpaperMaterial.shader = AssetManager::shader_list[wallpaperMaterial.shader_path];

	grassMaterial.diffuse_path = "assets/realgrass.jpg";
	grassMaterial.specularMap = AssetManager::LoadTexture("assets/grassspecular.png");
	grassMaterial.specular = vec3(2.0f);
	grassMaterial.shininess = 32.0f;
	grassMaterial.setShader("default");
	grassMaterial.save("assets/materials/grass_material.json");

	metalFloorMaterial.diffuse_path = "assets/earth.png";
	metalFloorMaterial.specular = vec3(2.0f);
	metalFloorMaterial.shininess = 32.0f;
	metalFloorMaterial.setShader("default");

	unlitMaterial.diffuse = vec4(1.0f);
	unlitMaterial.setShader("unlit");

	planeRenderer.mesh_path = "Cube.glb";
	planeRenderer.material = grassMaterial;;

	cubeRenderer.mesh_path = "sphere.glb";
	cubeRenderer.setMaterial("default_material.json");

	wallRenderer.mesh_path = "Cube.glb";
	wallRenderer.setMaterial("default_material.json");

	player.entity.transform.scale = vec3(1.0f, 1.0f, 1.0f);
	player.entity.AddComponent<Rigidbody>();
	player.entity.AddComponent<Collider>();
	player.entity.GetComponent<Collider>()->scale = vec3(1.0f, 3.0f, 1.0f);
	player.entity.name = "Player";
	
	plane_ent.transform.position = vec3(0.0f, 0.0f, 0.0f);
	plane_ent.transform.scale = vec3(20.0f, 20.0f, 20.0f);
	plane_ent.AddComponent<MeshRenderer>(planeRenderer);
	plane_ent.AddComponent<Collider>();
	plane_ent.GetComponent<Collider>()->scale = vec3(20.0f, 20.0f, 20.0f);
	plane_ent.name = "Floor";

	box1_ent.transform.position = vec3(5.0f, 15.0f, 0.0f);
	box1_ent.transform.scale = vec3(1.0f, 1.0f, 1.0f);
	box1_ent.AddComponent<MeshRenderer>(cubeRenderer);
	box1_ent.AddComponent<Collider>();
	box1_ent.AddComponent<Rigidbody>();
	box1_ent.GetComponent<Collider>()->scale = vec3(1.0f, 1.0f, 1.0f);
	box1_ent.name = "Box";

	wall_ent.transform.position = vec3(-15.0f, 25.0f, 0.0f);
	wall_ent.transform.scale = vec3(2.0f, 5.0f, 15.0f);
	wall_ent.AddComponent<MeshRenderer>(wallRenderer);
	wall_ent.AddComponent<Collider>();
	wall_ent.GetComponent<Collider>()->scale = vec3(2.0f, 5.0f, 15.0f);
	wall_ent.name = "Wall";

	light_ent.AddComponent<Light>();
	light_ent.GetComponent<Light>()->diffuse = vec3(1.0f);
	light_ent.transform.position = vec3(5.0f, 15.0f, 0.0f);
	light_ent.name = "Light";

	sceneData.skybox_ent = new Entity();
	sceneData.skybox_ent->name = "Sky";
	skybox = AssetManager::loadModel("assets/models/sphere.glb");

	Material skyboxMaterial = unlitMaterial;
	skyboxMaterial.diffuse_path = "assets/sky.jpg";

	sceneData.skybox_ent->AddComponent<MeshRenderer>();
	sceneData.skybox_ent->GetComponent<MeshRenderer>()->mesh_path = "sphere.glb";
	sceneData.skybox_ent->GetComponent<MeshRenderer>()->setMaterial("sky_material.json");
	//sceneData.skybox_ent->GetComponent<MeshRenderer>()->material = skyboxMaterial;

	sceneData.skybox_ent->transform.position = vec3(0.0f, 0.0f, 0.0f);
	sceneData.skybox_ent->transform.rotation = vec3(0.0f, 0.0f, 0.0f);
	sceneData.skybox_ent->transform.scale = vec3(200.0f, 200.0f, 200.0f);

	sceneData.sceneLighting.ambient = vec3(0.1f);

	Instantiate(&player.entity);
	Instantiate(&plane_ent);
	Instantiate(&wall_ent);
	Instantiate(&box1_ent);
	Instantiate(&light_ent);
	sceneData.lights.push_back(light_ent.GetComponent<Light>());

	Instantiate(*&sceneData.skybox_ent);
	std::cout << sceneManager.currentScene->sceneData.entityList.size() << std::endl;
	sceneManager.saveScene(sceneManager.currentScene);
}

void GameScene::Update()
{
	
	if (!sceneData.entityList.empty())
	{
		for (Entity* object : sceneData.entityList)
		{
			if (!object->HasComponent<Collider>() || !object->GetComponent<Collider>()->enabled)
			{
				continue;
			}
			for (Entity* physics_object : sceneData.entityList)
			{
				if (!physics_object->HasComponent<Rigidbody>() || object == physics_object || !physics_object->GetComponent<Rigidbody>()->enabled)
				{
					continue;
				}
				CheckCollision(physics_object->GetComponent<Collider>(), object->GetComponent<Collider>());
			}
		}
	}
	cam.move();
	player.move();
	light_ent.transform.position = player.entity.transform.position;
	sceneData.skybox_ent->transform.position = player.entity.transform.position;

	globe_ent.transform.rotation.x += Time::deltaTime * 5.0f;
}

void GameScene::FixedUpdate(double deltaTime)
{
	for (Entity* physics_object : sceneData.entityList)
	{
		if (!physics_object->HasComponent<Rigidbody>() || !physics_object->GetComponent<Rigidbody>()->enabled)
		{
			continue;
		}
		PhysicsUpdate(*physics_object, deltaTime);
	}
}

void GameScene::OnDisable()
{

}