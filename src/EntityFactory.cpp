#include "pch.h"
#include "EntityFactory.h"
#include "Random.h"
#include "Config.h"
#include "BasicMeleeBehavior.h"
#include "EventManager.h"
#include "Utilities.h"
#include "SpellHolder.h"
#include "SpellIdentifiers.h"
#include "BasicRangedBehavior.h"

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
	case EntityType::Moranna:
		spawnMorannaEntity(cellIndex);
		break;
	default:
		break;
	}
}

void EntityFactory::spawnProjectileEvent(const SpawnProjectileEvent& data)
{
	auto casterDir = data.caster.getComponent<DirectionComponent>().cCurrentDir;
	auto casterCellIndex = Utilities::getEntityCell(data.caster);

	switch (data.projId)
	{
	case SpellIdentifier::WaterBall:      return spawnWaterBall(data, casterCellIndex, casterDir); 
	case SpellIdentifier::PureProjectile: return spawnPureProjectiles(data, casterCellIndex, casterDir);
	case SpellIdentifier::Fireball:       return spawnFireballProjectiles(data, casterCellIndex);
	case SpellIdentifier::Bloodball:      return spawnBloodballProjectiles(data, casterCellIndex, casterDir);
	case SpellIdentifier::MorannaProjectile: return spawnWaterBall(data, casterCellIndex, casterDir); //placeholder
	default:							  break;
	}
}

void EntityFactory::addCommonComponents(Entity& entity, EntityType entType)
{
	const auto& entityData = mEntityData.get(entType);

	entity.addComponent<DirectionComponent>();
	entity.addComponent<EntityStateComponent>();
	entity.addComponent<AnimationComponent>();
	entity.addComponent<EntityTypeComponent>(entType);
	entity.addComponent<HealthBarComponent>();
	entity.addComponent<SpellEffectsComponent>();
	entity.addComponent<StatusIconsComponent>();
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

	//here we can also add PositioningComponent that holds logical position of the entity.
	entity.addComponent<PositionComponent>(pos);
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

void EntityFactory::addPositioningComponent(Entity& entity, int minRange)
{
	const auto& combatComp = entity.getComponent<CombatStatsComponent>();
	entity.addComponent<PositioningComponent>(minRange, combatComp.cAttackRange);
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

	for (int i = 0; i <= static_cast<int>(SpellIdentifier::Bloodball); ++i)
	{
		auto id = static_cast<SpellIdentifier>(i);
		spells.cSpells.emplace(id, 
			SpellInstance{.data = &SpellHolder::getInstance().get(id)});
	}

	for (int i = 0; i < 5; ++i)
	{
		assigned.cAssignedSpells.emplace(i, nullptr);
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

void EntityFactory::spawnMorannaEntity(const sf::Vector2i& cellIndex)
{
	constexpr int minRange = 2;
	auto& entity = mEntityManager.createEntity();
	addSpriteComponent(entity, TextureIdentifier::Moranna, cellIndexToPos(cellIndex));
	addCommonComponents(entity, EntityType::Moranna);
	addAIComponents(entity);
	addPositioningComponent(entity, minRange);

	entity.addComponent<RangedEnemyComponent>(SpellIdentifier::MorannaProjectile);
	entity.addComponent<BehaviorComponent>(std::make_unique<BasicRangedBehavior>(mBehaviorContext));
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

	auto& spellData = SpellHolder::getInstance().get(data.projId);
	if (spellData.type != SpellType::Projectile || !spellData.projectile)
		return;

	auto& projData = spellData.projectile.value();
	sf::Vector2f spawnPos{ cellIndex.x * Config::getCellSize().x, cellIndex.y * Config::getCellSize().y };
	bool playerCasted = data.caster.hasComponent<PlayerComponent>();
	sf::Vector2f offset{ Config::getCellSize().x / 2.f, Config::getCellSize().y / 2.f };

	auto& entity = mEntityManager.createEntity();
	auto& spriteComp = entity.addComponent<SpriteComponent>(getProjectileTexture(data.projId));
	spriteComp.cSprite.setOrigin(projectileSize * 0.5f);
	spriteComp.cSprite.setPosition(spawnPos + offset);
	spriteComp.cSprite.setRotation(sf::degrees(getProjectileRotation(casterDir)));

	auto& moveComp = entity.addComponent<MovementComponent>(projData.speed);
	moveComp.cDirectionVector = Utilities::dirToVector(casterDir);

	int projDmg = calculateProjectileDamage(data, projData);
	auto& projComp = entity.addComponent<SpellProjectileComponent>(projData, data.caster, playerCasted, projDmg);
	mEventManager.notify<ProjectileSpawnedEvent>(ProjectileSpawnedEvent(entity));
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

const sf::Texture& EntityFactory::getProjectileTexture(SpellIdentifier id)
{
	switch (id)
	{
	case SpellIdentifier::WaterBall:      return mTextures.get(TextureIdentifier::WaterBall0);
	case SpellIdentifier::PureProjectile: return mTextures.get(TextureIdentifier::PureProjectile0);
	case SpellIdentifier::Fireball:       return mTextures.get(TextureIdentifier::Fireball0);
	case SpellIdentifier::Bloodball:      return mTextures.get(TextureIdentifier::Bloodball0);
	default:							  break;
	}

	//default return;
	return mTextures.get(TextureIdentifier::WaterBall0);
}

int EntityFactory::calculateProjectileDamage(const SpawnProjectileEvent& data, const ProjectileSpell& spellData) const
{
	int damage = Random::get(spellData.minDmg, spellData.maxDmg);
	bool isRangedEnemy = data.caster.hasComponent<RangedEnemyComponent>();

	//if caster is ranged enemy & this projectile will be only his basic attack (not plain spell)
	//then we add to damage based on caster's combat stats
	if (isRangedEnemy)
	{
		const auto& rangedComp = data.caster.getComponent<RangedEnemyComponent>();
		if (rangedComp.cSpellId == data.projId)
		{
			const auto& combatStats = data.caster.getComponent<CombatStatsComponent>();
			damage += combatStats.cAttackDamage;
		}
	}

	return damage;
}
