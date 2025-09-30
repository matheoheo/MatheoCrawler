#include "pch.h"
#include "BeamSpell.h"
#include "Config.h"
#include "Utilities.h"
#include "Tile.h"
#include "TileMap.h"

BeamSpell::BeamSpell(const BeamData& beamData, const TileMap& tileMap, const std::vector<const Tile*>& affectedTiles)
	:mBeamData(beamData),
	mTileMap(tileMap),
	mAffectedTiles(affectedTiles),
	mTimeSinceStart(0),
	mIsComplete(false)
{
	createBeam();
}

void BeamSpell::update(const sf::Time& deltaTime, EventManager& eventManager)
{
	if (mIsComplete)
		return;

	addTimePassed(deltaTime);
	animateBeam();

	if (hasTimePassed())
	{
		onBeamFinish(eventManager);
		mIsComplete = true;
	}
}

void BeamSpell::render(sf::RenderWindow& window)
{
	window.draw(mOuter);
	window.draw(mInner);
}

bool BeamSpell::hasCompleted() const
{
	return mIsComplete;
}

float BeamSpell::getBeamLength() const
{
	return mBeamData.length * Config::getCellSize().x - Config::getCellSize().x;
}

sf::Vector2f BeamSpell::getBeamPos() const	
{
	auto casterCell = Utilities::getCellIndex(mBeamData.casterPos);
	sf::Vector2f pos{
		casterCell.x * Config::getCellSize().x,
		casterCell.y * Config::getCellSize().y
	};

	float halfCell = Config::getCellSize().x * 0.5f;

	switch (mBeamData.dir) {
	case Direction::Up:
		pos.x += halfCell;
		pos.y -= halfCell;
		break;
	case Direction::Left:
		pos.x -= halfCell;
		pos.y += halfCell;
		break;
	case Direction::Bottom:
		pos.x += halfCell;
		pos.y += Config::getCellSize().y + halfCell;
		break;
	case Direction::Right:
		pos.y += halfCell;
		pos.x += Config::getCellSize().x + halfCell;
		break;
	}

	return pos;
}

sf::Vector2f BeamSpell::getBeamDimension() const
{
	if (isVertical())
		return { BeamSpell::BeamSpellConfig::Thickness, getBeamLength() };
	
	return { getBeamLength(), BeamSpell::BeamSpellConfig::Thickness };
}

sf::Vector2f BeamSpell::getBeamOrigin(const sf::Vector2f& size) const
{
	auto dir = mBeamData.dir;
	switch (dir)
	{
	case Direction::Up:     return { size.x * 0.5f, size.y };
	case Direction::Left:   return { size.x, size.y * 0.5f };
	case Direction::Bottom: return { size.x * 0.5f, 0 };
	case Direction::Right:  return { 0, size.y * 0.5f };
	}

	return{};
}

sf::Vector2f BeamSpell::getInnerStartingSize() const
{
	if (isVertical())
		return { BeamSpell::BeamSpellConfig::Thickness, 0.f};

	return { 0.f, BeamSpell::BeamSpellConfig::Thickness };
}

void BeamSpell::updateInnerBeam(float length)
{
	sf::Vector2f size;
	if (isVertical())
		size = { BeamSpellConfig::Thickness, length };
	else
		size = { length, BeamSpellConfig::Thickness };
	
	mInner.setSize(size);
	mInner.setOrigin(getBeamOrigin(size));
}

bool BeamSpell::isVertical() const
{
	auto dir = mBeamData.dir;
	return dir == Direction::Up || dir == Direction::Bottom;
}

void BeamSpell::createBeam()
{
	auto pos = getBeamPos();
	auto size = getBeamDimension();
	auto origin = getBeamOrigin(size);

	mOuter.setSize(size);
	mOuter.setFillColor(mBeamData.outerColor);
	mOuter.setOrigin(origin);
	mOuter.setOutlineColor(sf::Color::Black);
	mOuter.setOutlineThickness(1.0f);
	mOuter.setPosition(pos);

	mInner.setSize(getInnerStartingSize());
	mInner.setFillColor(mBeamData.innerColor);
	mInner.setPosition(pos);

}

void BeamSpell::addTimePassed(const sf::Time& deltaTime)
{
	mTimeSinceStart += deltaTime.asMilliseconds();
}

float BeamSpell::getProgress() const
{
	float progress = static_cast<float>(mTimeSinceStart) / static_cast<float>(mBeamData.chargeTime);
	return std::clamp(progress, 0.f, 1.0f);
}

bool BeamSpell::hasTimePassed() const
{
	return mTimeSinceStart >= mBeamData.chargeTime;
}

void BeamSpell::animateBeam()
{
	float progress = getProgress();
	float desiredLength = getBeamLength();
	float currLength = progress * desiredLength;
	updateInnerBeam(currLength);
	pulseColor();
}

void BeamSpell::pulseColor()
{
	float timeInSec = mTimeSinceStart / 1000.f;
	float pulse = 0.5f + 0.3f * std::abs(std::sin(timeInSec * 10.f));
	sf::Color base = mBeamData.innerColor;

	sf::Color pulsingColor
	{
		static_cast<std::uint8_t>(base.r * pulse),
		static_cast<std::uint8_t>(base.g * pulse),
		static_cast<std::uint8_t>(base.b * pulse),
		base.a
	};

	mInner.setFillColor(pulsingColor);
}

void BeamSpell::onBeamFinish(EventManager& eventManager)
{
	//Beam is only casted by enemy entities.
	//That means, that on affectedTiles all we looking for is player only
	Entity* player = nullptr;
	for (const auto& tile : mAffectedTiles)
	{
		if (player)
			break;
	
		for (auto& ent : tile->occupyingEntities)
		{
			if (ent->hasComponent<PlayerComponent>())
			{
				player = ent;
			}
		}
	}
	if (!player)
		return;

	eventManager.notify<HitByProjectileEvent>(HitByProjectileEvent(*player, mBeamData.damage, true, false));
}
