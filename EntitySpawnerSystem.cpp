#include "pch.h"
#include "EntitySpawnerSystem.h"
#include "EntityTypes.h"
#include "Random.h"
#include "Config.h"

EntitySpawnerSystem::EntitySpawnerSystem(SystemContext& systemContext, AssetManager<TextureIdentifier, sf::Texture>& textures)
	:ISystem(systemContext),
	mTextures(textures)
{
	registerToEvents();
	createEntityData();

	//ToDo: rest of class.
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
			spawnEntity(data.cellIndex, data.entityType);
		});
}

void EntitySpawnerSystem::createEntityData()
{
	createPlayerEntityData();
	createSkletorusEntityData();
}

void EntitySpawnerSystem::createPlayerEntityData()
{
	BaseEntityData data;
	data.tag = "Player_Default";
	data.moveSpeed = 165.f;

	data.combatStats.cAttackDamage = 19;
	data.combatStats.cAttackRange = 1;
	data.combatStats.cAttackSpeed = 1.3f;
	data.combatStats.cDefence = 8;
	data.combatStats.cHealth = 188;
	data.combatStats.cMaxHealth = data.combatStats.cHealth;

}

void EntitySpawnerSystem::createSkletorusEntityData()
{
}

void EntitySpawnerSystem::spawnEntity(const sf::Vector2i& cellIndex, EntityType entType)
{
	switch (entType)
	{
	case EntityType::Player:
		break;
	case EntityType::SkeletonAxe:
		break;
	default:
		break;
	}
}

void EntitySpawnerSystem::addCommonComponents(Entity& entity, EntityType entType)
{
	entity.addComponent<DirectionComponent>();
	entity.addComponent<EntityStateComponent>();
	entity.addComponent<AnimationComponent>();
	entity.addComponent<EntityTypeComponent>(entType);
	entity.addComponent<CombatStatsComponent>();
	entity.addComponent<HealthBarComponent>();
}

void EntitySpawnerSystem::addSpriteComponent(Entity& entity, TextureIdentifier textureId, const sf::Vector2f& pos)
{
	auto& spriteComp = entity.addComponent<SpriteComponent>(mTextures.get(textureId));
	spriteComp.cSprite.setPosition(pos);
	spriteComp.cSprite.setTextureRect(sf::IntRect({ 0, 0 }, { 64, 64 }));

}

void EntitySpawnerSystem::spawnPlayerEntity(const sf::Vector2i& cellIndex)
{
	auto& entity = mSystemContext.entityManager.createEntity();
	addCommonComponents(entity, EntityType::Player);
	addSpriteComponent(entity, TextureIdentifier::Player, cellIndexToPos(cellIndex));
	entity.addComponent<TagComponent>("PLAYER_NAME");
	entity.addComponent<MovementComponent>(165.f);
	entity.addComponent<PlayerComponent>();

}

sf::Vector2f EntitySpawnerSystem::cellIndexToPos(const sf::Vector2i& cellIndex) const
{
	return { cellIndex.x * Config::getCellSize().x, cellIndex.y * Config::getCellSize().y };
}
