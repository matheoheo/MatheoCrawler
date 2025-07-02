#include "pch.h"
#include "EntityFactory.h"
#include "Random.h"
#include "Config.h"
#include "BasicMeleeBehavior.h"
#include "EventManager.h"
#include "Utilities.h"
#include "SpellHolder.h"
#include "SpellIdentifiers.h"


EntityFactory::EntityFactory(EntityManager& entityManager, AssetManager<TextureIdentifier, sf::Texture>& textures,
	BehaviorContext& behaviorContext, EventManager& eventManager)
    :mEntityManager(entityManager),
    mTextures(textures),
	mBehaviorContext(behaviorContext),
	mEventManager(eventManager)
{
}

void EntityFactory::spawnEntity(const sf::Vector2i& cellIndex, EntityType entType)
{
	switch (entType)
	{
	case EntityType::Player:
		spawnPlayerEntity(cellIndex);
		break;
	case EntityType::Skletorus:
		spawnSkletorusEntity(cellIndex);
		break;
	case EntityType::Bonvik:
		spawnBonvikEntity(cellIndex);
		break;
	default:
		break;
	}
}

void EntityFactory::spawnProjectileEvent(const SpawnProjectileEvent& data)
{
	auto casterDir = data.caster.getComponent<DirectionComponent>().cCurrentDir;
	auto casterCellIndex = Utilities::getEntityCell(data.caster);

	if (data.projId == SpellIdentifier::WaterBall)
		spawnWaterBall(data, casterCellIndex, casterDir);
	else if (data.projId == SpellIdentifier::PureProjectile)
		spawnPureProjectiles(data, casterCellIndex, casterDir);
	else if (data.projId == SpellIdentifier::Fireball)
		spawnFireballProjectiles(data, casterCellIndex);
	else if (data.projId == SpellIdentifier::Bloodball)
		spawnBloodballProjectiles(data, casterCellIndex, casterDir);
}

void EntityFactory::addCommonComponents(Entity& entity, EntityType entType)
{
	const auto& entityData = mEntityData.get(entType);

	entity.addComponent<DirectionComponent>();
	entity.addComponent<EntityStateComponent>();
	entity.addComponent<AnimationComponent>();
	entity.addComponent<EntityTypeComponent>(entType);
	entity.addComponent<HealthBarComponent>();
	entity.addComponent<TagComponent>(entityData.tag);
	entity.addComponent<MovementComponent>(entityData.moveSpeed);
	entity.addComponent<FieldOfViewComponent>(entityData.fovRange);
	entity.addComponent<CombatStatsComponent>(getAdjustedCombatStats(entType));
	entity.addComponent<AttackComponent>().cAttackDataMap = entityData.cAttackDataMap;
}

void EntityFactory::addSpriteComponent(Entity& entity, TextureIdentifier textureId, const sf::Vector2f& pos)
{
	auto& spriteComp = entity.addComponent<SpriteComponent>(mTextures.get(textureId));
	spriteComp.cSprite.setPosition(pos);
	spriteComp.cSprite.setTextureRect(sf::IntRect({ 0, 0 }, { 64, 64 }));
}

void EntityFactory::addAIComponents(Entity& entity)
{
	entity.addComponent<EnemyComponent>();
	entity.addComponent<EntityAIStateComponent>();
	entity.addComponent<PatrolAIComponent>(static_cast<float>(getRandomPatrolDelay(1600, 3200)));
	entity.addComponent<ChaseAIComponent>();
	entity.addComponent<PathComponent>();
	entity.addComponent<CombatStatsComponent>();
	entity.addComponent<AITimersComponent>();
	entity.addComponent<AvailableAttacksComponent>();
}

