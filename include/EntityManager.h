#pragma once
#include "Entity.h"

class EntityManager
{
public:
	EntityManager();
	
	Entity& createEntity();
	Entity* getEntity(size_t id);
	void removeEntity(size_t id);

	const std::vector<std::unique_ptr<Entity>>& getEntities() const;

	template <ComponentType... Components>
	std::vector<Entity*> getEntitiesWithComponents() const; 

	Entity& getPlayer() const;
	void printSize();
private:
	std::vector<std::unique_ptr<Entity>> mEntities;
	size_t mNextEntityId;
};

template<ComponentType ...Components>
inline std::vector<Entity*> EntityManager::getEntitiesWithComponents() const
{
	std::vector<Entity*> result;
	for (auto& entity : mEntities)
	{
		if ((entity->hasComponent<Components>() && ...))
			result.push_back(entity.get());
	}
	return result;
}
