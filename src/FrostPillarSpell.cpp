#include "pch.h"
#include "FrostPillarSpell.h"
#include "TileMap.h"
#include "SpellHolder.h"
#include "Config.h"
#include "Utilities.h"
#include "Random.h"
#include "EventManager.h"

FrostPillarSpell::FrostPillarSpell(const Entity& caster, const TileMap& tileMap, const sf::Vector2f& castPos)
	:IAOESpell(caster, tileMap, castPos),
	mUpdateEffect(true)
{
	mCastTime = 2500; //After 2.5 second the effect takes place.
	initEffects();
}

void FrostPillarSpell::update(const sf::Time& deltaTime, EventManager& eventManager)
{
	if (mUpdateEffect)
	{
		for (auto& effect : mFrozenEffects)
			updateEffect(effect, deltaTime);
	}
	addTimePassed(deltaTime);
	if (hasCastFinished())
		onCastFinish(eventManager);
}

void FrostPillarSpell::render(sf::RenderWindow& window)
{
	renderEffects(window);
}

void FrostPillarSpell::initEffects()
{
	const auto& offsets = SpellHolder::getInstance().get(SpellIdentifier::FrostPillar).aoe->offsets;
	auto affectedTiles = getAffectedTiles(offsets, mCastPos);
	for (const Tile* tile : affectedTiles)
		mFrozenEffects.push_back(createEffect(tile->tileVisual.getPosition()));

}

void FrostPillarSpell::renderEffects(sf::RenderWindow& window)
{
	for (const auto& effect : mFrozenEffects)
	{
		window.draw(effect.frozenShape);
	}
}

FrostPillarSpell::FrozenTileEffect FrostPillarSpell::createEffect(const sf::Vector2f& pos)
{
	constexpr float outlineThickness = 1.f;
	constexpr std::uint8_t alphaZero = 0;

	FrozenTileEffect effect;
	effect.frozenShape.setSize(Config::getCellSize());
	effect.frozenShape.setFillColor(getColorMod(FrostColors::fade, alphaZero));
	effect.frozenShape.setPosition(pos);
	effect.frozenShape.setOutlineThickness(outlineThickness);
	effect.frozenShape.setOutlineColor(getColorMod(FrostColors::aura, alphaZero));

	return effect;
}

void FrostPillarSpell::updateEffect(FrozenTileEffect& effect, const sf::Time& deltaTime)
{
	float progress = getProgressRatio();
	std::uint8_t nextAlpha = static_cast<std::uint8_t>(progress * FrostColors::fade.a);
	effect.frozenShape.setFillColor(getColorMod(FrostColors::fade, nextAlpha));

	//for outline, we want to achieve a pulsing effect', not just straight fading in.
	constexpr int pulsesCount = 3; 
	constexpr float pi = std::numbers::pi_v<float>;
	constexpr std::uint8_t maxAlpha = FrostColors::aura.a;
	const float wave = std::sin(progress * pi * pulsesCount);
	const sf::Color nextColor{ getColorMod(FrostColors::aura, wave * maxAlpha) };
	effect.frozenShape.setOutlineColor(nextColor);
}

void FrostPillarSpell::onCastFinish(EventManager& eventManager)
{
	const auto& spell = SpellHolder::getInstance().get(SpellIdentifier::FrostPillar);
	const auto& offsets = spell.aoe->offsets;
	auto hitEntities = getAffectedEntities(offsets, mCastPos);

	for (Entity* ent : hitEntities)
	{
		int dmg = Random::get(spell.aoe->minDmg, spell.aoe->maxDmg);
		eventManager.notify<HitByAOESpellEvent>(HitByAOESpellEvent(*ent, dmg));
		eventManager.notify<AddSpellEffectEvent>(AddSpellEffectEvent(*ent, SpellEffect::MovementFrozen));
	}
	complete();
}

sf::Color FrostPillarSpell::getColorMod(const sf::Color& color, std::uint8_t alpha) const
{
	return sf::Color
	{
		color.r,
		color.g,
		color.b,
		alpha
	};
}

sf::Vector2f FrostPillarSpell::getRandomPosWithinCell(const sf::Vector2f& cellPos) const
{
	int minX = static_cast<int>(cellPos.x);
	int minY = static_cast<int>(cellPos.y);

	int maxX = minX + static_cast<int>(Config::getCellSize().x);
	int maxY = minY + static_cast<int>(Config::getCellSize().y);

	float x = static_cast<float>(Random::get(minX, maxX));
	float y = static_cast<float>(Random::get(minY, maxY));

	return {x,y};
}


