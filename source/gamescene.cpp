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
	Mesh planeMesh = renderer.getMeshFromDir("Plane.glb");
	Mesh cubeMesh = renderer.getMeshFromDir("Cube.glb");


	defaultMaterial.diffuse = vec4(1.0f);
	defaultMaterial.diffuseMap = graphics::LoadTexture("assets/container.png");
	defaultMaterial.specularMap = graphics::LoadTexture("assets/specular2.png");
	defaultMaterial.shader = 6;

	wallpaperMaterial.diffuse = vec4(1.0f);
	wallpaperMaterial.diffuseMap = graphics::LoadTexture("assets/wallpapertest.png");
	wallpaperMaterial.shader = 6;

	grassMaterial.diffuseMap = graphics::LoadTexture("assets/realgrass.jpg");
	grassMaterial.specularMap = graphics::LoadTexture("assets/grassspecular.png");
	grassMaterial.specular = vec3(2.0f);
	grassMaterial.shininess = 32.0f;
	grassMaterial.shader = 6;

	metalFloorMaterial.diffuseMap = graphics::LoadTexture("assets/Wood.jpg");
	//metalFloorMaterial.specularMap = graphics::LoadTexture("assets/metal_specular.png");
	metalFloorMaterial.specular = vec3(2.0f);
	metalFloorMaterial.shininess = 32.0f;
	metalFloorMaterial.shader = 6;

	unlitMaterial.diffuse = vec4(1.0f);
	unlitMaterial.shader = 3;

	planeRenderer.mesh_path = "Cube.glb";
	planeRenderer.material = metalFloorMaterial;

	cubeRenderer.mesh_path = "sphere.glb";
	cubeRenderer.material = defaultMaterial;

	wallRenderer.mesh_path = "Cube.glb";
	wallRenderer.material = wallpaperMaterial;

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

	currentScene.skybox_ent = new Entity();
	currentScene.skybox_ent->name = "Sky";
	skybox = graphics::loadModel("assets/models/sphere.glb");

	Material skyboxMaterial = unlitMaterial;
	skyboxMaterial.diffuseMap = graphics::LoadTexture("assets/sky.jpg");

	currentScene.skybox_ent->AddComponent<MeshRenderer>();
	currentScene.skybox_ent->GetComponent<MeshRenderer>()->mesh = skybox;
	currentScene.skybox_ent->GetComponent<MeshRenderer>()->material = skyboxMaterial;

	currentScene.skybox_ent->transform.position = vec3(0.0f, 0.0f, 0.0f);
	currentScene.skybox_ent->transform.rotation = vec3(0.0f, 0.0f, 0.0f);
	currentScene.skybox_ent->transform.scale = vec3(200.0f, 200.0f, 200.0f);

	currentScene.sceneLighting.ambient = vec3(0.1f);

	Instantiate(&player.entity);
	Instantiate(&plane_ent);
	//plane_ent.GetComponent<MeshRenderer>()->mesh = cubeMesh;
	Instantiate(&wall_ent);
	Instantiate(&box1_ent);
	Instantiate(&light_ent);
	currentScene.lights.push_back(light_ent.GetComponent<Light>());

	Instantiate(*&currentScene.skybox_ent);
}

void GameScene::Update()
{
	
	if (!currentScene.entityList.empty())
	{
		for (Entity* object : currentScene.entityList)
		{
			if (!object->HasComponent<Collider>() || !object->GetComponent<Collider>()->enabled)
			{
				continue;
			}
			for (Entity* physics_object : currentScene.entityList)
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
	currentScene.skybox_ent->transform.position = player.entity.transform.position;

	globe_ent.transform.rotation.x += Time::deltaTime * 5.0f;
}

void GameScene::FixedUpdate(double deltaTime)
{
	for (Entity* physics_object : currentScene.entityList)
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