#include "pch.h"
#include "PlayerRegenerationSystem.h"

PlayerRegenerationSystem::PlayerRegenerationSystem(SystemContext& systemContext)
	:ISystem(systemContext)
{
}

void PlayerRegenerationSystem::update(const sf::Time& deltaTime)
{
	auto& player = mSystemContext.entityManager.getPlayer();
	auto& regenComp = player.getComponent<RegenerationComponent>();
	addToLastRegenTime(regenComp, deltaTime);
	if (isRegenerationDue(player,regenComp))
	{
		regenerate(player, regenComp);
	}

}

void PlayerRegenerationSystem::addToLastRegenTime(RegenerationComponent& regenComp, const sf::Time& deltaTime)
{
	regenComp.cTimeSinceLastRegen += deltaTime.asMilliseconds();
}

bool PlayerRegenerationSystem::isRegenerationDue(Entity& player, RegenerationComponent& regenComp) const
{
	constexpr int regenerateInterval = 2000; //regenerate every 2 seconds.
	return regenComp.cTimeSinceLastRegen >= regenerateInterval;
}

void PlayerRegenerationSystem::regenerate(Entity& player, RegenerationComponent& regenComp)
{
	auto& combatStats = player.getComponent<CombatStatsComponent>();
	bool needRegenHp   = combatStats.cHealth  < combatStats.cMaxHealth;
	bool needRegenMana = combatStats.cMana  < combatStats.cMaxMana;

	if (needRegenHp)
	{
		int hpPlus = regenComp.cHpRegen;
		if (combatStats.cHealth + hpPlus > combatStats.cMaxHealth)
			hpPlus = combatStats.cMaxHealth - combatStats.cHealth;
		combatStats.cHealth += hpPlus;
	}
	if (needRegenMana)
	{
		int manaPlus = regenComp.cManaRegen;
		if (combatStats.cMana + manaPlus > combatStats.cMaxMana)
			manaPlus = combatStats.cMaxMana - combatStats.cMana;
		combatStats.cMana += manaPlus;
	}

	regenComp.cTimeSinceLastRegen = 0;
	if (needRegenHp || needRegenMana)
		notifyStatusUI();
}

void PlayerRegenerationSystem::notifyStatusUI()
{
	mSystemContext.eventManager.notify<UpdatePlayerStatusEvent>(UpdatePlayerStatusEvent());
}
