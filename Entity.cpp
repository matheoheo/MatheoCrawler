#include "pch.h"
#include "Entity.h"

Entity::Entity(EntityID entityId)
    :mEntityId(entityId)
{
}

Entity::EntityID Entity::getEntityId() const
{
    return mEntityId;
}
