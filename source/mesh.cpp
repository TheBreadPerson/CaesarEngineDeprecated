#include <mesh.hpp>
#include <globals.hpp>

#include <stb_image.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <shader.hpp>
#include <asset_manager.hpp>

#include <fastgltf/glm_element_traits.hpp>
#include <fastgltf/core.hpp>
#include <fastgltf/types.hpp>

#include <iostream>

using namespace glm;

Mesh::Mesh(const std::vector<Vertex>& verts, const std::vector<unsigned int>& inds) : vertices(verts), indices(inds)
{

}

fastgltf::Parser parser;

Mesh AssetManager::loadModel(std::filesystem::path path)
{
	Mesh gltfMesh;

	constexpr auto gltfOptions = fastgltf::Options::None;

	if (!std::filesystem::exists(path)) {
		std::cerr << "Failed to find " << path << '\n';
		return gltfMesh;
	}

	

	auto gltfFile = fastgltf::MappedGltfFile::FromPath(path);
	if (!bool(gltfFile)) {
		std::cerr << "Failed to open glTF file: " << fastgltf::getErrorMessage(gltfFile.error()) << '\n';
		return gltfMesh;
	}

	auto asset = parser.loadGltfBinary(gltfFile.get(), path.parent_path(), gltfOptions);
	if (asset.error() != fastgltf::Error::None) {
		std::cerr << "Failed to load glTF: " << fastgltf::getErrorMessage(asset.error()) << '\n';
		return gltfMesh;
	}
	std::cout << "Successfully loaded " << path << '\n';
	fastgltf::Asset gltf = std::move(asset.get());

	std::vector<unsigned int> indices;
	std::vector<Vertex> vertices;
	for (fastgltf::Mesh& mesh : gltf.meshes)
	{
		for (auto&& p : mesh.primitives)
		{
			size_t initial_vtx = vertices.size();

			// load indexes
			{
				fastgltf::Accessor& indexaccessor = gltf.accessors[p.indicesAccessor.value()];
				indices.reserve(indices.size() + indexaccessor.count);

				fastgltf::iterateAccessor<std::uint32_t>(gltf, indexaccessor,
					[&](std::uint32_t idx) {
						indices.push_back(idx + initial_vtx);
					});
			}

			// load vertex positions
			{
				fastgltf::Accessor& posAccessor = gltf.accessors[p.findAttribute("POSITION")->accessorIndex];
				vertices.resize(vertices.size() + posAccessor.count);

				fastgltf::iterateAccessorWithIndex<glm::vec3>(gltf, posAccessor,
					[&](glm::vec3 v, size_t index)
					{
						Vertex newvtx;
						newvtx.position = v;
						newvtx.normal = { 1, 0, 0 };
						newvtx.texCoords = { 0, 0 };
						newvtx.color = glm::vec4{ 1.f };
						vertices[initial_vtx + index] = newvtx;
					});
			}

			// load vertex normals
			auto normals = p.findAttribute("NORMAL");
			if (normals != p.attributes.end())
			{

				fastgltf::iterateAccessorWithIndex<glm::vec3>(gltf, gltf.accessors[(*normals).accessorIndex],
					[&](glm::vec3 v, size_t index)
					{
						vertices[initial_vtx + index].normal = v;
					});
			}

			// load UVs
			auto uv = p.findAttribute("TEXCOORD_0");
			if (uv != p.attributes.end())
			{

				fastgltf::iterateAccessorWithIndex<glm::vec2>(gltf, gltf.accessors[(*uv).accessorIndex],
					[&](glm::vec2 v, size_t index)
					{
						vertices[initial_vtx + index].texCoords = v;
					});
			}

			// load vertex colors
			auto colors = p.findAttribute("COLOR_0");
			if (colors != p.attributes.end())
			{
				fastgltf::iterateAccessorWithIndex<glm::vec4>(gltf, gltf.accessors[(*colors).accessorIndex],
					[&](glm::vec4 v, size_t index)
					{
						vertices[initial_vtx + index].color = v;
					});
			}
		}
	}

	gltfMesh.vertices = vertices;
	gltfMesh.indices = indices;

	return gltfMesh;
}

void MeshRenderer::setMesh(const char* _mesh_path)
{
	mesh_path = _mesh_path;
}


unsigned int AssetManager::LoadTexture(const char* filepath)
{
	unsigned int texture;

	// texture 
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	// The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
	unsigned char* data = stbi_load(filepath, &width, &height, &nrChannels, 0);
	if (data)
	{
		std::cout << "Successfully loaded texture: " << filepath << std::endl;
		glTexImage2D(GL_TEXTURE_2D, 0, nrChannels > 3 ? GL_RGBA : GL_RGB, width, height, 0, nrChannels > 3 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture: " << filepath << std::endl;
		data = new unsigned char[1 * 1 * 4];
		data[0] = 255;
		data[1] = 0;
		data[2] = 255;
		data[3] = 255;
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	stbi_image_free(data);
	return texture;
}
//unsigned int graphics::LoadTexture(const char* filepath)
//{
//	unsigned int texture;
//
//	// Generate and bind the texture
//	glGenTextures(1, &texture);
//	glBindTexture(GL_TEXTURE_2D, texture);
//
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//	// Load image data
//	int width, height, nrChannels;
//	stbi_set_flip_vertically_on_load(true); // Flip the image on the y-axis
//	unsigned char* data = stbi_load(filepath, &width, &height, &nrChannels, 0);
//
//	if (data)
//	{
//		std::cout << "Successfully loaded texture: " << filepath << std::endl;
//		// Upload texture data to GPU (no mipmaps, no parameters set)
//		glTexImage2D(GL_TEXTURE_2D, 0, nrChannels > 3 ? GL_RGBA : GL_RGB, width, height, 0,
//			nrChannels > 3 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);
//	}
//	else
//	{
//		std::cout << "Failed to load texture: " << filepath << std::endl;
//	}
//
//	// Free image data
//	stbi_image_free(data);
//
//	// Return the texture ID
//	return texture;
//}