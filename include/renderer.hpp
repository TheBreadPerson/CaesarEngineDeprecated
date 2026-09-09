#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <mesh.hpp>
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <stdio.h>

using namespace glm;

enum class MeshType
{
	Cube,
	Plane
};

class Renderer {
public:
	void init();
	void draw(GLFWwindow* window);

	void cleanup();

	std::vector<Mesh> meshes;
	std::vector<std::string> mesh_dirs;

	void render(Entity& entity)
	{
		// Assuming entity has a method to check if it has components and get them
		if (entity.HasComponent<MeshRenderer>())
		{
			/*Mesh* mesh = entity.GetComponent<MeshRenderer>()->mesh;*/
			// Bind and draw the mesh
			drawMesh(entity.GetComponent<MeshRenderer>()->mesh);
		}
		else
		{
			std::cerr << "Entity " << entity.name << " does not have a MeshRenderer component" << std::endl;
		}
	}

	Mesh getMeshFromDir(const char* mesh_dir)
	{
		std::string mesh_name = mesh_dir;
		auto it = std::find(mesh_dirs.begin(), mesh_dirs.end(), mesh_name);
		if (it != mesh_dirs.end())
		{
			size_t index = it - mesh_dirs.begin();
			return meshes[index];
		}
		else
		{
			std::cout << "ERROR: MESH NOT FOUND" << std::endl;
		}
	}

	void reloadShaders();

private:
	void setupMesh(Mesh& mesh)
	{
		glGenVertexArrays(1, &mesh.VAO);
		glGenBuffers(1, &mesh.VBO);
		glGenBuffers(1, &mesh.EBO);

		// Bind the Vertex Array Object
		glBindVertexArray(mesh.VAO);

		// Bind and set vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
		glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Vertex), &mesh.vertices[0], GL_STATIC_DRAW);

		// Bind and set element buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), &mesh.indices[0], GL_STATIC_DRAW);

		// Vertex Positions
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		glEnableVertexAttribArray(0);

		// Vertex Colors
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, color)));
		glEnableVertexAttribArray(1);

		// 
		// 
		// 
		// Coords
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, texCoords)));
		glEnableVertexAttribArray(2);

		// Normals
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, normal)));
		glEnableVertexAttribArray(3);

		/*if (mesh.texture_path != "")
		{
			mesh.texture = Mesh::LoadTexture(mesh.texture_path.c_str());
			mesh.hasTexture = true;
		}*/
		
		glBindVertexArray(0);
	}

	//// Apply 

	// for the current mesh (e.g., set shader matrices)
	mat4 setupTransform(const Transform& transform)
	{
		// Use the transform's position, rotation, and scale to build model matrix
		float angle = length(transform.rotation);
		vec3 angleAxis = angle > 0.0f ? normalize(transform.rotation) : vec3(1.0f);
		glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), transform.position);
		modelMatrix = rotate(modelMatrix, radians(angle), angleAxis);
		modelMatrix = scale(modelMatrix, transform.scale);
		return modelMatrix;
		// Set the model matrix in the shader
	}

	// Perform the draw call for the mesh
	void drawMesh(Mesh mesh)
	{
		//if (mesh == nullptr) return;
		if (mesh.VAO == 0) setupMesh(mesh);
		glBindVertexArray(mesh.VAO);
		glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
};