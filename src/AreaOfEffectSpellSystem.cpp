#include "pch.h"
#include "AreaOfEffectSpellSystem.h"
#include "FrostPillarSpell.h"
#include "BladeDanceSpell.h"
#include "ToxicCloudSpell.h"
#include "ThunderstormSpell.h"

AreaOfEffectSpellSystem::AreaOfEffectSpellSystem(SystemContext& systemContext, const TileMap& tileMap)
	:ISystem(systemContext),
	mTileMap(tileMap)
{
	registerToEvents();
	createSpellsRegistry();
}

void AreaOfEffectSpellSystem::update(const sf::Time& deltaTime)
{
	for (auto& spell : mActiveSpells)
		spell->update(deltaTime, mSystemContext.eventManager);

	std::erase_if(mActiveSpells, [](const auto& spell) {
		return spell->isCompleted();
	});
}

void AreaOfEffectSpellSystem::render(sf::RenderWindow& window)
{
	for (auto& spell : mActiveSpells)
		spell->render(window);
}


void AreaOfEffectSpellSystem::registerToEvents()
{
	registerToCastAOESpellEvent();
}

void AreaOfEffectSpellSystem::registerToCastAOESpellEvent()
{
	mSystemContext.eventManager.registerEvent<CastAOESpellEvent>([this](const CastAOESpellEvent& data)
		{
			if (!mSpellsRegistry.contains(data.spellId))
				return;
			auto nextSpellPtr = mSpellsRegistry.at(data.spellId)(data);
			if (data.customDmg)
				nextSpellPtr->setCustomDamage(*data.customDmg);
			mActiveSpells.push_back(std::move(nextSpellPtr));
		});
}

void AreaOfEffectSpellSystem::createSpellsRegistry()
{
	mSpellsRegistry[SpellIdentifier::FrostPillar] = [this](const CastAOESpellEvent& data) {
		return std::make_unique<FrostPillarSpell>(data.source, mTileMap, data.castPos);
	};
	mSpellsRegistry[SpellIdentifier::BladeDance] = [this](const CastAOESpellEvent& data) {
		return std::make_unique<BladeDanceSpell>(data.source, mTileMap, data.castPos);
	};
	mSpellsRegistry[SpellIdentifier::ToxicCloud] = [this](const CastAOESpellEvent& data) {
		return std::make_unique<ToxicCloudSpell>(data.source, mTileMap, data.castPos);
	};
	mSpellsRegistry[SpellIdentifier::Thunderstorm] = [this](const CastAOESpellEvent& data) {
		return std::make_unique<ThunderstormSpell>(data.source, mTileMap, data.castPos);
	};
}
