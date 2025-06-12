#pragma once
#include <string>
#include "Directions.h"
#include "EntityStates.h"
#include "AnimationIdentifiers.h"
#include "AnimationFrame.h"
#include "EntityTypes.h"
#include "EntityAIState.h"
#include "IBehavior.h"
#include "AttackData.h"
#include "Config.h"

class Entity;

struct IComponent
{
	virtual ~IComponent() = default;
};

struct TagComponent : public IComponent 
{
	std::string cTag;
	TagComponent(const std::string& tag) : cTag(tag) {}
};

struct MovementComponent : public IComponent
{
	float cMoveSpeed;
	sf::Vector2f cInitialPosition;
	sf::Vector2f cNextPos;
	sf::Vector2f cDirectionVector;

	MovementComponent(float moveSpeed) 
		: cMoveSpeed(moveSpeed) 
	{}
};

struct PlayerComponent : public IComponent
{
	//
};

struct EnemyComponent : public IComponent
{

};

struct BossComponent : public IComponent
{

};

struct EntityTypeComponent : public IComponent
{
	EntityType cEntityType;

	EntityTypeComponent(EntityType type)
		:cEntityType(type) {}
};

struct SpriteComponent : public IComponent
{
	sf::Sprite cSprite;

	SpriteComponent(const sf::Texture& texture)
		:cSprite(texture)
	{}
};

struct DirectionComponent : public IComponent
{
	Direction cCurrentDir;
	Direction cNextDir;
	Direction cLastDir;
	DirectionComponent(Direction dir = Direction::Up) 
		:cCurrentDir(dir),
		 cNextDir(dir),
		 cLastDir(dir)
	{}
};

struct EntityStateComponent : public IComponent
{
	EntityState cCurrentState;

	EntityStateComponent() : cCurrentState(EntityState::Idle) {}
};

struct AnimationComponent : public IComponent
{
	const std::vector<AnimationFrame>* cFrames;
	float cFrameDuration;
	float cTimer;
	size_t cCurrentIndex;
	AnimationIdentifier cCurrentId;
	sf::Vector2f cStartPosition; //this is needed, because each frame has its own offset, and we need to keep changing positions
	bool cApplyOffset;
	const AnimationIdentifier cDefaultAnimId;

	AnimationComponent()
		:cFrames(nullptr),
		 cFrameDuration(80.f),
		 cTimer(0.f),
		 cCurrentIndex(0), 
		 cCurrentId(AnimationIdentifier::GenericSpellCast),
		 cApplyOffset(false),
		 cDefaultAnimId(AnimationIdentifier::GenericWalk)
	{}
};

struct FieldOfViewComponent : public IComponent
{
	int cRangeOfView;

	FieldOfViewComponent(int range = 5) 
		: cRangeOfView(range) {}
};
struct EntityAIStateComponent : public IComponent
{
	EntityAIState cState;

	EntityAIStateComponent(EntityAIState state = EntityAIState::None)
		:cState(state) {}
};
struct PatrolAIComponent : public IComponent
{
	float cElapsed; //time elapsed since last movement
	float cPatrolCooldown; //time before next movement;

	PatrolAIComponent(float patrolCooldown = 2000.f)
		:cElapsed(0.f),
		cPatrolCooldown(patrolCooldown) {}
};

struct ChaseAIComponent : public IComponent
{
	Entity* cTarget;
	float cTimeSinceLastRecalculation;
	int cUnreachableRetryCount;
	bool cTargetReachableByPath;

	ChaseAIComponent()
		:cTarget(nullptr),
		cTimeSinceLastRecalculation(0.f),
		cUnreachableRetryCount(0),
		cTargetReachableByPath(false){}
};

struct PathComponent : public IComponent
{
	std::deque<sf::Vector2i> cPathCells;
};

struct AITimersComponent : public IComponent
{
	float cTimeSinceTargetWasLastSeen;
	float cTimeSinceLastLOSCheck;

	AITimersComponent()
		:cTimeSinceTargetWasLastSeen(0.f),
		cTimeSinceLastLOSCheck(0.f) {}
};

struct BehaviorComponent : public IComponent
{
	using BehaviorPtr = std::unique_ptr<IBehavior>;
	BehaviorPtr cBehavior;

	BehaviorComponent(BehaviorPtr behavior)
		:cBehavior(std::move(behavior)) {}
};

struct CombatStatsComponent : public IComponent
{
	int cHealth;
	int cMaxHealth;
	int cAttackDamage;
	int cAttackRange; //in tiles
	int cDefence;
	float cAttackSpeed; //attacks per second

	CombatStatsComponent()
		:cHealth(100),
		cMaxHealth(cHealth),
		cAttackDamage(45),
		cAttackRange(1),
		cDefence(3),
		cAttackSpeed(1.0f) {}
};

struct AttackComponent : public IComponent
{
	std::unordered_map<AnimationIdentifier, AttackData> cAttackDataMap;
	float cAttackCooldownTimer;
	AttackData* cLastAttackData;
	AnimationIdentifier cLastAttackId;
	AnimationIdentifier cNextAttackId;

	AttackComponent()
		:cAttackCooldownTimer(1000.f),
		cLastAttackData(nullptr),
		cLastAttackId(AnimationIdentifier::Attack1),
		cNextAttackId(AnimationIdentifier::Attack1) 
	{}
};

struct HealthBarComponent : public IComponent
{
	sf::RectangleShape cForegroundBar;
	sf::RectangleShape cBackgroundBar;
	bool cIsVisible;
	int cVisibleTimer; 

	HealthBarComponent(const sf::Vector2f& size = Config::hpBarDefaultSize)
		:cIsVisible(true),
		cVisibleTimer(0)
	{
		cBackgroundBar.setFillColor(Config::hpBarBackgroundColor);
		cBackgroundBar.setSize(size);
		cBackgroundBar.setOutlineColor(Config::hpBarOutlineColor);
		cBackgroundBar.setOutlineThickness(1.f);

		cForegroundBar.setSize(size);
		cForegroundBar.setFillColor(Config::hpBarForegroundColor);
	}
};