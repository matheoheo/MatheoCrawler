#include "pch.h"
#include "BladeDanceSpell.h"
#include "SpellHolder.h"
#include "Entity.h"
#include "Utilities.h"
#include "Random.h"
#include "EventManager.h"


BladeDanceSpell::BladeDanceSpell(const Entity& caster, const TileMap& tileMap, const sf::Vector2f& castPos)
	:IAOESpell(caster, tileMap, castPos),
	mTicksCount(0)
{
	createBlades();
}

void BladeDanceSpell::update(const sf::Time& deltaTime, EventManager& eventManager)
{
	addTimePassed(deltaTime);
	if (isTimeToMakeDamage())
	{
		makeDamage(eventManager);
		++mTicksCount;
	}

	if (mTimePassed > Timing::spellDuration)
		complete();

	updateAllBlades(deltaTime);
}

void BladeDanceSpell::render(sf::RenderWindow& window)
{
	for (const auto& blade : mBlades)
		window.draw(blade.shape);
}

bool BladeDanceSpell::isTimeToMakeDamage() const
{
	return mTimePassed - (Timing::damageTickInterval * mTicksCount) > Timing::damageTickInterval;
}

void BladeDanceSpell::makeDamage(EventManager& eventManager)
{
	const auto& pos = mCaster.getComponent<PositionComponent>().cLogicPosition;
	const auto& spell = SpellHolder::getInstance().get(SpellIdentifier::BladeDance);
	const auto& offsets = spell.aoe->offsets;
	bool isCasterPlayer = mCaster.hasComponent<PlayerComponent>();

	auto hitEntities = getAffectedEntities(offsets, pos);

	for (Entity* ent : hitEntities)
	{
		bool isEntPlayer = ent->hasComponent<PlayerComponent>();
		//no friendly damage between monsters.
		if (!isCasterPlayer && !isEntPlayer)
			continue;

		int dmg = Random::get(spell.aoe->minDmg, spell.aoe->maxDmg);
		eventManager.notify<HitByAOESpellEvent>(HitByAOESpellEvent(*ent, dmg, isCasterPlayer));
	}
}

BladeDanceSpell::BladeRect BladeDanceSpell::createBladeRect(sf::Angle angle, bool isFriendly) const
{
	BladeRect blade;
	blade.shape.setSize(Visual::bladeSize);
	blade.shape.setOrigin(Visual::bladeSize * 0.5f);
	blade.shape.setFillColor(isFriendly ? Visual::friendlyColor : Visual::enemyColor);
	blade.angle = angle;

	return blade;
}

void BladeDanceSpell::createBlades()
{
	bool friendly = mCaster.hasComponent<PlayerComponent>();
	constexpr float pi = std::numbers::pi_v<float>;
	constexpr float fullRotationRad = 2.f * pi;
	for (int i = 0; i < Visual::bladeCount; ++i)
	{
		sf::Angle angle = sf::radians( i * fullRotationRad / Visual::bladeCount);
		mBlades.push_back(createBladeRect(angle, friendly));
	}
}

void BladeDanceSpell::updateBlade(BladeRect& blade, const sf::Time& deltaTime)
{
	const auto& casterPos = getCasterPos();
	const auto centeredPos = casterPos + (Config::getCellSize() * 0.5f);

	blade.angle += sf::radians(Visual::rotationSpeedRad * deltaTime.asSeconds());
	float x = centeredPos.x + std::cos(blade.angle.asRadians()) * Visual::bladeRadius;
	float y = centeredPos.y + std::sin(blade.angle.asRadians()) * Visual::bladeRadius;

	blade.shape.setRotation(blade.angle);
	blade.shape.setPosition({ x, y });
}

void BladeDanceSpell::updateAllBlades(const sf::Time& deltaTime)
{
	for (auto& blade : mBlades)
		updateBlade(blade, deltaTime);
}
