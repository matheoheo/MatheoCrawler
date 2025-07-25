#pragma once
#include <iostream>
#include "AnimationHolder.h"
#include "SpellIdentifiers.h"
#include "IMapGenerator.h"

class Entity;
struct Tile;
struct AttackData;
enum class EntityType;
enum class Direction;
enum class MessageType;
enum class StateIdentifier;

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
	Entity& entity;

	FinalizeAnimationEvent(Entity& entity)
		:entity(entity) {}
};

struct PlayCastSpellAnimation : public IEvent
{
	Entity& entity;
	int castTime;
	AnimationIdentifier animId;

	PlayCastSpellAnimation(Entity& entity, int castTime, AnimationIdentifier animId)
		:entity(entity),
		castTime(castTime),
		animId(animId)
	{}
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
	const sf::Vector2i destinationCell;
	bool forceRecalculation;

	RequestPathEvent(Entity& entity, sf::Vector2i destinationCell, bool forceRecalculation = false)
		:entity(entity),
		forceRecalculation(forceRecalculation),
		destinationCell(destinationCell)
	{}
};

struct PathAbortedEvent : public IEvent
{
	Entity& entity;

	PathAbortedEvent(Entity& ent)
		:entity(ent)
	{}
};

struct AddToPathFollowSystemEvent : public IEvent
{
	Entity& entity;
	AddToPathFollowSystemEvent(Entity& entity)
		:entity(entity)
	{}
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
	Entity& entity;
	const AttackData* lastAttackData;
	AttackAnimationFinishedEvent(Entity& entity, const AttackData* lastAttackData)
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
	bool grantMoney; //this parametr decides wether player should reviced money or not

	EntityDiedEvent(Entity& entity, bool grantMoney = true)
		:entity(entity),
		grantMoney(grantMoney)
	{}
};

struct SpawnEntityEvent : public IEvent
{
	sf::Vector2i cellIndex; //cell on which entity will be spawned
	EntityType entityType;

	SpawnEntityEvent(const sf::Vector2i& cellIndex, EntityType entityType)
		:cellIndex(cellIndex), entityType(entityType)
	{}
};

struct PlayerGotHitEvent : public IEvent
{
	int damageTaken;

	PlayerGotHitEvent(int damageTaken)
		:damageTaken(damageTaken)
	{}
};

struct UpdatePlayerResourcesEvent : public IEvent
{
	//no need for any data,
	//PlayerResourcesUI updates by itself.
};

struct LogMessageEvent : public IEvent
{
	MessageType type;
	std::optional<int> value;
	std::optional<std::string> customMessage;

	LogMessageEvent(MessageType type, std::optional<int> value = {} , std::string_view custom = "")
		:type(type),
		value(value)
	{
		if (!custom.empty())
			customMessage = std::string(custom);
	}
};

struct SelectAttackEvent : public IEvent
{
	int attackId;

	SelectAttackEvent(int attackId)
		:attackId(attackId)
	{}
};

struct SwitchStateEvent : public IEvent
{
	StateIdentifier stateId;
	bool popPrevious;

	SwitchStateEvent(StateIdentifier stateId, bool popPrevious)
		:stateId(stateId),
		popPrevious(popPrevious)
	{

	}
};

struct PopStateEvent : public IEvent
{
	//no need for data
};

struct EnterLoadingStateEvent : public IEvent
{
	std::vector<std::function<void()>> tasksVec;

	EnterLoadingStateEvent(std::vector<std::function<void()>>&& tasksVec)
		:tasksVec(tasksVec)
	{}
};

struct UpdatePlayerStatusEvent : public IEvent
{
	//no need for data
};

struct CastSpellEvent : public IEvent
{
	Entity& caster;
	Entity* target;
	SpellIdentifier spellId;
	std::optional<sf::Keyboard::Key> usedKey;

	CastSpellEvent(Entity& caster, Entity* target, SpellIdentifier id, std::optional<sf::Keyboard::Key> key = {})
		:caster(caster),
		target(target),
		spellId(id),
		usedKey(key)
	{}
};

//called after casting animation is finished
struct CastSpellFinishedEvent : public IEvent
{
	Entity& caster;
	SpellIdentifier id;

	CastSpellFinishedEvent(Entity& caster, SpellIdentifier id)
		:caster(caster),
		id(id)
	{}
};

struct StartSpellCooldownUIEvent : public IEvent
{
	sf::Keyboard::Key usedKey;
	int& spellCd;

	StartSpellCooldownUIEvent(sf::Keyboard::Key usedKey, int& spellCd)
		:usedKey(usedKey),
		spellCd(spellCd)
	{}
};

struct TriggerHealSpellEvent : public IEvent
{
	const Entity& caster;

