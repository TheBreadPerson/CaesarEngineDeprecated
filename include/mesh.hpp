#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include "transform.hpp"
#include <filesystem>
#include <memory>
#include <unordered_map>
#include <stdio.h>


struct Vertex
{
	glm::vec3 position;
	glm::vec4 color;
	glm::vec2 texCoords;
	glm::vec3 normal;
};
struct Material
{
	unsigned int shader = 6;
	glm::vec3 emission;
	unsigned int emissionMap;
	glm::vec3 diffuse;
	unsigned int diffuseMap;
	glm::vec3 specular;
	unsigned int specularMap;
	float shininess;

	Material() :
		emission(0.1f),
		diffuse(1.0f),
		specular(0.1f),
		shininess(32.0f)
	{}
};

struct Mesh
{
	unsigned int VBO, VAO, EBO;

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	Mesh() : vertices(), indices(), VAO(0), VBO(0), EBO(0) {}

	Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
};

class MeshRenderer : public Component
{
public:
    Mesh mesh;
	std::string mesh_path;
	Material material;
	void setMesh(const char* _mesh_path);
};

namespace graphics
{
	unsigned int LoadTexture(const char* filepath);
	Mesh loadModel(std::filesystem::path path);
}