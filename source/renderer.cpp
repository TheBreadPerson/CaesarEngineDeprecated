#include <glad/glad.h>
#include <iostream>
#include <GLFW/glfw3.h>
#include <memory>
#include "renderer.hpp"
#include "inputs.hpp"
#include "camera.hpp"
#include "globals.hpp"
#include "scene.hpp"
#include "gamescene.hpp"
#include "shader.hpp"
#include "entity.hpp"
#include <light.hpp>
#include <debug.hpp>
#include <stb_image.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include <filesystem>

using namespace glm;


unsigned int vertexShader;
unsigned int fragmentShader;

unsigned int shaderProgram;
unsigned int defaultShaderID;

unsigned int woodTexture, crateTexture;

int textureUnits[16] = { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15 };

vec3 cubePositions[] =
{
	vec3(0.0f,  0.0f,  0.0f),
	vec3(2.0f,  2.0f, 0.0f),
	vec3(-2.0f, -2.0f, 0.0f),
	vec3(2.0f, -2.0f, 0.0f),
	vec3(-2.0f, 2.0f, 0.0f)
};


void Renderer::init()
{
	//glEnable(GL_FRAMEBUFFER_SRGB);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_COLOR);
	glEnable(GL_DEPTH_TEST);

	cam.transform.position = vec3(0.0f, 0.0f, 0.0f);

	// Loop through models folder and setup all meshes, added their mesh data to meshes vector
	/*if (std::filesystem::exists(models_path) && std::filesystem::is_directory(models_path))
	{
		for (const auto& entry : std::filesystem::directory_iterator(models_path))
		{
			if (std::filesystem::is_regular_file(entry.status()) && entry.path().extension() == ".glb")
			{
				Mesh mesh = AssetManager::loadModel(entry.path());
				setupMesh(mesh);
				std::string mesh_dir = std::filesystem::relative(entry.path(), models_path).generic_string();
				std::cout << "adding " << mesh_dir << " to mesh list." << std::endl;
				AssetManager::mesh_list[mesh_dir] = mesh;
			}
		}
	}*/
	
	/*Shader unlitShader = Shader("assets/shaders/unlit.vert", "assets/shaders/unlit.frag", nullptr, shader::UNLIT);
	unlitShader.compile();

	Shader defaultShader = Shader("assets/shaders/default.vert", "assets/shaders/default.frag", nullptr);
	defaultShader.compile();

	unlitShader.use();

	AssetManager::shader_list["default"] = defaultShader;
	AssetManager::shader_list["unlit"] = unlitShader;*/

	glClearColor(0.01f, 0.01f, 0.01f, 1.0f);
	glViewport(0, 0, screen_width, screen_height);
}

void Renderer::draw(GLFWwindow* window)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (Input::GetKeyDown(KeyCode::R))
	{
		reloadShaders();
	}

	float timeValue = static_cast<float>(glfwGetTime());

	mat4 projection = perspective(radians(cam.fov), ((float)screen_width / screen_height), 0.01f, 1000.0f);
	mat4 view = lookAt(cam.transform.position, cam.transform.position + cam.transform.forward, cam.transform.up);

	SceneData sceneData = sceneManager.currentScene->sceneData;

	for (Entity* object : sceneData.entityList)
	{
		if (!object->HasComponent<MeshRenderer>())
		{
			continue;
		}
		MeshRenderer* meshRenderer = object->GetComponent<MeshRenderer>();
		if (meshRenderer->material.diffuse_map == 0)
		{
			meshRenderer->material.diffuse_map = AssetManager::LoadTexture(meshRenderer->material.diffuse_path.c_str());
		}
		Shader* shader = &meshRenderer->material.shader;

		shader->use();

		// SHADER CONSTANTS
		shader->setMat4("projection", projection);
		shader->setMat4("view", view);
		shader->setVec4("objColor", vec4(meshRenderer->material.diffuse, 1.0f));
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, meshRenderer->material.diffuse_map);
		shader->setInt("texture2d", 0);
		mat4 model = setupTransform(object->transform);
		shader->setMat4("model", model);

		bool useTexture = (meshRenderer->material.diffuse_map != 0);
		shader->setBool("useTexture", useTexture);
		// SHADER CONSTANTS

		if (shader->shaderType == shader::DEFAULT)
		{
			// Apply scene lighting
			for (int i = 0; i < sceneData.lights.size(); i++)
			{
				std::string lightName = "pointLights[" + std::to_string(i) + "]";
				if (sceneData.lights[i] != nullptr)
				{
					shader->setVec3(lightName + ".position", sceneData.lights[i]->entity->transform.position);
				}
				else
				{
					std::cout << "Light is null" << std::endl;
				}
				shader->setVec3(lightName + ".ambient",sceneData.lights[i]->ambient);
				shader->setVec3(lightName + ".diffuse",sceneData.lights[i]->diffuse);
				shader->setVec3(lightName + ".specular",sceneData.lights[i]->specular);
				shader->setFloat(lightName + ".constant", 1.0f);
				shader->setFloat(lightName + ".linear", 1.0f / sceneData.lights[i]->radius);
				shader->setFloat(lightName + ".quadratic", 1.0f / pow(sceneData.lights[i]->radius, 2));
				shader->setFloat(lightName + ".intensity",sceneData.lights[i]->intensity);
			}
			shader->setVec3("sceneLight.ambient",sceneData.sceneLighting.ambient);

			// Activate and bind the emission map
			glActiveTexture(GL_TEXTURE0); // Activate texture unit 0
			glBindTexture(GL_TEXTURE_2D, meshRenderer->material.emissionMap); // Bind emission map
			shader->setInt("material.emissionMap", 0); // Set the sampler uniform to texture unit 0
			shader->setVec3("material.emission", meshRenderer->material.emission);
			glActiveTexture(GL_TEXTURE1); // Activate texture unit 1
			glBindTexture(GL_TEXTURE_2D, meshRenderer->material.diffuse_map); // Bind diffuse map
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			shader->setInt("material.diffuseMap", 1); // Set the sampler uniform to texture unit 1
			shader->setVec3("material.diffuse", meshRenderer->material.diffuse);
			// Activate and bind the specular map
			glActiveTexture(GL_TEXTURE2); // Activate texture unit 2
			glBindTexture(GL_TEXTURE_2D, meshRenderer->material.specularMap); // Bind specular map
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			shader->setInt("material.specularMap", 2); // Set the sampler uniform to texture unit 2
			shader->setVec3("material.specular", meshRenderer->material.specular);
			// Set shininess value
			shader->setFloat("material.shininess", meshRenderer->material.shininess);

			shader->setBool("useTexture", (meshRenderer->material.diffuse_map != 0));

			shader->setVec3("viewPos", cam.transform.position);
		}
		if (object->GetID() ==sceneData.skybox_ent->GetID()) glDepthMask(GL_FALSE); // If object is skybox, disable depth
		render(*object);
		glDepthMask(GL_TRUE);
	}
	
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	glfwSwapBuffers(window);
}

void Renderer::reloadShaders()
{
	/*for (auto& [name, shader] : AssetManager::shader_list)
	{
		shader.reload();
	}*/
}

void Renderer::cleanup()
{
	AssetManager::shader_list.clear();
}