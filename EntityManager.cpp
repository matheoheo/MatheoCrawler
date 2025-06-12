#include "pch.h"
#include "EntityManager.h"

EntityManager::EntityManager()
    :mNextEntityId(0)
{
    //mEntities.reserve(450);
}

Entity& EntityManager::createEntity()
{
    mEntities.emplace_back(std::make_unique<Entity>(mNextEntityId++));
    return *mEntities.back();
}

Entity* EntityManager::getEntity(size_t id)
{
    auto it = std::ranges::find_if(mEntities, [id](const auto& ent)
        {
            return ent->getEntityId() == id;
        });
    
    if (it != std::ranges::end(mEntities))
        return it->get();

    return nullptr;
}

void EntityManager::removeEntity(size_t id)
{
    //std::cout << "Trying to remove entity of id: " << id << '\n';
    std::erase_if(mEntities, [id](const auto& entity)
        {
            return entity->getEntityId() == id;
        });
}

Entity& EntityManager::getPlayer() const
{
    auto it = std::ranges::find_if(mEntities, [](const auto& ent)
        {
            return ent->hasComponent<PlayerComponent>();
        });

    if (it == std::ranges::end(mEntities))
        throw std::runtime_error("Player does not exist.");

    return *it->get();
}

void EntityManager::printSize()
{
    std::cout << "There are: " << mEntities.size() << '\n';
}
