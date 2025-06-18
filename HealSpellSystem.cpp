#include "pch.h"
#include "HealSpellSystem.h"
#include "Utilities.h"

HealSpellSystem::HealSpellSystem(SystemContext& systemContext)
	:ISystem(systemContext)
{
	registerToEvents();
}

void HealSpellSystem::update(const sf::Time& deltaTime)
{
	
}

void HealSpellSystem::registerToEvents()
{
	registerToTriggerHealSpellEvent();
}

void HealSpellSystem::registerToTriggerHealSpellEvent()
{
	mSystemContext.eventManager.registerEvent<TriggerHealSpellEvent>([this](const TriggerHealSpellEvent& data)
		{
			auto value = data.caster.getComponent<SpellbookComponent>().cLastSpell->data->healValue;
			heal(data.caster, value);
		});
}

void HealSpellSystem::heal(const Entity& entity, float healPercent)
{
	auto& combatStats = entity.getComponent<CombatStatsComponent>();
	int healValue = static_cast<int>(combatStats.cMaxHealth * healPercent);
	combatStats.cHealth = std::clamp(combatStats.cHealth + healValue, 0, combatStats.cMaxHealth);
	std::cout << "x\n";
}
