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
	mBehaviorContext(behaviorContext)
{
	createEntityData();
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
	data.fovRange = 5;

	data.combatStats.cAttackDamage = 19;
	data.combatStats.cAttackRange = 1;
	data.combatStats.cAttackSpeed = 1.3f;
	data.combatStats.cDefence = 8;
	data.combatStats.cHealth = 188;
	data.combatStats.cMaxHealth = data.combatStats.cHealth;
	data.cAttackDataMap = std::move(getPlayerAttackDataMap());

	mEntityData.emplace(EntityType::Player, std::move(data));
}

void EntitySpawnerSystem::createSkletorusEntityData()
{
	BaseEntityData data;
	data.tag = "Skletorus";
	data.moveSpeed = 115.f;
	data.fovRange = 3;

	data.combatStats.cAttackDamage = 7;
	data.combatStats.cAttackRange = 1;
	data.combatStats.cAttackSpeed = 1.1f;
	data.combatStats.cDefence = 2;
	data.combatStats.cHealth = 57;
	data.combatStats.cMaxHealth = data.combatStats.cHealth;

	data.cAttackDataMap = getBasicMeleeAttackDataMap();
	mEntityData.emplace(EntityType::Skletorus, std::move(data));
}

void EntitySpawnerSystem::spawnEntity(const sf::Vector2i& cellIndex, EntityType entType)
{
	switch (entType)
	{
	case EntityType::Player:
		spawnPlayerEntity(cellIndex);
		break;
	case EntityType::Skletorus:
		spawnSkletorusEntity(cellIndex);
		break;
	default:
		break;
	}
}

void EntitySpawnerSystem::addCommonComponents(Entity& entity, EntityType entType)
{
	auto& entityData = mEntityData.at(entType);

	entity.addComponent<DirectionComponent>();
	entity.addComponent<EntityStateComponent>();
	entity.addComponent<AnimationComponent>();
	entity.addComponent<EntityTypeComponent>(entType);
	entity.addComponent<HealthBarComponent>();
	entity.addComponent<TagComponent>(entityData.tag);
	entity.addComponent<MovementComponent>(entityData.moveSpeed);
	entity.addComponent<FieldOfViewComponent>(entityData.fovRange);
	entity.addComponent<CombatStatsComponent>(entityData.combatStats);
	entity.addComponent<AttackComponent>().cAttackDataMap = entityData.cAttackDataMap;
}

void EntitySpawnerSystem::addSpriteComponent(Entity& entity, TextureIdentifier textureId, const sf::Vector2f& pos)
{
	auto& spriteComp = entity.addComponent<SpriteComponent>(mTextures.get(textureId));
	spriteComp.cSprite.setPosition(pos);
	spriteComp.cSprite.setTextureRect(sf::IntRect({ 0, 0 }, { 64, 64 }));
}

void EntitySpawnerSystem::addAIComponents(Entity& entity)
{
	entity.addComponent<EnemyComponent>();
	entity.addComponent<EntityAIStateComponent>();
	entity.addComponent<PatrolAIComponent>(static_cast<float>(getRandomPatrolDelay(1600, 3200)));
	entity.addComponent<ChaseAIComponent>();
	entity.addComponent<PathComponent>();
	entity.addComponent<CombatStatsComponent>();
	entity.addComponent<AITimersComponent>();
}

void EntitySpawnerSystem::spawnPlayerEntity(const sf::Vector2i& cellIndex)
{
	auto& entity = mSystemContext.entityManager.createEntity();
	addCommonComponents(entity, EntityType::Player);
	addSpriteComponent(entity, TextureIdentifier::Player, cellIndexToPos(cellIndex));
	entity.addComponent<PlayerComponent>();

	notifyTileOccupied(entity);
}

