#pragma once
#include <string>
#include <map>
#include "Directions.h"
#include "EntityStates.h"
#include "AnimationIdentifiers.h"
#include "AnimationFrame.h"
#include "EntityTypes.h"
#include "EntityAIState.h"
#include "IBehavior.h"
#include "AttackData.h"
#include "Config.h"
#include "SpellData.h"
#include "ISpellEffect.h"
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

struct PositionComponent : public IComponent
{
	sf::Vector2f cLogicPosition;

	PositionComponent(sf::Vector2f pos = { 0.f, 0.f })
		:cLogicPosition(pos)
	{}
};

struct MovementComponent : public IComponent
{
	bool cMovementBlocked; //< debuff, movement might be frozen
	float cBaseMoveSpeed;
	float cMoveSpeed;  //this might be effected by some buffs or debuffs
	sf::Vector2f cInitialPosition;
	sf::Vector2f cNextPos;
	sf::Vector2f cDirectionVector;

	MovementComponent(float moveSpeed) 
		: cBaseMoveSpeed(moveSpeed),
		cMoveSpeed(moveSpeed),
		cMovementBlocked(false)
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
	int cUnreachableRetryCount;
	bool cTargetReachableByPath;

	ChaseAIComponent()
		:cTarget(nullptr),
		cUnreachableRetryCount(0),
		cTargetReachableByPath(false)
	{}
};

struct PathComponent : public IComponent
{
	int cTimeSinceLastRecalculation;
	sf::Vector2i cTargetCell;
	std::deque<sf::Vector2i> cPathCells;
	bool cPathAborted;

	PathComponent()
		:cTimeSinceLastRecalculation(0),
		cPathAborted(false)
	{}
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
	float cBaseAttackSpeed; //attacks per second
	float cAttackSpeed; //this might be affected by buffs or debuffs
	
	int cMana;
	int cMaxMana;
	int cMagicDefence;

	CombatStatsComponent()
		:cHealth(100),
		cMaxHealth(cHealth),
		cAttackDamage(45),
		cAttackRange(1),
		cDefence(3),
		cBaseAttackSpeed(1.0f),
		cAttackSpeed(1.0f),
		cMana(12),
		cMaxMana(cMana),
		cMagicDefence(1){}

	CombatStatsComponent(const CombatStatsComponent& other)
		:cHealth(other.cHealth),
		cMaxHealth(other.cMaxHealth),
		cAttackDamage(other.cAttackDamage),
		cAttackRange(other.cAttackRange),
		cDefence(other.cDefence),
		cBaseAttackSpeed(other.cBaseAttackSpeed),
		cAttackSpeed(other.cAttackSpeed),
		cMana(other.cMana),
		cMaxMana(other.cMaxMana),
		cMagicDefence(other.cMagicDefence)
	{}
};

struct AttackComponent : public IComponent
{
	std::unordered_map<AnimationIdentifier, AttackData> cAttackDataMap;
	float cAttackCooldownTimer;
	AttackData* cLastAttackData;
	AnimationIdentifier cLastAttackId;
	AnimationIdentifier cNextAttackId;

	AttackComponent()
		:cAttackCooldownTimer(0.f),
		cLastAttackData(nullptr),
		cLastAttackId(AnimationIdentifier::Attack1),
		cNextAttackId(AnimationIdentifier::Attack1) 
	{}
};

//for AI
//each entity might have different sets of attacks
struct AvailableAttacksComponent : public IComponent
{
	std::vector<AnimationIdentifier> mAttacks;

	AvailableAttacksComponent() { mAttacks.emplace_back(AnimationIdentifier::Attack1); }
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
	~HealthBarComponent()
	{
		cIsVisible = false;
	}
};
struct PlayerManaBarComponent : public IComponent
{
	sf::RectangleShape cForegroundBar;
	sf::RectangleShape cBackgroundBar;
	
