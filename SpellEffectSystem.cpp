#include "pch.h"
#include "SpellEffectSystem.h"
#include "ISpellEffect.h"
#include "FireBurnSpellEffect.h"

SpellEffectSystem::SpellEffectSystem(SystemContext& systemContext)
	:ISystem(systemContext)
{
	registerToEvents();
	createEffectRegistry();
}

void SpellEffectSystem::update(const sf::Time& deltaTime)
{
	for (const auto& entity : mTrackedEntities)
	{
		auto& effectsComp = entity->getComponent<SpellEffectsComponent>();
		auto& actives = effectsComp.cActiveEffects;

		for (auto& effect : actives)
			effect->update(*entity, mSystemContext.eventManager, deltaTime);

		removeFinishedEffects(effectsComp);
	}
}

void SpellEffectSystem::registerToEvents()
{
	registerToAddSpellEffectEvent();
}

void SpellEffectSystem::registerToAddSpellEffectEvent()
{
	mSystemContext.eventManager.registerEvent<AddSpellEffectEvent>([this](const AddSpellEffectEvent& data)
		{
			auto& effectsComp = data.hitEntity.getComponent<SpellEffectsComponent>();
			auto& actives = effectsComp.cActiveEffects;

			if (!mEffectRegistry.contains(data.spellEffect))
				return;
			auto newEffect = mEffectRegistry.at(data.spellEffect)(); //mEffectRegistry holds lambdas, that's why additional ()
			//maybe this type of effect already exists on target entity, so we try to merge
			for (auto& effect : actives)
			{
				if (effect->tryMerge(*newEffect)) //returns true if merging was successfull
					return;
			}
			//in case there was not this effect on entity - we just push it.
			actives.push_back(std::move(newEffect));
			if (!isEntityAlreadyTracked(data.hitEntity))
				mTrackedEntities.push_back(&data.hitEntity);
			std::cout << "Pushed new!\n";
		});
}

void SpellEffectSystem::removeFinishedEffects(SpellEffectsComponent& spellEffectsComp)
{
	auto& actives = spellEffectsComp.cActiveEffects;
	std::erase_if(actives, [](const auto& effect)
		{
			return effect->hasEffectFinished();
		});
}

void SpellEffectSystem::createEffectRegistry()
{
	mEffectRegistry[SpellEffect::FireBurn] = []() {
		return std::make_unique<FireBurnSpellEffect>(3000, 19); //just for test for now
	};
}
