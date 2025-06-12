#pragma once
#include <iostream>
#include "AnimationHolder.h"
class Entity;
enum class Direction;
struct Tile;
struct AttackData;
enum class EntityType;

struct IEvent
{
	virtual ~IEvent() = default;
};

struct MoveRequestedEvent : public IEvent
{
	Entity& entity;
	Direction dir;

	MoveRequestedEvent(Entity& entity, Direction dir)
		:entity(entity),
		dir(dir) {}
};

struct MoveAllowedEvent : public IEvent
{
	Entity& entity;
	sf::Vector2f nextPos;

	MoveAllowedEvent(Entity& entity, const sf::Vector2f& nextPos)
		:entity(entity),
		nextPos(nextPos) {}
};

struct PlayerMoveFinishedEvent : public IEvent
{
	sf::Vector2f newPlayerPos;

	PlayerMoveFinishedEvent(const sf::Vector2f& pos)
		:newPlayerPos(pos) {}
};

struct EntityMoveFinishedEvent : public IEvent
{
	Entity& entity;
	sf::Vector2f newEntityPos;

	EntityMoveFinishedEvent(Entity& entity, const sf::Vector2f& newEntityPos)
		:entity(entity), newEntityPos(newEntityPos) {}
};

struct PlayGenericWalkEvent : public IEvent
{
	Entity& entity;

	PlayGenericWalkEvent(Entity& entity)
		:entity(entity) {}
};
struct FinalizeAnimationEvent : public IEvent
{
	const Entity& entity;

	FinalizeAnimationEvent(const Entity& entity)
		:entity(entity) {}
};

struct PlayEntitySpecificAnimationEvent : public IEvent
{
	Entity& entity;
	const EntityAnimationKey key;

	PlayEntitySpecificAnimationEvent(Entity& entity, const EntityAnimationKey& key)
		:entity(entity),
		key(key) {}
};

struct PlayAttackAnimationEvent : public IEvent
{
	Entity& entity;
	AnimationIdentifier animId;

	PlayAttackAnimationEvent(Entity& entity, AnimationIdentifier animId)
		:entity(entity),
		animId(animId) {}
};

struct ReserveTileEvent : public IEvent
{
	Entity* entity; //this is a pointer, so we can also reserve tile back to nullptr;
	sf::Vector2f tilePos;

	ReserveTileEvent(Entity* entity, const sf::Vector2f& tilePos)
		:entity(entity), tilePos(tilePos) {}
};

struct TileOccupiedEvent : public IEvent
{
	Entity& entity;
	sf::Vector2f tilePos;

	TileOccupiedEvent(Entity& entity, const sf::Vector2f& tilePos)
		:entity(entity), tilePos(tilePos) {}
};

struct TileVacatedEvent : public IEvent
{
	Entity& entity;
	sf::Vector2f tilePos;

	TileVacatedEvent(Entity& entity, const sf::Vector2f& tilePos)
		:entity(entity), tilePos(tilePos) {}
};

struct StartPatrollingEvent : public IEvent //for AI monsters
{
	Entity& entity;

	StartPatrollingEvent(Entity& ent)
		:entity(ent) {
	}
};

struct UpdateEntityRenderTilesEvent : public IEvent
{
	const std::vector<Tile*>& vec;

	UpdateEntityRenderTilesEvent(const std::vector<Tile*>& vec)
		:vec(vec) {}
};

struct StartChasingEvent : public IEvent
{
	Entity& entity;
	Entity& target;

	StartChasingEvent(Entity& entity, Entity& target)
		:entity(entity), target(target) {}
};

struct RequestPathEvent : public IEvent
{
	Entity& entity;
	bool forceRecalculation;

	RequestPathEvent(Entity& entity, bool forceRecalculation = false)
		:entity(entity),
		forceRecalculation(forceRecalculation) {}
};

struct MonsterAppearedEvent : public IEvent
{
	Entity& entity;

	MonsterAppearedEvent(Entity& entity)
		:entity(entity) 
	{
	}
};

struct MonsterDisappearedEvent : public IEvent
{
	Entity& entity;

	MonsterDisappearedEvent(Entity& entity)
		:entity(entity) {
	}
};

struct TileFadeRequestEvent : public IEvent
{
	using TileSet = std::unordered_set<Tile*>;
	TileSet tiles;

	TileFadeRequestEvent(TileSet&& tiles)
		:tiles(tiles) {}
};

struct StartAttackingEvent : public IEvent
{
	Entity& entity;
	AnimationIdentifier animId;
	StartAttackingEvent(Entity& entity, AnimationIdentifier animId)
		:entity(entity),
		animId(animId) 
	{}
};

struct AttackAnimationFinishedEvent : public IEvent
{
	const Entity& entity;
	const AttackData* lastAttackData;
	AttackAnimationFinishedEvent(const Entity& entity, const AttackData* lastAttackData)
		:entity(entity),
		lastAttackData(lastAttackData){}
};

struct HitByAttackEvent : public IEvent
{
	const Entity& attacker;
	std::vector<Entity*> hitEntities;

	HitByAttackEvent(const Entity& attacker, const std::vector<Entity*>& hitEntities)
		:attacker(attacker),
		hitEntities(hitEntities)
	{}
};

struct HealthBarUpdateEvent : public IEvent
{
	Entity& entity;
	int damageTaken; //for future usage maybe

	HealthBarUpdateEvent(Entity& entity, int damageTaken = 1)
		:entity(entity),
		damageTaken(damageTaken)
	{}
};

struct RemoveEntityFromSystemEvent : public IEvent
{
	Entity& entity;

	RemoveEntityFromSystemEvent(Entity& entity)
		:entity(entity) {}
};

struct EntityDiedEvent : public IEvent
{
	Entity& entity;

	EntityDiedEvent(Entity& entity)
		:entity(entity) {}
};

struct SpawnEntityEvent : public IEvent
{
	sf::Vector2i cellIndex; //cell on which entity will be spawned
	EntityType entityType;

	SpawnEntityEvent(const sf::Vector2i& cellIndex, EntityType entityType)
		:cellIndex(cellIndex), entityType(entityType)
	{}
};