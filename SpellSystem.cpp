#include "pch.h"
#include "SpellSystem.h"
#include "Utilities.h"
#include "MessageTypes.h"

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
			spellbookComp.cLastSpellId = data.spellId;
			
			data.caster.getComponent<EntityStateComponent>().cCurrentState = EntityState::CastingSpell;
			notifyAnimationSystem(data.caster, thisSpell.data->castTime);
			notifyEffectSystem(data.caster, sf::Color(100, 255, 100), thisSpell.data->castTime * 2);
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

void SpellSystem::notifyEffectSystem(Entity& entity, const sf::Color& color, int durationTime)
{
	mSystemContext.eventManager.notify<StartGlowUpEffect>(StartGlowUpEffect(entity, color, durationTime));
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

void SpellSystem::notifyCastFinished(const Entity& entity, SpellIdentifier id)
{
	if (id == SpellIdentifier::BasicHeal)
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
