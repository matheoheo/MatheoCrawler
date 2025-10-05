#pragma once
#include "IBehavior.h"
#include "SpellIdentifiers.h"
#include "AnimationIdentifiers.h"

struct SpellRule
{
	std::string name;
	SpellIdentifier spellId = SpellIdentifier::BossHeal;
	std::function<bool(const Entity&, const Entity&)> condition;
	std::function<void(Entity&, Entity&)> execute;
	int priority = 0;
};

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

	void determineCurrentSpell(Entity& self, Entity& target);
private:
	struct RayBehaviorConfig
	{
		//This color reflects how the entity looks. 
		//Will be used to manipulate look of some of the spells.
		static constexpr sf::Color rayColor{ 200, 100, 0 }; 
		struct PhaseConfig
		{
			int minHpPercent = 0;
			int maxHpPercent = 100;
			std::vector<AnimationIdentifier> unlockedAttacks;
			std::vector<SpellRule> spellRules;
		};
	};
	using PhaseConfig = RayBehaviorConfig::PhaseConfig;
	const PhaseConfig& getPhase(const Entity& entity);

	void setupPhases();
	void setupFirstPhase();
	void setupSecondPhase();
	void setupThirdPhase();
	void sortSpellRules(PhaseConfig& phase);
private:
	SpellRule createWaterBallRule();
	SpellRule createBeamRule();
	SpellRule createPureProjectileRule();
	SpellRule createBladeDanceRule();
	SpellRule createBossHealRule();
private:
	bool canCastSpell(const Entity& entity, SpellIdentifier id);
	bool canCastProjectile(const Entity& self, const Entity& target, int range, int axisTolerance = 0) const;
	void castProjectile(Entity& self, Entity& target, float dmgPercent, SpellIdentifier id);
	bool canCastBeam(const Entity& self, const Entity& target) const;
	void castBeam(Entity& self, Entity& target);
	int getDistance(const Entity& self, const Entity& target) const;
	AnimationIdentifier getAttack(const Entity& entity);
	void setupBehaviors();
	bool isInMeleeRange(const Entity& self, const Entity& target) const;

private:
	int getTargetsHealth(const Entity& target, float percent = 1.0f) const;
private:
	std::vector<PhaseConfig> mPhases;
	const SpellRule* mCurrentSpell;
};