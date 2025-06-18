#include "pch.h"
#include "EntitySpawnerSystem.h"
#include "EntityTypes.h"
#include "Random.h"
#include "Config.h"
#include "BasicMeleeBehavior.h"
#include "Utilities.h"

EntitySpawnerSystem::EntitySpawnerSystem(SystemContext& systemContext, AssetManager<TextureIdentifier, sf::Texture>& textures,
	BehaviorContext& behaviorContext, SpellHolder& spellHolder)
	:ISystem(systemContext),
	mTextures(textures),
	mBehaviorContext(behaviorContext),
	mEntityFactory(systemContext.entityManager, textures, behaviorContext, systemContext.eventManager, spellHolder)
{
	registerToEvents();
}

void EntitySpawnerSystem::update(const sf::Time& deltaTime)
{
}

void EntitySpawnerSystem::registerToEvents()
{
	registerToSpawnEntityEvent();
}

void EntitySpawnerSystem::registerToSpawnEntityEvent()
{
	mSystemContext.eventManager.registerEvent<SpawnEntityEvent>([this](const SpawnEntityEvent& data)
		{
			mEntityFactory.spawnEntity(data.cellIndex, data.entityType);
		});
}