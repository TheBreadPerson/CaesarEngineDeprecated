#include "entity.hpp"
#include "globals.hpp"
#include <scene.hpp>

Entity::EntityID Entity::nextId = 0;

// Instantiate a new entity
Entity* Instantiate(Entity* entity)
{
	if (entity->GetComponent<Light>())
	{
		sceneManager.currentScene->sceneData.lights.push_back(entity->GetComponent<Light>());
	}
	sceneManager.currentScene->sceneData.entityList.push_back(entity);
	return sceneManager.currentScene->sceneData.entityList.back();
}