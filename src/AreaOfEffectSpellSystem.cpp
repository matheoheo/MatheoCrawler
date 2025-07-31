#include "pch.h"
#include "AreaOfEffectSpellSystem.h"
#include "FrostPillarSpell.h"
#include "BladeDanceSpell.h"

AreaOfEffectSpellSystem::AreaOfEffectSpellSystem(SystemContext& systemContext, const TileMap& tileMap)
	:ISystem(systemContext),
	mTileMap(tileMap)
{
	registerToEvents();
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
			//mActiveSpells.emplace_back(std::make_unique<FrostPillarSpell>(data.source, mTileMap, data.castPos));
			mActiveSpells.emplace_back(std::make_unique<BladeDanceSpell>(data.source, mTileMap, data.castPos));
		});
}