	PlayerManaBarComponent()
	{
		cForegroundBar.setSize(Config::manaBarSize);
		cBackgroundBar.setSize(Config::manaBarSize);
		cForegroundBar.setFillColor(Config::manaBarColor);
		cBackgroundBar.setFillColor({ 43, 24, 92 });
		cBackgroundBar.setOutlineColor({ 161, 135, 255 });
		cBackgroundBar.setOutlineThickness(1.5f);
	}
};

struct PlayerResourcesComponent : public IComponent
{
	int cGold;
	PlayerResourcesComponent()
		:cGold(100)
	{}
};

struct AttackSelectionComponent : public IComponent
{
	AnimationIdentifier cSelectedId;

	AttackSelectionComponent()
		:cSelectedId(AnimationIdentifier::Attack1)
	{}
};

struct RegenerationComponent : public IComponent
{
	int cHpRegen;
	int cManaRegen;
	int cTimeSinceLastRegen;

	int cHpRegenBonus;
	int cManaRegenBonus;
	int cHpBonusDuration;
	int cManaBonusDuration;
	RegenerationComponent()
		:cHpRegen(1),
		cManaRegen(1),
		cTimeSinceLastRegen(0),
		cHpRegenBonus(0),
		cManaRegenBonus(0),
		cHpBonusDuration(0),
		cManaBonusDuration(0)
	{}
};

struct SpellbookComponent : public IComponent
{
	std::unordered_map<SpellIdentifier, SpellInstance> cSpells; //every spell, that entity can use is in this map
	SpellInstance* cLastSpell;
	SpellIdentifier cLastSpellId;
	std::vector<SpellInstance*> cCasted; //keep lastly casted spell instances, so we can subtract cooldown timer.
	SpellbookComponent()
		: cLastSpell(nullptr),
		cLastSpellId(SpellIdentifier::QuickHeal)
	{}
};

struct SpellProjectileComponent : public IComponent
{
	const ProjectileSpell* cSpellData;
	Entity* cSource;
	float cDistanceTraveled;
	bool cPlayerCasted;
	float cMaxDistance;
	std::vector<size_t> cHitTargets; //keep track of hit target ids so we do not rehit them.
	int cFinalDmg;

	SpellProjectileComponent(const ProjectileSpell& spellData, Entity& source, bool playerCasted, int cDmg)
		:cSpellData(&spellData),
		cSource(&source),
		cDistanceTraveled(0.f),
		cPlayerCasted(playerCasted),
		cMaxDistance(Config::getCellSize().x * spellData.range),
		cFinalDmg(cDmg)
	{}
};

//UI
struct AssignedSpellsComponent : public IComponent
{
	std::map<int, SpellInstance*> cAssignedSpells; //in UI slots
};

struct StatisticsUpgradeComponent : public IComponent
{
	std::unordered_map<std::string, int> cUpgradesMap;
};

struct PositioningComponent : public IComponent
{
	int cMinRange;
	int cMaxRange;
	bool cFocusOnDistance; //is entity more focused on keeping max range, or on finding any closest position.
	sf::Vector2i cTargetCell;
	bool cLastRepositionTryFailed;

	PositioningComponent(int minRange, int maxRange)
		:cMinRange(minRange),
		cMaxRange(maxRange),
		cFocusOnDistance(true),
		cLastRepositionTryFailed(false)
	{}
};

struct RangedEnemyComponent : public IComponent
{
	//Ranged enemies cast projectiles as basic attacks.
	//This variable determines a spell that is assigned for enemy entity that acts as a basic attack.
	SpellIdentifier cSpellId;

	RangedEnemyComponent(SpellIdentifier spellId)
		:cSpellId(spellId)
	{}
};

struct SpellEffectsComponent : public IComponent
{
	std::vector<std::unique_ptr<ISpellEffect>> cActiveEffects;
};

struct StatusIconsComponent : public IComponent
{
	struct StatusIcon {
		sf::Sprite sprite;
		SpellEffect effectId;

		StatusIcon(const sf::Texture& texture, SpellEffect effectId)
			:sprite(texture),
			effectId(effectId)
		{}
	};

	std::vector<std::unique_ptr<StatusIcon>> cIcons;
};