void EntityFactory::spawnPlayerEntity(const sf::Vector2i& cellIndex)
{
	auto& entity = mEntityManager.createEntity();
	addCommonComponents(entity, EntityType::Player);
	addSpriteComponent(entity, TextureIdentifier::Player, cellIndexToPos(cellIndex));
	entity.addComponent<PlayerComponent>();
	entity.addComponent<PlayerResourcesComponent>();
	entity.addComponent<AttackSelectionComponent>();
	entity.addComponent<PlayerManaBarComponent>();
	entity.addComponent<RegenerationComponent>();
	entity.addComponent<StatisticsUpgradeComponent>();
	auto& assigned = entity.addComponent<AssignedSpellsComponent>();
	auto& spells = entity.addComponent<SpellbookComponent>();

	constexpr std::array<SpellIdentifier, 5> playerSpells =
	{
		SpellIdentifier::QuickHeal,
		SpellIdentifier::ManaRegen,
		SpellIdentifier::PureProjectile,
		SpellIdentifier::WaterBall,
		SpellIdentifier::Fireball
	};

	int i = 0;
	for (auto id : playerSpells )
	{
		spells.cSpells.emplace(id, SpellInstance{ .data = &SpellHolder::getInstance().get(id)});
		assigned.cAssignedSpells.emplace(i, &spells.cSpells.at(id));
		++i;
	}
	
	notifyTileOccupied(entity);

}

void EntityFactory::spawnSkletorusEntity(const sf::Vector2i& cellIndex)
{
	auto& entity = mEntityManager.createEntity();
	addSpriteComponent(entity, TextureIdentifier::Skletorus, cellIndexToPos(cellIndex));
	addCommonComponents(entity, EntityType::Skletorus);
	addAIComponents(entity);
	entity.addComponent<BehaviorComponent>(std::make_unique<BasicMeleeBehavior>(mBehaviorContext));
	notifyTileOccupied(entity);

}

void EntityFactory::spawnBonvikEntity(const sf::Vector2i& cellIndex)
{
	auto& entity = mEntityManager.createEntity();
	addSpriteComponent(entity, TextureIdentifier::Bonvik, cellIndexToPos(cellIndex));
	addCommonComponents(entity, EntityType::Bonvik);
	addAIComponents(entity);
	entity.addComponent<BehaviorComponent>(std::make_unique<BasicMeleeBehavior>(mBehaviorContext));
	auto& avAttacksComp = entity.getComponent<AvailableAttacksComponent>();
	avAttacksComp.mAttacks.emplace_back(AnimationIdentifier::Attack2);

	notifyTileOccupied(entity);
}

sf::Vector2f EntityFactory::cellIndexToPos(const sf::Vector2i& cellIndex) const
{
    return { cellIndex.x * Config::getCellSize().x, cellIndex.y * Config::getCellSize().y };
}

int EntityFactory::getRandomPatrolDelay(int min, int max) const
{
    return Random::get(min, max);
}

void EntityFactory::notifyTileOccupied(Entity& entity)
{
	mEventManager.notify<TileOccupiedEvent>(TileOccupiedEvent(entity, Utilities::getEntityPos(entity)));
}

CombatStatsComponent EntityFactory::getAdjustedCombatStats(EntityType entType) const
{
	const auto& defaultStats = mEntityData.get(entType).combatStats;
	if (entType == EntityType::Player)
		return defaultStats;

	CombatStatsComponent adjustedStats(defaultStats);

	float multiplier = getStatMultiplier();
	adjustedStats.cAttackDamage *= multiplier;
	adjustedStats.cDefence *= multiplier;
	adjustedStats.cMaxHealth *= multiplier;
	adjustedStats.cHealth = adjustedStats.cMaxHealth;


	return adjustedStats;
}

float EntityFactory::getStatMultiplier() const
{
	return std::pow(1.20f, Config::difficulityLevel - 1);
}

void EntityFactory::spawnProjectile(const SpawnProjectileEvent& data, const sf::Vector2i& cellIndex, Direction casterDir)
{
	constexpr sf::Vector2f projectileSize{ 48.f, 48.f };

	auto& spellBookComp = data.caster.getComponent<SpellbookComponent>();
	auto& thisSpell = spellBookComp.cSpells.at(data.projId);
	auto& spellData = thisSpell.data;
	if (spellData->type != SpellType::Projectile || !spellData->projectile)
		return;

	auto& projData = spellData->projectile.value();
	sf::Vector2f spawnPos{ cellIndex.x * Config::getCellSize().x, cellIndex.y * Config::getCellSize().y };
	bool playerCasted = data.caster.hasComponent<PlayerComponent>();
	sf::Vector2f offset{ Config::getCellSize().x / 2.f, Config::getCellSize().y / 2.f };

	auto& entity = mEntityManager.createEntity();
	auto& spriteComp = entity.addComponent<SpriteComponent>(getProjectileTexture(data.projId, *spellData));
	spriteComp.cSprite.setOrigin(projectileSize * 0.5f);
	spriteComp.cSprite.setPosition(spawnPos + offset);
	spriteComp.cSprite.setRotation(sf::degrees(getProjectileRotation(casterDir)));

	auto& moveComp = entity.addComponent<MovementComponent>(projData.speed);
	moveComp.cDirectionVector = Utilities::dirToVector(casterDir);

	auto& projComp = entity.addComponent<ProjectileComponent>(projData, playerCasted);

	mEventManager.notify<ProjectileSpawnedEvent>(ProjectileSpawnedEvent(entity));
	std::cout << "Spawned at: " << cellIndex.x << ' ' << cellIndex.y << '\n';
}

