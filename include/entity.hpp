#pragma once
#include <transform.hpp>
#include <glm/glm.hpp>
#include <vector>
#include <unordered_map>
#include <memory>
#include <typeindex>
#include <iostream>

struct Transform
{
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    glm::vec3 forward;
    glm::vec3 up;

    Transform() :
        position(0.0f),
        rotation(1.0f, 0.0f, 0.0f),
        scale(1.0f),
        forward(1.0f, 0.0f, 0.0f),
        up(0.0f, 1.0f, 0.0f) {}
};


class Entity;
class Component
{
public:
    bool enabled = true;
    Entity* entity;
    Transform* transform;
    virtual ~Component() = default;
};

class Entity
{
private:
    using EntityID = size_t;
    static EntityID nextId;
    EntityID id;
    std::unordered_map<std::type_index, std::shared_ptr<Component>> components;
public:
    Transform transform;
    std::string name;

    Entity() : id(nextId++) {}

	int GetID() const { return id; }

    // Add a component to this entity
    template <typename T, typename... Args>
    T* AddComponent(Args&&... args)
    {
		// Set component entity to this entity
		auto component = std::make_shared<T>(std::forward<Args>(args)...);
		component->entity = this;
		component->transform = &this->transform;
        components[typeid(T)] = component;
		return static_cast<T*>(component.get());
    }

    // Get a component attached to this entity
    template <typename T>
    T* GetComponent()
    {
        auto it = components.find(typeid(T));
        if (it != components.end())
        {
            return static_cast<T*>(it->second.get());
        }
        return nullptr;
    }
    template <typename T>
    void RemoveComponent()
    {
        auto it = components.find(typeid(T));
        if (it != components.end())
        {
            components.erase(it);
        }
    }

    // Check if the entity has a specific component
    template <typename T>
    bool HasComponent() const
    {
        return components.find(typeid(T)) != components.end();
    }

    bool hasComponents()
    {
        return !components.empty();
    }

    std::unordered_map<std::type_index, std::shared_ptr<Component>> getComponents()
    {
        return components;
    }
};

Entity* Instantiate(Entity* entity);