	TriggerHealSpellEvent(const Entity& caster)
		:caster(caster)
	{}
};

struct StartGlowUpEffect : public IEvent
{
	const Entity& entity;
	sf::Color targetColor;
	int duration;

	StartGlowUpEffect(const Entity& entity, const sf::Color& targetColor, int duration)
		:entity(entity),
		targetColor(targetColor),
		duration(duration)
	{}
};

struct TriggerHpRegenSpellEvent : public IEvent
{
	Entity& caster;
	int bonusRegen;
	int duration;

	TriggerHpRegenSpellEvent(Entity& caster, int bonusRegen, int duration)
		:caster(caster),
		bonusRegen(bonusRegen),
		duration(duration)
	{}
};

struct TriggerMpRegenSpellEvent : public IEvent
{
	Entity& caster;
	int bonusRegen;
	int duration;

	TriggerMpRegenSpellEvent(Entity& caster, int bonusRegen, int duration)
		:caster(caster),
		bonusRegen(bonusRegen),
		duration(duration)
	{}
};

struct StartRegenEffect : public IEvent
{
	Entity& caster;
	sf::Color color;
	int duration;

	StartRegenEffect(Entity& entity, const sf::Color& color, int duration)
		:caster(entity),
		color(color),
		duration(duration)
	{}
};

struct SpawnProjectileEvent : public IEvent
{
	Entity& caster;
	SpellIdentifier projId;

	SpawnProjectileEvent(Entity& caster, SpellIdentifier id)
		:caster(caster),
		projId(id)
	{}
};

struct ProjectileSpawnedEvent : public IEvent
{
	Entity& entity;

	ProjectileSpawnedEvent(Entity& entity)
		:entity(entity)
	{}
};

struct HitByProjectileEvent : public IEvent
{
	Entity& hitEntity;
	int projectileDamage;
	bool isPlayerHit;
	bool playerCasted;

	HitByProjectileEvent(Entity& hitEntity, int dmg, bool playerHit, bool playerCasted)
		:hitEntity(hitEntity),
		projectileDamage(dmg),
		isPlayerHit(playerHit),
		playerCasted(playerCasted)
	{}
};


struct CloseShopEvent : public IEvent
{
	//no need for data
};

struct RepositionToAttackEvent : public IEvent
{
	Entity& entity;
	Entity& target;

	RepositionToAttackEvent(Entity& entity, Entity& target)
		:entity(entity),
		target(target)
	{}
};

struct LoadNextLevelEvent : public IEvent
{
	//no need for data
};

struct BeforeLoadNextLevelEvent : public IEvent
{
	//Called before loading next level.
};

struct GenerateEntitiesEvent : public IEvent
{
	const IMapGenerator::SpawnPoints spawnPoints;

	GenerateEntitiesEvent(const IMapGenerator::SpawnPoints& spawnPoints)
		:spawnPoints(spawnPoints)
	{}
};

struct BindSpellEvent : public IEvent
{
	sf::Keyboard::Key slotKey; //(from Z to B)
	SpellIdentifier spellId;

	BindSpellEvent(sf::Keyboard::Key slotKey, SpellIdentifier id)
		:slotKey(slotKey),
		spellId(id)
	{}
};

struct ReBindSpellActionEvent : public IEvent
{
	using Key = sf::Keyboard::Key;
	Key newSlotKey;
	SpellIdentifier spellId;
	std::optional<Key> oldSlotKey; //in case spell was already binded we will just swap

	ReBindSpellActionEvent(Key newSlotKey, SpellIdentifier spellId, std::optional<Key> oldSlotKey = {})
		:newSlotKey(newSlotKey),
		spellId(spellId),
		oldSlotKey(oldSlotKey)
	{}
};

struct RemoveActionBindEvent : public IEvent
{
	sf::Keyboard::Key slotKey;

	RemoveActionBindEvent(sf::Keyboard::Key slotKey)
		:slotKey(slotKey)
	{}
};

struct HitByTickDamageEvent : public IEvent
{
	Entity& entity;
	int damage;

	HitByTickDamageEvent(Entity& entity, int damage)
		:entity(entity),
		damage(damage)
	{}
};

struct AddSpellEffectEvent : public IEvent
{
	Entity& sourceEntity;
	Entity& hitEntity;
	SpellEffect spellEffect;

	AddSpellEffectEvent(Entity& sourceEntity, Entity& hitEntity, SpellEffect spellEffect)
		:sourceEntity(sourceEntity),
		hitEntity(hitEntity),
		spellEffect(spellEffect)
	{}
};

struct MoveSpeedChangedEvent : public IEvent
{
	Entity& entity;

	MoveSpeedChangedEvent(Entity& entity)
		:entity(entity)
	{}
};