#pragma once
#include <unordered_map>
#include <stdio.h>
#include <mesh.hpp>

class AssetManager
{
	std::unordered_map<std::string, Mesh> mesh_list;
};