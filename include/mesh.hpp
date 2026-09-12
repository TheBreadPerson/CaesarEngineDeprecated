#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include "transform.hpp"
#include <filesystem>
#include <memory>
#include <unordered_map>
#include <stdio.h>
#include <shader.hpp>

struct Vertex
{
	glm::vec3 position;
	glm::vec4 color;
	glm::vec2 texCoords;
	glm::vec3 normal;
};
struct Material
{
	std::string shader_path;
	Shader shader;

	glm::vec3 emission;
	unsigned int emissionMap;

	glm::vec3 diffuse;
	std::string diffuse_path;
	unsigned int diffuse_map = 0;

	glm::vec3 specular;
	unsigned int specularMap;
	float shininess;

	void setShader(const char* _shader_name);

	Material() :
		emission(0.1f),
		diffuse(1.0f),
		specular(0.1f),
		shininess(32.0f)
	{}

	void save(const char* save_path);
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
	std::string material_path; // Only for saving!
	Material material;
	void setMesh(const char* _mesh_path);
	void setMaterial(const char* _material_path);
};