void EntitySpawnerSystem::spawnSkletorusEntity(const sf::Vector2i& cellIndex)
{
	auto& entity = mSystemContext.entityManager.createEntity();
	addSpriteComponent(entity, TextureIdentifier::Skletorus, cellIndexToPos(cellIndex));
	addCommonComponents(entity, EntityType::Skletorus);
	addAIComponents(entity);
	entity.addComponent<BehaviorComponent>(std::make_unique<BasicMeleeBehavior>(mBehaviorContext));
	
	notifyTileOccupied(entity);
}

void EntitySpawnerSystem::notifyTileOccupied(Entity& entity)
{
	auto pos = Utilities::getEntityPos(entity);
	mSystemContext.eventManager.notify<TileOccupiedEvent>(TileOccupiedEvent(entity, pos));
}

sf::Vector2f EntitySpawnerSystem::cellIndexToPos(const sf::Vector2i& cellIndex) const
{
	return { cellIndex.x * Config::getCellSize().x, cellIndex.y * Config::getCellSize().y };
}

EntitySpawnerSystem::AttackDataMap EntitySpawnerSystem::getBasicMeleeAttackDataMap() const
{
	AttackDataMap result;
	AttackData att;
	att.damageMultiplier = 1.0f;
	att.speedMultiplier = 1.0f;
	
	att.hitOffsets[Direction::Up]     = std::vector<sf::Vector2i>{ { 0, -1} };
	att.hitOffsets[Direction::Left]   = std::vector<sf::Vector2i>{ {-1,  0} };
	att.hitOffsets[Direction::Bottom] = std::vector<sf::Vector2i>{ { 0,  1} };
	att.hitOffsets[Direction::Right]  = std::vector<sf::Vector2i>{ { 1,  0} };

	result[AnimationIdentifier::Attack1] = std::move(att);
	return result;
}

EntitySpawnerSystem::AttackDataMap EntitySpawnerSystem::getPlayerAttackDataMap() const
{
	AttackDataMap result;
	AttackData att1, att2, att3;
	att1.damageMultiplier = 1.0f;
	att1.speedMultiplier = 1.0f;
	att1.hitOffsets[Direction::Up]     = std::vector<sf::Vector2i>{ {-1, 0},  { 0, -1} };
	att1.hitOffsets[Direction::Left]   = std::vector<sf::Vector2i>{ {-1, 0},  { 0, -1} };
	att1.hitOffsets[Direction::Bottom] = std::vector<sf::Vector2i>{ { 1, 0},  { 0,  1} };
	att1.hitOffsets[Direction::Right]  = std::vector<sf::Vector2i>{ { 1, 0},  { 0,  1} };

	att2.damageMultiplier = 1.0f;
	att2.speedMultiplier = 1.0f;
	att2.hitOffsets[Direction::Up]     = std::vector<sf::Vector2i>{ { 1, 0}, { 0, -1} };
	att2.hitOffsets[Direction::Left]   = std::vector<sf::Vector2i>{ {-1, 0}, { 0,  1} };
	att2.hitOffsets[Direction::Bottom] = std::vector<sf::Vector2i>{ {-1, 0}, { 0,  1} };
	att2.hitOffsets[Direction::Right]  = std::vector<sf::Vector2i>{ { 1, 0}, { 0, -1} };

	att3.damageMultiplier = 1.35f;
	att3.speedMultiplier = 1.1f;
	att3.hitOffsets[Direction::Up]     = std::vector<sf::Vector2i>{  { 0, -1} };
	att3.hitOffsets[Direction::Left]   = std::vector<sf::Vector2i>{  {-1,  0} };
	att3.hitOffsets[Direction::Bottom] = std::vector<sf::Vector2i>{  { 0,  1} };
	att3.hitOffsets[Direction::Right]  = std::vector<sf::Vector2i>{  { 1,  0} };

	result[AnimationIdentifier::Attack1] = std::move(att1);
	result[AnimationIdentifier::Attack2] = std::move(att2);
	result[AnimationIdentifier::Attack3] = std::move(att3);

	return result;
}

int EntitySpawnerSystem::getRandomPatrolDelay(int min, int max) const
{
	return Random::get(min, max);
}