void EntityFactory::spawnWaterBall(const SpawnProjectileEvent& data, const sf::Vector2i& cellIndex, Direction casterDir)
{
	auto spawnCell = Utilities::getNextCellIndex(cellIndex, casterDir);
	spawnProjectile(data, spawnCell, casterDir);
}

void EntityFactory::spawnPureProjectiles(const SpawnProjectileEvent& data, const sf::Vector2i& cellIndex, Direction casterDir)
{
	std::vector<sf::Vector2i> cells;
	if (casterDir == Direction::Up || casterDir == Direction::Bottom)
		cells = std::vector<sf::Vector2i>{ {cellIndex.x - 1, cellIndex.y}, {cellIndex.x + 1, cellIndex.y} };
	else if (casterDir == Direction::Left || casterDir == Direction::Right)
		cells = std::vector<sf::Vector2i>{ {cellIndex.x, cellIndex.y - 1}, {cellIndex.x, cellIndex.y + 1} };
	cells.push_back(cellIndex);

	for (const auto& cell : cells)
	{
		
		auto spawnCell = Utilities::getNextCellIndex(cell, casterDir);
		spawnProjectile(data, spawnCell, casterDir);
	}
}

void EntityFactory::spawnFireballProjectiles(const SpawnProjectileEvent& data, const sf::Vector2i& cellIndex)
{
	constexpr std::array<Direction, 4> dirs =
	{
		Direction::Up, Direction::Left, Direction::Bottom, Direction::Right
	};

	for (Direction dir : dirs)
	{
		auto spawnCell = Utilities::getNextCellIndex(cellIndex, dir);
		spawnProjectile(data, spawnCell, dir);
	}
}

void EntityFactory::spawnBloodballProjectiles(const SpawnProjectileEvent& data, const sf::Vector2i& cellIndex, Direction casterDir)
{
	std::vector<Direction> projDirections;
	if (casterDir == Direction::Up || casterDir == Direction::Bottom)
		projDirections = std::vector<Direction>{ Direction::Left, Direction::Right };
	else if (casterDir == Direction::Left || casterDir == Direction::Right)
		projDirections = std::vector<Direction>{ Direction::Up, Direction::Bottom };

	for (Direction dir : projDirections)
	{
		auto spawnCell = Utilities::getNextCellIndex(cellIndex, dir);
		spawnProjectile(data, spawnCell, dir);
	}
}

float EntityFactory::getProjectileRotation(Direction dir) const
{
	switch (dir)
	{
	case Direction::Right:
		return 0.f;
	case Direction::Bottom:
		return 90.f;
	case Direction::Left:
		return 180.f;
	case Direction::Up:
		return 270.f;
	default:
		return 0.f;
	}
	return 0.0f;
}

const sf::Texture& EntityFactory::getProjectileTexture(SpellIdentifier id, const SpellData& data)
{
	//this class will be developed more in future.
	if (id == SpellIdentifier::WaterBall)
	{
		return mTextures.get(TextureIdentifier::WaterBall0);
	}
	else if (id == SpellIdentifier::PureProjectile)
	{
		return mTextures.get(TextureIdentifier::PureProjectile0);
	}
	else if (id == SpellIdentifier::Fireball)
	{
		return mTextures.get(TextureIdentifier::Fireball0);
	}
	else if (id == SpellIdentifier::Bloodball)
	{
		return mTextures.get(TextureIdentifier::Bloodball0);
	}
	//default return;
	return mTextures.get(TextureIdentifier::WaterBall0);
}
