#pragma once
#include <string>
#include "Directions.h"
#include "EntityStates.h"
#include "AnimationIdentifiers.h"
#include "AnimationFrame.h"
#include "EntityAIState.h"
#include "IBehavior.h"

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

	ChaseAIComponent()
		:cTarget(nullptr),
		cTimeSinceLastRecalculation(0.f) {}
};

struct PathComponent : public IComponent
{
	std::deque<sf::Vector2i> cPathCells;
};

struct AttackRangeComponent : public IComponent
{
	int cAttackRange;

	AttackRangeComponent(int range = 1)
		:cAttackRange(range) {}
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