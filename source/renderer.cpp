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

Shader defaultShader("assets/shaders/default.vert", "assets/shaders/default.frag");
Shader unlitShader("assets/shaders/unlit.vert", "assets/shaders/unlit.frag");
std::vector<Shader> shaders;

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

	const char* path = "assets/models/";
	// Loop through models folder and setup all meshes, added their mesh data to meshes vector
	if (std::filesystem::exists(path) && std::filesystem::is_directory(path))
	{
		for (const auto& entry : std::filesystem::directory_iterator(path))
		{
			if (std::filesystem::is_regular_file(entry.status()) && entry.path().extension() == ".glb")
			{
				meshes.push_back(graphics::loadModel(entry.path()));
				mesh_dirs.push_back(std::filesystem::relative(entry.path(), path).generic_string());
			}
		}
	}

	for (Entity* object : currentScene.entityList)
	{
		if (!object->HasComponent<MeshRenderer>())
		{
			continue;
		}
		setupMesh(object->GetComponent<MeshRenderer>()->mesh);
	}

	unlitShader.compile();
	defaultShader.compile();

	shaders.push_back(defaultShader);
	shaders.push_back(unlitShader);

	unlitShader.use();

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

	for (Shader shader : shaders)
	{
		shader.use();

		shader.setMat4("projection", projection);
		shader.setMat4("view", view);

		for (Entity* object : currentScene.entityList)
		{
			if (!object->HasComponent<MeshRenderer>())
			{
				continue;
			}
			
			MeshRenderer* objMesh = object->GetComponent<MeshRenderer>();
			//shader.setBool("useTexture", objMesh->hasTexture);
			shader.setVec4("objColor", vec4(object->GetComponent<MeshRenderer>()->material.diffuse, 1.0f));
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, object->GetComponent<MeshRenderer>()->material.diffuseMap);
			shader.setInt("texture2d", 0);

			bool useTexture = (object->GetComponent<MeshRenderer>()->material.diffuseMap != 0);
			shader.setBool("useTexture", useTexture);

			// Replace 3 with MeshRenderer material shader ID
			if (shader.ID != object->GetComponent<MeshRenderer>()->material.shader)
			{
				continue;
			}
			
			mat4 model = setupTransform(object->transform);
			shader.setMat4("model", model);

			// LIGHTS
			if (shader.ID == 6)
			{
				for (int i = 0; i < currentScene.lights.size(); i++)
				{
					std::string lightName = "pointLights[" + std::to_string(i) + "]";
					if (currentScene.lights[i] != nullptr)
					{
						shader.setVec3(lightName + ".position", currentScene.lights[i]->entity->transform.position);
					}
					else
					{
						std::cout << "Light is null" << std::endl;
					}
					shader.setVec3(lightName + ".ambient", currentScene.lights[i]->ambient);
					shader.setVec3(lightName + ".diffuse", currentScene.lights[i]->diffuse);
					shader.setVec3(lightName + ".specular", currentScene.lights[i]->specular);
					shader.setFloat(lightName + ".constant", 1.0f);
					shader.setFloat(lightName + ".linear", 1.0f/currentScene.lights[i]->radius);
					shader.setFloat(lightName + ".quadratic", 1.0f/pow(currentScene.lights[i]->radius, 2));
					shader.setFloat(lightName + ".intensity", currentScene.lights[i]->intensity);
				}
				shader.setVec3("sceneLight.ambient", currentScene.sceneLighting.ambient);
			}

			// MATERIAL HERE
			if (shader.ID == 6)
			{
				// Activate and bind the emission map
				glActiveTexture(GL_TEXTURE0); // Activate texture unit 0
				glBindTexture(GL_TEXTURE_2D, object->GetComponent<MeshRenderer>()->material.emissionMap); // Bind emission map
				shader.setInt("material.emissionMap", 0); // Set the sampler uniform to texture unit 0
				shader.setVec3("material.emission", object->GetComponent<MeshRenderer>()->material.emission);

				// Activate and bind the diffuse map
				glActiveTexture(GL_TEXTURE1); // Activate texture unit 1
				glBindTexture(GL_TEXTURE_2D, object->GetComponent<MeshRenderer>()->material.diffuseMap); // Bind diffuse map
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				shader.setInt("material.diffuseMap", 1); // Set the sampler uniform to texture unit 1
				shader.setVec3("material.diffuse", object->GetComponent<MeshRenderer>()->material.diffuse);

				// Activate and bind the specular map
				glActiveTexture(GL_TEXTURE2); // Activate texture unit 2
				glBindTexture(GL_TEXTURE_2D, object->GetComponent<MeshRenderer>()->material.specularMap); // Bind specular map
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				shader.setInt("material.specularMap", 2); // Set the sampler uniform to texture unit 2
				shader.setVec3("material.specular", object->GetComponent<MeshRenderer>()->material.specular);

				// Set shininess value
				shader.setFloat("material.shininess", object->GetComponent<MeshRenderer>()->material.shininess);

				shader.setVec3("viewPos", cam.transform.position);
			}


			//SKYBOX
			if (object->GetID() == currentScene.skybox_ent->GetID()) glDepthMask(GL_FALSE); // If object is skybox, disable depth
			render(*object);

			glDepthMask(GL_TRUE);
		}
	}
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	glfwSwapBuffers(window);
}

void Renderer::reloadShaders()
{
	defaultShader.reload();
}

void Renderer::cleanup()
{
	shaders.clear();
}