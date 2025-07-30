#include "pch.h"
#include "StatusIconDisplaySystem.h"
#include "Utilities.h"

StatusIconDisplaySystem::StatusIconDisplaySystem(SystemContext& systemContext, AssetManager<TextureIdentifier, sf::Texture>& textures)
	:ISystem(systemContext),
	mTextures(textures),
	mIconSize({16.f, 16.f})
{
	registerToEvents();
}

void StatusIconDisplaySystem::update(const sf::Time& deltaTime)
{
	for (const auto& entity : mTrackedEntities)
	{
		removeFinishedIcons(*entity);
		positionIcons(*entity);
	}
	removeFinishedEntities();
}

void StatusIconDisplaySystem::render(sf::RenderWindow& window)
{
	for (const auto& entity : mTrackedEntities)
	{
		auto& iconsComp = entity->getComponent<StatusIconsComponent>();
		for (auto& icon : iconsComp.cIcons)
			window.draw(icon->sprite);
	}
}

void StatusIconDisplaySystem::registerToEvents()
{
	registerToAddSpellEffectEvent();
}

void StatusIconDisplaySystem::registerToAddSpellEffectEvent()
{
	mSystemContext.eventManager.registerEvent<AddSpellEffectEvent>([this](const AddSpellEffectEvent& data)
		{
			if (!entityHasIconAlready(data.hitEntity, data.spellEffect))
				addIconToEntity(data.hitEntity, data.spellEffect);

			if (!isEntityAlreadyTracked(data.hitEntity))
				mTrackedEntities.push_back(&data.hitEntity);
		});
}

void StatusIconDisplaySystem::removeFinishedEntities()
{
	std::erase_if(mTrackedEntities, [](const Entity* entity)
		{
			auto& iconsComp = entity->getComponent<StatusIconsComponent>();
			return iconsComp.cIcons.empty();
		});
}

void StatusIconDisplaySystem::removeFinishedIcons(Entity& entity)
{
	const auto& effectsComp = entity.getComponent<SpellEffectsComponent>();
	const auto& effects = effectsComp.cActiveEffects;
	auto& iconsComp = entity.getComponent<StatusIconsComponent>();
	auto& icons = iconsComp.cIcons;

	std::vector<SpellEffect> activeEffects;
	std::vector<SpellEffect> activeIcons;

	//populate vectors
	std::ranges::transform(effects, std::back_inserter(activeEffects), &ISpellEffect::getSpellEffectId);
	std::ranges::transform(icons, std::back_inserter(activeIcons), &StatusIconsComponent::StatusIcon::effectId);

	//Now, if we make a subtraction (activeIcons - activeEffects) we get SpellEffects that are no longer active
	//That means they are to be deleted.
	std::vector<SpellEffect> iconsToRemove;
	std::ranges::copy_if(activeIcons, std::back_inserter(iconsToRemove), [&](SpellEffect x) {
		return !std::ranges::any_of(activeEffects, [&](SpellEffect y) {
			return x == y;
			});
		});

	std::erase_if(icons, [&iconsToRemove](const auto& icon) {
		return std::ranges::find(iconsToRemove, icon->effectId)	!= std::ranges::end(iconsToRemove);
		});
}

void StatusIconDisplaySystem::positionIcons(Entity& entity)
{
	constexpr size_t maxIconsPerRow = 4;
	constexpr size_t maxIconsVisible = 8;
	constexpr float outOfScreenPos = -10000.f;
	auto& iconsComp = entity.getComponent<StatusIconsComponent>();
	auto& icons = iconsComp.cIcons;
	sf::Vector2f entPos = entity.getComponent<PositionComponent>().cLogicPosition;

	for (size_t i = 0; i < icons.size(); ++i)
	{
		//so size of icon is 16x16, that means we can fit only 4 icons on left and only 4 on right.
		float xPos = (i < maxIconsPerRow) ? entPos.x : entPos.x + Config::getCellSize().x - mIconSize.x;
		float yPos = (i < maxIconsPerRow) ? entPos.y + i * mIconSize.y : entPos.y + (i - maxIconsPerRow) * mIconSize.y;

		if (i >= maxIconsVisible)
		{
			//if we exceeded limit, then other icons are gonna be rendered out of screen.
			xPos = outOfScreenPos;
			yPos = outOfScreenPos;
		}

		icons[i]->sprite.setPosition({ xPos, yPos });
	}
}

bool StatusIconDisplaySystem::entityHasIconAlready(Entity& entity, SpellEffect effectId)
{
	auto& iconsComp = entity.getComponent<StatusIconsComponent>();
	auto& icons = iconsComp.cIcons;
	auto it = std::ranges::find(icons, effectId, &StatusIconsComponent::StatusIcon::effectId);
	
	return it != std::ranges::end(icons);
}

void StatusIconDisplaySystem::addIconToEntity(Entity& entity, SpellEffect effectId)
{
	using StatusIcon = StatusIconsComponent::StatusIcon;
	auto& iconsComp = entity.getComponent<StatusIconsComponent>();
	auto& icons = iconsComp.cIcons;
	auto textureId = getTextureIdForEffect(effectId);
	if (!textureId)
		return; //failed to find proper TextureIdentifier for this effect

	auto& texture = mTextures.get(textureId.value());
	std::unique_ptr<StatusIcon> icon = std::make_unique<StatusIcon>(texture, effectId);
	Utilities::scaleSprite(icon->sprite, mIconSize);

	icons.push_back(std::move(icon));
}

std::optional<TextureIdentifier> StatusIconDisplaySystem::getTextureIdForEffect(SpellEffect effectId)
{
	using MapPair = std::pair<SpellEffect, TextureIdentifier>;
	constexpr std::size_t effectsCount = 3;
	constexpr std::array<MapPair, effectsCount> mapping =
	{ {
		{SpellEffect::FireBurn,          TextureIdentifier::FireBurnIcon},
		{SpellEffect::MovementSpeedSlow, TextureIdentifier::MoveSpeedSlowIcon},
		{SpellEffect::MovementFrozen,    TextureIdentifier::MoveFrozenIcon}
	} };

	auto it = std::ranges::find(mapping, effectId, &MapPair::first);
	if (it != std::ranges::end(mapping))
		return it->second;

	return {};
}
