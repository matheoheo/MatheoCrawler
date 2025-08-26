#include "pch.h"
#include "EntitySpawnerSystem.h"
#include "EntityTypes.h"
#include "Random.h"
#include "Config.h"
#include "BasicMeleeBehavior.h"
#include "Utilities.h"

EntitySpawnerSystem::EntitySpawnerSystem(SystemContext& systemContext, AssetManager<TextureIdentifier, sf::Texture>& textures,
	BehaviorContext& behaviorContext)
	:ISystem(systemContext),
	mTextures(textures),
	mBehaviorContext(behaviorContext),
	mEntityFactory(systemContext.entityManager, textures, behaviorContext, systemContext.eventManager)
{
	registerToEvents();
}

void EntitySpawnerSystem::update(const sf::Time& deltaTime)
{
}

void EntitySpawnerSystem::registerToEvents()
{
	registerToSpawnEntityEvent();
	registerToSpawnProjectileEvent();
	registerToGenerateEntitiesEvent();
}

void EntitySpawnerSystem::registerToSpawnEntityEvent()
{
	mSystemContext.eventManager.registerEvent<SpawnEntityEvent>([this](const SpawnEntityEvent& data)
		{
			mEntityFactory.spawnEntity(data.cellIndex, data.entityType);
		});
}

void EntitySpawnerSystem::registerToSpawnProjectileEvent()
{
	mSystemContext.eventManager.registerEvent<SpawnProjectileEvent>([this](const SpawnProjectileEvent& data)
		{
			mEntityFactory.spawnProjectileEvent(data);
		});
}

void EntitySpawnerSystem::registerToGenerateEntitiesEvent()
{
	mSystemContext.eventManager.registerEvent<GenerateEntitiesEvent>([this](const GenerateEntitiesEvent& data)
		{
			spawnEntities(data.spawnPoints);
		});
}

EntityType EntitySpawnerSystem::getRandomWeightedSpawn(std::span<const WeightedSpawn> spawns)
{
	int totalWeight = 0;
	for (const auto& spawn : spawns)
		totalWeight += spawn.weight;

	int roll = Random::get(1, totalWeight);
	int sum = 0;

	for (const auto& spawn : spawns)
	{
		sum += spawn.weight;
		if (roll <= sum)
			return spawn.type;
	}

	return spawns.back().type;
}

void EntitySpawnerSystem::spawnEntities(const IMapGenerator::SpawnPoints& spawnPoints)
{
	constexpr size_t entityCount = 4;
	constexpr std::array<WeightedSpawn, entityCount> weightedSpawns =
	{ {
		{EntityType::Skletorus, 50},
		{EntityType::Bonvik, 25},
		{EntityType::Moranna, 10},
		{EntityType::Ray, 100}
	} };

	for (const auto& point : spawnPoints)
	{
		EntityType type = getRandomWeightedSpawn(weightedSpawns);
		mEntityFactory.spawnEntity(point, type);
	}
}
