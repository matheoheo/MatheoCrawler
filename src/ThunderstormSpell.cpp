#include "pch.h"
#include "ThunderstormSpell.h"
#include "Config.h"
#include "Utilities.h"
#include "SpellHolder.h"
#include "Tile.h"
#include "Random.h"

ThunderstormSpell::ThunderstormSpell(const Entity& caster, const TileMap& tileMap, const sf::Vector2f& castPos)
	:IAOESpell(caster, tileMap, castPos)
{
	mCastTime = Timing::spellDuration;
	createOnStartEffect();
}

void ThunderstormSpell::update(const sf::Time& deltaTime, EventManager& eventManager)
{
	addTimePassed(deltaTime);
	if (hasCastFinished())
	{
		makeDamage(eventManager);
		complete();
	}
	updateWarnings();
	updateBolts();
}

void ThunderstormSpell::render(sf::RenderWindow& window)
{
	for (const auto& warning : mWarnings)
		window.draw(warning.shape);
	renderBolts(window);
}

void ThunderstormSpell::createOnStartEffect()
{
	auto& offsets = SpellHolder::getInstance().get(SpellIdentifier::Thunderstorm).aoe->offsets;
	auto affectedTiles = getAffectedTiles(offsets, mCastPos);
	
	for (const Tile* tile : affectedTiles)
		createWarning(tile->tileVisual.getPosition());
}

void ThunderstormSpell::createWarning(const sf::Vector2f& pos)
{
	Warning warning;
	warning.tilePos = pos;

	auto& rect = warning.shape;
	rect.setSize(Visuals::startWarningSize);
	rect.setFillColor(Colors::startWarningColor);
	rect.setOrigin(Visuals::startWarningSize * 0.5f);
	rect.setPosition(pos + (Config::getCellSize() * 0.5f));

	mWarnings.push_back(std::move(warning));
}

void ThunderstormSpell::updateWarnings()
{
	auto progress = getWarningProgress();
	const sf::Vector2f desiredSize = Config::getCellSize();
	const sf::Vector2f newSize{ progress * desiredSize };
	const sf::Vector2f origin{ newSize * 0.5f };
	const sf::Color nextColor = Utilities::lerpColor(Colors::startWarningColor, Colors::targetWarningColor, progress);

	for (auto& warning : mWarnings)
	{
		warning.shape.setSize(newSize);
		warning.shape.setOrigin(origin);
		warning.shape.setPosition(warning.tilePos + (Config::getCellSize() * 0.5f));
		warning.shape.setFillColor(nextColor);
	}

	if (progress >= 1.0f) //maxProgress
	{
		for (auto& w : mWarnings)
			createBolt(w.tilePos);
		mWarnings.clear();
	}
}

float ThunderstormSpell::getWarningProgress() const
{
	return std::clamp(static_cast<float>(mTimePassed) / static_cast<float>(Timing::warningDuration), 0.f, 1.0f);
}

void ThunderstormSpell::createBolt(const sf::Vector2f& pos)
{
	constexpr int margin = 5;

	//Random::get doesn't take floats as arguments
	int minX = static_cast<int>(pos.x);
	int maxX = minX + static_cast<int>(Config::getCellSize().x);

	minX += margin;
	maxX -= margin;

	int minY = static_cast<int>(pos.y);
	int maxY = minY + static_cast<int>(Config::getCellSize().y);
	std::vector<sf::Vector2f> basePoints;
	for (int i = 0; i < Visuals::strikeBoltSegments; ++i)
	{
		const float t = static_cast<float>(i) / static_cast<float>(Visuals::strikeBoltSegments);
		float segmentX = static_cast<float>(Random::get(minX, maxX));
		float segmentY = pos.y + (t * Config::getCellSize().y);
		basePoints.emplace_back(segmentX, segmentY);
	}
	constexpr std::array<sf::Vector2f, 5> offsets =
	{ {
		{ 0.f,  0.f},
		{-1.0f, 0.f},
		{ 1.0f, 0.f},
		{ 0.f, -1.0f},
		{ 0.f,  1.0f}
	} };
	StrikeBolt strikeBolt;

	for (const auto& offset : offsets)
	{
		sf::VertexArray ver(sf::PrimitiveType::LineStrip);
		for (const auto& point : basePoints)
			ver.append(sf::Vertex(point + offset, Colors::strikeBoltColor));
		strikeBolt.thickBolts.push_back(std::move(ver));
	}
	mStrikeBolts.push_back(std::move(strikeBolt));
}

void ThunderstormSpell::updateBolts()
{
	constexpr std::uint8_t targetAlpha = 200;
	const float boltProgress = (mTimePassed - Timing::warningDuration) / static_cast<float>(Timing::strikeBoltDuration);
	const std::uint8_t currAlpha = static_cast<std::uint8_t>(targetAlpha * boltProgress);
	for (auto& strikeBolt : mStrikeBolts)
	{
		for (auto& bolt : strikeBolt.thickBolts)
		{
			for (size_t i = 0; i < bolt.getVertexCount(); ++i)
				bolt[i].color = sf::Color{ Colors::strikeBoltColor.r, Colors::strikeBoltColor.g, Colors::strikeBoltColor.b, currAlpha };
		}
	}
}

void ThunderstormSpell::renderBolts(sf::RenderWindow& window)
{
	for (const auto& bolt : mStrikeBolts)
		for (const auto& b : bolt.thickBolts)
			window.draw(b);
}

void ThunderstormSpell::makeDamage(EventManager& eventManager)
{
	SpellIdentifier id{ SpellIdentifier::Thunderstorm };
	const auto& offsets = SpellHolder::getInstance().get(id).aoe->offsets;
	auto affectedEnts = getAffectedEntities(offsets, mCastPos);
	IAOESpell::hitEntities(affectedEnts, id, eventManager);
}
