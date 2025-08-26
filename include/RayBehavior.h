#pragma once
#include "IBehavior.h"
#include "SpellIdentifiers.h"
#include "AnimationIdentifiers.h"

class RayBehavior
	:public IBehavior
{
public:
	RayBehavior(BehaviorContext& behaviorContext);
	virtual void update(Entity& entity, const sf::Time& deltaTime) override;
protected:
	virtual void determineNextTask(Entity& entity) override;
	virtual void swapToTargetting(Entity& entity) override;
	virtual void swapToAttacking(Entity& entity, Entity& target) override;
	virtual void fallbackOnNoDirection(Entity& self, Entity& target) override;
	virtual void handleTargettingLogic(Entity& self, Entity& target) override;
	virtual void handleAttackingLogic(Entity& self, Entity& target) override;
private:
	struct RayBehaviorConfig
	{
		struct PhaseConfig
		{
			int hpThreshold = 0; //in percentage
			std::vector<SpellIdentifier> unlockedSpells;
			std::vector<AnimationIdentifier> unlockedAttacks;
		};
		static constexpr size_t MaxPhases = 3;

		static inline const PhaseConfig phases[MaxPhases] = {
			{80, {}, {AnimationIdentifier::Attack1}},
			{50, {}, {AnimationIdentifier::Attack3}},
			{20, {}, {AnimationIdentifier::Attack2}}
		};
	};
	using PhaseConfig = RayBehaviorConfig::PhaseConfig;
	const PhaseConfig& getPhase(const Entity& entity);
private:
	bool canCastSpell(const Entity& entity, SpellIdentifier id);
	AnimationIdentifier getAttack(const Entity& entity);
};