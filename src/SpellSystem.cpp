#include "pch.h"
#include "SpellSystem.h"
#include "Utilities.h"
#include "MessageTypes.h"
#include "SpellHolder.h"
#include "TileMap.h"

SpellSystem::SpellSystem(SystemContext& systemContext, const TileMap& tileMap, const sf::Vector2f& mousePos)
	:ISystem(systemContext),
	mTileMap(tileMap),
	fMousePos(mousePos)
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
			if (isAOESpell(data.spellId))
			{
				if (doesSpellDependOnMousePos(data.spellId) && !canCastAtMousePos(data.caster))
					return;
			}
			updateLastSpell(spellbookComp, data.spellId);
		//	subtractMana(data.caster, spellbookComp);
			auto& thisSpell = spellbookComp.cSpells[data.spellId];
			thisSpell.cooldownRemaining = thisSpell.data->cooldown;
			if (data.usedKey)
				notifyUISystem(data.usedKey.value(), thisSpell.cooldownRemaining);

			auto animType = getAnimationBasedOnSpellType(*thisSpell.data);
			data.caster.getComponent<EntityStateComponent>().cCurrentState = EntityState::CastingSpell;

			notifyAnimationSystem(data.caster, thisSpell.data->castTime, animType);
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

void SpellSystem::notifyUISystem(sf::Keyboard::Key key, int& cooldown)
{
	mSystemContext.eventManager.notify<StartSpellCooldownUIEvent>(StartSpellCooldownUIEvent(key, cooldown));
}

void SpellSystem::notifyAnimationSystem(Entity& entity, int castTime, AnimationIdentifier animId)
{
	mSystemContext.eventManager.notify<PlayCastSpellAnimation>(PlayCastSpellAnimation(entity, castTime, animId));
}

void SpellSystem::notifyEffectSystem(const CastSpellEvent& data, const SpellInstance& spell)
{
	auto spellid = data.spellId;
	auto& evm = mSystemContext.eventManager;
	if(isHealingSpell(spellid))
		evm.notify<StartGlowUpEffect>(StartGlowUpEffect(data.caster, sf::Color(100, 255, 100), spell.data->castTime));
	else if (isRegenSpell(spellid))
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
	mSystemContext.eventManager.notify<UpdatePlayerStatusEvent>(UpdatePlayerStatusEvent()); //notifies ui system
}

bool SpellSystem::canCastAtMousePos(const Entity& entity) const
{
	//returns true, if there is line of sight between entitie's position and tile under mouse position
	//returns false otherwise
	const auto& positionComponent = entity.getComponent<PositionComponent>();
	const auto& pos = positionComponent.cLogicPosition;
	auto entityCell = Utilities::getCellIndex(pos);
	auto targetCell = Utilities::getCellIndex(fMousePos);

	return mTileMap.getTile(targetCell.x, targetCell.y)->isWalkableRaw() && mTileMap.isLineOfSightClear(entityCell, targetCell);
}

void SpellSystem::notifyCastFinished(Entity& entity, SpellIdentifier id)
{
	if (isRegenSpell(id))
		onRegenSpellCast(entity, id);
	else if (isHealingSpell(id))
		mSystemContext.eventManager.notify<TriggerHealSpellEvent>(TriggerHealSpellEvent(entity));
	else if (isProjectileSpell(id))
		mSystemContext.eventManager.notify<SpawnProjectileEvent>(SpawnProjectileEvent(entity, id));
	else if (isAOESpell(id))
		mSystemContext.eventManager.notify<CastAOESpellEvent>(CastAOESpellEvent(entity, id, fMousePos));
}

AnimationIdentifier SpellSystem::getAnimationBasedOnSpellType(const SpellData& data) const
{
	if (data.type == SpellType::Heal)
		return AnimationIdentifier::GenericSpellCast;
	else if (data.type == SpellType::Projectile)
		return AnimationIdentifier::GenericShoot;
	else if (data.type == SpellType::AreaOfEffect)
		return AnimationIdentifier::GenericSlashUnarmed;

	return AnimationIdentifier::GenericSpellCast;
}

void SpellSystem::onRegenSpellCast(Entity& entity, SpellIdentifier id)
{
	//those 2 are also a healing spells, but they require different system
	const SpellInstance* lastSpell = entity.getComponent<SpellbookComponent>().cLastSpell;
	int regen = (id == SpellIdentifier::HealthRegen) ?
		lastSpell->data->healing->bonusHpRegen :
		lastSpell->data->healing->bonusManaRegen;
	int duration = lastSpell->data->duration;

	if (id == SpellIdentifier::HealthRegen)
		mSystemContext.eventManager.notify<TriggerHpRegenSpellEvent>(TriggerHpRegenSpellEvent(entity, regen, duration));
	else if (id == SpellIdentifier::ManaRegen)
		mSystemContext.eventManager.notify<TriggerMpRegenSpellEvent>(TriggerMpRegenSpellEvent(entity, regen, duration));
}

bool SpellSystem::isHealingSpell(SpellIdentifier id) const
{
	return id == SpellIdentifier::QuickHeal || id == SpellIdentifier::MajorHeal;
}

bool SpellSystem::isRegenSpell(SpellIdentifier id) const
{
	return id == SpellIdentifier::HealthRegen || id == SpellIdentifier::ManaRegen;
}

bool SpellSystem::isProjectileSpell(SpellIdentifier id) const
{
	return id == SpellIdentifier::WaterBall || id == SpellIdentifier::PureProjectile ||
		   id == SpellIdentifier::Fireball  || id == SpellIdentifier::Bloodball;
}

bool SpellSystem::isAOESpell(SpellIdentifier id) const
{
	return id == SpellIdentifier::FrostPillar || id == SpellIdentifier::BladeDance ||
		id == SpellIdentifier::ToxicCloud || id == SpellIdentifier::Thunderstorm;
}

bool SpellSystem::doesSpellDependOnMousePos(SpellIdentifier id) const
{
	return id == SpellIdentifier::FrostPillar ||
		   id == SpellIdentifier::Thunderstorm;
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
