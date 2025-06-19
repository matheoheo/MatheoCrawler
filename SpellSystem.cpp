#include "pch.h"
#include "SpellSystem.h"
#include "Utilities.h"
#include "MessageTypes.h"
#include "SpellHolder.h"

SpellSystem::SpellSystem(SystemContext& systemContext)
	:ISystem(systemContext)
{
	registerToEvents();
}

void SpellSystem::update(const sf::Time& deltaTime)
{
	for (const auto& entity : mTrackedEntities)
	{
		auto& spellBookComp = entity->getComponent<SpellbookComponent>();
		auto& casted = spellBookComp.cCasted;
		if (casted.empty())
			addToFinished(entity);

		for (auto it = std::begin(casted); it != std::end(casted);)
		{
			auto& spell = *it;
			spell->cooldownRemaining -= deltaTime.asMilliseconds();
			if (spell->cooldownRemaining <= 0)
			{
				spell->cooldownRemaining = 0;
				it = casted.erase(it);
			}
			else
				++it;
		}
	}

	removeFinishedEntities();
}

void SpellSystem::registerToEvents()
{
	registerToCastSpellEvent();
	registerToCastAnimationFinished();
}

void SpellSystem::registerToCastSpellEvent()
{
	mSystemContext.eventManager.registerEvent<CastSpellEvent>([this](const CastSpellEvent& data)
		{
			if (!Utilities::isEntityIdling(data.caster))
				return;

			auto& spellbookComp = data.caster.getComponent<SpellbookComponent>();
			if (!canCastSpell(data, spellbookComp))
				return;

			updateLastSpell(spellbookComp, data.spellId);
			subtractMana(data.caster, spellbookComp);
			auto& thisSpell = spellbookComp.cSpells[data.spellId];
			thisSpell.cooldownRemaining = thisSpell.data->cooldown;
			
			data.caster.getComponent<EntityStateComponent>().cCurrentState = EntityState::CastingSpell;

			notifyAnimationSystem(data.caster, thisSpell.data->castTime);
			notifyEffectSystem(data, thisSpell);
			if(!isEntityAlreadyTracked(data.caster))
				mTrackedEntities.push_back(&data.caster);
		});
}

void SpellSystem::registerToCastAnimationFinished()
{
	mSystemContext.eventManager.registerEvent<CastSpellFinishedEvent>([this](const CastSpellFinishedEvent& data)
		{
			notifyCastFinished(data.caster, data.id);
		});
}

void SpellSystem::notifyAnimationSystem(Entity& entity, int castTime)
{
	mSystemContext.eventManager.notify<PlayCastSpellAnimation>(PlayCastSpellAnimation(entity, castTime));
}

void SpellSystem::notifyEffectSystem(const CastSpellEvent& data, const SpellInstance& spell)
{
	auto spellid = data.spellId;
	auto& evm = mSystemContext.eventManager;
	if(spellid == SpellIdentifier::QuickHeal || spellid == SpellIdentifier::MajorHeal)
		evm.notify<StartGlowUpEffect>(StartGlowUpEffect(data.caster, sf::Color(100, 255, 100), spell.data->castTime));

	else if (spellid == SpellIdentifier::ManaRegen || spellid == SpellIdentifier::HealthRegen)
	{
		auto color = (spellid == SpellIdentifier::HealthRegen) ? Config::hpBarColor : Config::manaBarColor;
		evm.notify<StartRegenEffect>(StartRegenEffect(data.caster, color, spell.data->duration));
	}
}

bool SpellSystem::canCastSpell(const CastSpellEvent& data, SpellbookComponent& spellbookComp) const
{
	auto& spells = spellbookComp.cSpells;
	
	auto it = spells.find(data.spellId);
	if (it == std::end(spells) || !it->second.data) //cant find proper spell
		return false;

	if (it->second.cooldownRemaining > 0) //still not available to cast
		return false;

	auto myMana = data.caster.getComponent<CombatStatsComponent>().cMana;
	auto& spellData = it->second.data;

	if (myMana < spellData->cost) //no mana
		return false;

	return true;
}

void SpellSystem::updateLastSpell(SpellbookComponent& spellbookComp, SpellIdentifier id)
{
	if (spellbookComp.cSpells.contains(id))
	{
		spellbookComp.cLastSpell = &spellbookComp.cSpells.at(id);
		spellbookComp.cLastSpellId = id;
		spellbookComp.cCasted.emplace_back(spellbookComp.cLastSpell);
	}
}

void SpellSystem::subtractMana(Entity& entity, SpellbookComponent& spellbookComp)
{
	if (!entity.hasComponent<PlayerComponent>() ||  !spellbookComp.cLastSpell)
		return;

	auto lastSpellCost = spellbookComp.cLastSpell->data->cost;
	entity.getComponent<CombatStatsComponent>().cMana -= lastSpellCost;
	//notify ui system
	mSystemContext.eventManager.notify<UpdatePlayerStatusEvent>(UpdatePlayerStatusEvent());
}

void SpellSystem::notifyCastFinished(Entity& entity, SpellIdentifier id)
{
	if (id == SpellIdentifier::HealthRegen || id == SpellIdentifier::ManaRegen)
	{
		//those 2 are also a healing spells, but they require different system
		const SpellInstance* lastSpell = entity.getComponent<SpellbookComponent>().cLastSpell;
		int regen = (id == SpellIdentifier::HealthRegen) ? lastSpell->data->bonusHpRegen : lastSpell->data->bonusManaRegen;
		int duration = lastSpell->data->duration;

		if (id == SpellIdentifier::HealthRegen)
			mSystemContext.eventManager.notify<TriggerHpRegenSpellEvent>(TriggerHpRegenSpellEvent(entity, regen, duration));
		else if (id == SpellIdentifier::ManaRegen)
			mSystemContext.eventManager.notify<TriggerMpRegenSpellEvent>(TriggerMpRegenSpellEvent(entity, regen, duration));
	}
	else if (id == SpellIdentifier::QuickHeal || id == SpellIdentifier::MajorHeal)
	{
		mSystemContext.eventManager.notify<TriggerHealSpellEvent>(TriggerHealSpellEvent(entity));
	}
}

void SpellSystem::addToFinished(Entity* entity)
{
	mFinishedEntities.emplace_back(entity->getEntityId());
}

void SpellSystem::removeFinishedEntities()
{
	std::erase_if(mTrackedEntities, [this](const Entity* ent)
		{
			return std::ranges::find(mFinishedEntities, ent->getEntityId()) != std::ranges::end(mFinishedEntities);
		});

	mFinishedEntities.clear();
}
