#pragma once
#include "ConceptDefinitions.h"
#include "Components.h"
#include <typeindex>

class Entity
{
public:
	using EntityID = size_t;

	Entity(EntityID entityId);
	EntityID getEntityId() const;

	template <ComponentType Component, typename... Args>
	Component& addComponent(Args&&... args);

	template <ComponentType Component>
	Component& getComponent() const;

	template <ComponentType Component>
	bool hasComponent() const;

	template <ComponentType Component>
	void removeComponent();
private:
	const EntityID mEntityId;
	std::unordered_map<std::type_index, std::unique_ptr<IComponent>> mComponents;
};

template<ComponentType Component, typename ...Args>
inline Component& Entity::addComponent(Args&& ...args)
{
	mComponents.try_emplace(typeid(Component), std::make_unique<Component>(std::forward<Args>(args)...));
	return getComponent<Component>();
}

template<ComponentType Component>
inline Component& Entity::getComponent() const
{
	assert(hasComponent<Component>());
	return *static_cast<Component*>(mComponents.at(typeid(Component)).get());
}

template<ComponentType Component>
inline bool Entity::hasComponent() const
{
	return mComponents.contains(typeid(Component));
}

template<ComponentType Component>
inline void Entity::removeComponent()
{
	if (!hasComponent<Component>())
		return;

	mComponents.erase(typeid(Component));
}
