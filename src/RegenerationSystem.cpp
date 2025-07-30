#include "pch.h"
#include "RegenerationSystem.h"

RegenerationSystem::RegenerationSystem(SystemContext& systemContext)
	:ISystem(systemContext)
{
	registerToEvents();
}

void RegenerationSystem::update(const sf::Time& deltaTime)
{
	for (const auto& entity : mTrackedEntities)
	{
		auto& regenComp = entity->getComponent<RegenerationComponent>();
		addToLastRegenTime(regenComp, deltaTime);
		if (isRegenerationDue(*entity, regenComp))
		{
			regenerate(*entity, regenComp);
			resetRegenerationTimer(regenComp);
		}
		updateBonusRegeneration(regenComp, deltaTime);
	}
}

void RegenerationSystem::registerToEvents()
{
	registerToRegenerationSpellCasted();
}

void RegenerationSystem::registerToRegenerationSpellCasted()
{
	mSystemContext.eventManager.registerEvent<TriggerHpRegenSpellEvent>([this](const TriggerHpRegenSpellEvent& data)
		{
			auto& regenComp = data.caster.getComponent<RegenerationComponent>();
			regenComp.cHpRegenBonus = data.bonusRegen;
			regenComp.cHpBonusDuration = data.duration;

			if (!isEntityAlreadyTracked(data.caster))
				mTrackedEntities.push_back(&data.caster);
		});

	mSystemContext.eventManager.registerEvent<TriggerMpRegenSpellEvent>([this](const TriggerMpRegenSpellEvent& data)
		{
			auto& regenComp = data.caster.getComponent<RegenerationComponent>();
			regenComp.cManaRegenBonus = data.bonusRegen;
			regenComp.cManaBonusDuration = data.duration;

			if (!isEntityAlreadyTracked(data.caster))
				mTrackedEntities.push_back(&data.caster);
		});
}

void RegenerationSystem::addToLastRegenTime(RegenerationComponent& regenComp, const sf::Time& deltaTime)
{
	regenComp.cTimeSinceLastRegen += deltaTime.asMilliseconds();
}

bool RegenerationSystem::isRegenerationDue(Entity& player, RegenerationComponent& regenComp) const
{
	constexpr int regenerateInterval = 2000; //regenerate every 2 seconds.
	return regenComp.cTimeSinceLastRegen >= regenerateInterval;
}

void RegenerationSystem::regenerate(Entity& player, RegenerationComponent& regenComp)
{
	auto& combatStats = player.getComponent<CombatStatsComponent>();
	bool needRegenHp   = combatStats.cHealth  < combatStats.cMaxHealth;
	bool needRegenMana = combatStats.cMana  < combatStats.cMaxMana;

	if (needRegenHp)
	{
		int hpPlus = regenComp.cHpRegen + regenComp.cHpRegenBonus;
		if (combatStats.cHealth + hpPlus > combatStats.cMaxHealth)
			hpPlus = combatStats.cMaxHealth - combatStats.cHealth;
		combatStats.cHealth += hpPlus;
	}
	if (needRegenMana)
	{
		int manaPlus = regenComp.cManaRegen + regenComp.cManaRegenBonus;
		if (combatStats.cMana + manaPlus > combatStats.cMaxMana)
			manaPlus = combatStats.cMaxMana - combatStats.cMana;
		combatStats.cMana += manaPlus;
	}

	if (needRegenHp || needRegenMana)
		notifyStatusUI();
}

void RegenerationSystem::resetRegenerationTimer(RegenerationComponent& regenComp)
{
	regenComp.cTimeSinceLastRegen = 0;
}

void RegenerationSystem::updateBonusRegeneration(RegenerationComponent& regenComp, const sf::Time& deltaTime)
{

	if (regenComp.cHpBonusDuration > 0)
	{
		regenComp.cHpBonusDuration -= deltaTime.asMilliseconds();
		if (regenComp.cHpBonusDuration <= 0)
		{
			regenComp.cHpRegenBonus = 0;
			regenComp.cHpBonusDuration = 0;
		}
	}

	if (regenComp.cManaBonusDuration > 0)
	{
		regenComp.cManaBonusDuration -= deltaTime.asMilliseconds();
		if (regenComp.cManaBonusDuration <= 0)
		{
			regenComp.cManaBonusDuration = 0;
			regenComp.cManaRegenBonus = 0;
		}
	}
}

void RegenerationSystem::notifyStatusUI()
{
	mSystemContext.eventManager.notify<UpdatePlayerStatusEvent>(UpdatePlayerStatusEvent());
}
